#include "AbstractLazy.h"

#include <algorithm>
#include <charconv>
#include <cctype>
#include <utility>

namespace LazyOrm {
namespace {

std::string join(const std::vector<std::string> &items, const std::string &delimiter)
{
    std::string result;
    for (size_t i = 0; i < items.size(); ++i) {
        if (i != 0) {
            result.append(delimiter);
        }
        result.append(items[i]);
    }
    return result;
}

std::string trim(std::string value)
{
    const auto first = value.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\n\r");
    return value.substr(first, last - first + 1);
}

DbVariant limitValue(const DbVariant &value)
{
    if (value.typeName() != "string") {
        return value;
    }

    const std::string text = trim(value.toString());
    long long number = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), number);
    if (error == std::errc() && end == text.data() + text.size()) {
        return number;
    }
    return value;
}

class ParameterizedBuilder
{
    AbstractLazy::ParameterDialect mDialect;
    ParameterizedQuery &mQuery;
    std::string mConditionAlias;

    bool postgreSQL() const { return mDialect == AbstractLazy::ParameterDialect::PostgreSQL; }
    bool mariaDB() const { return mDialect == AbstractLazy::ParameterDialect::MariaDB; }

    std::string bind(const DbVariant &value)
    {
        if (value.typeName() == "string") {
            const std::string text = value.toString();
            if (text.starts_with("[no']") || text.starts_with("[no`]")) {
                return text.substr(5);
            }
        }

        mQuery.params.push_back(value);
        if (postgreSQL()) {
            return "$" + std::to_string(mQuery.params.size());
        }
        return "?";
    }

    std::string quote(const DbVariant &identifier) const
    {
        return postgreSQL() ? identifier.setDoubleQuote() : identifier.setBackTick();
    }

    std::string identifier(const DbVariant &value) const
    {
        return mConditionAlias.empty() ? quote(value) : mConditionAlias + "." + quote(value);
    }

    static bool nullOperand(const DbVariant &value)
    {
        return value.typeName().empty() ||
               (value.typeName() == "string" && value.toLowerString() == "null");
    }

    std::string condition(const std::vector<DbVariant> &items)
    {
        if (items.size() == 1) {
            return items[0].toString();
        }

        if (items.size() == 2) {
            const bool isNull = nullOperand(items[1]);
            return identifier(items[0]) + (isNull ? " IS NULL" : " = " + bind(items[1]));
        }

        if (items.size() == 3) {
            const std::string operation = items[1].toLowerString();
            if ((operation == "is" || operation == "is not") && nullOperand(items[2])) {
                return identifier(items[0]) + " " + items[1].toString() + " NULL";
            }
            if ((operation == "in" || operation == "not in") && items[2].isList()) {
                std::vector<std::string> placeholders;
                for (const auto &value : items[2].toList()) {
                    placeholders.push_back(bind(value));
                }
                return identifier(items[0]) + " " + items[1].toString() + " (" + join(placeholders, ",") + ")";
            }
            return identifier(items[0]) + " " + items[1].toString() + " " + bind(items[2]);
        }

        if (items.size() == 5 && items[1].toLowerString() == "between") {
            const std::string lower = bind(items[2]);
            const std::string upper = bind(items[4]);
            return identifier(items[0]) + " " + items[1].toString() + " " + lower +
                   " " + items[3].toString() + " " + upper;
        }

        std::vector<std::string> fragments;
        for (const auto &item : items) {
            fragments.push_back(item.toString());
        }
        return join(fragments, " ");
    }

    std::string nestedFilter(const WhereFilter &filter)
    {
        if (!filter.nestedWhereFilters().empty()) {
            std::string result;
            for (const auto &child : filter.nestedWhereFilters()) {
                result.append(nestedFilter(child));
            }
            if (filter.nestedWhereFilters().size() > 1) {
                return "(" + result + ")";
            }
            return result;
        }

        std::string result;
        for (const auto &items : filter.nestedDbVariant()) {
            if (items.size() == 1) {
                result.append(" " + items[0].toString() + " ");
            } else {
                result.append("(" + condition(items) + ")");
            }
        }
        return result;
    }

    std::string filter(const WhereFilter &where, const std::string &leading)
    {
        if (where.empty()) {
            return {};
        }

        std::string body;
        if (!where.nestedWhereFilters().empty()) {
            for (const auto &child : where.nestedWhereFilters()) {
                body.append(nestedFilter(child));
            }
            if (where.nestedWhereFilters().size() > 1) {
                body = "(" + body + ")";
            }
        } else {
            for (const auto &items : where.nestedDbVariant()) {
                if (items.size() == 1) {
                    body.append(" " + items[0].toString() + " ");
                } else {
                    body.append(condition(items));
                }
            }
        }
        return leading + body + " ";
    }

    std::string limit(const FilterVariant &conditions)
    {
        std::vector<DbVariant> values;
        bool commaSyntax = false;
        std::visit([&](const auto &value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
                values = value;
                commaSyntax = values.size() == 3 && values[1].toString() == ",";
                if (commaSyntax) {
                    values = {values[0], values[2]};
                }
            } else if constexpr (std::is_same_v<T, DbVariant>) {
                const std::string text = value.toString();
                const auto comma = text.find(',');
                if (comma != std::string::npos) {
                    values = {trim(text.substr(0, comma)), trim(text.substr(comma + 1))};
                    commaSyntax = true;
                } else if (!text.empty()) {
                    values = {value};
                }
            }
        }, conditions);

        if (values.empty()) {
            return {};
        }

        if (commaSyntax && mariaDB() && values.size() == 2) {
            std::swap(values[0], values[1]);
        }

        std::string result = " LIMIT " + bind(limitValue(values[0]));
        if (values.size() >= 2) {
            result.append(" OFFSET " + bind(limitValue(values[1])));
        }
        return result.append(" ");
    }

public:
    ParameterizedBuilder(AbstractLazy::ParameterDialect dialect, ParameterizedQuery &query)
        : mDialect(dialect), mQuery(query)
    {
    }

    std::string value(const DbVariant &input)
    {
        return bind(input);
    }

    std::string where(const WhereFilter &filterValue, const std::string &leading = " WHERE ")
    {
        return filter(filterValue, leading);
    }

    std::string whereAliased(const WhereFilter &filterValue, const std::string &alias,
                             const std::string &leading = " WHERE ")
    {
        mConditionAlias = alias;
        const std::string result = filter(filterValue, leading);
        mConditionAlias.clear();
        return result;
    }

    std::string having(const HavingFilter &filterValue)
    {
        return filter(filterValue, " HAVING ");
    }

    std::string pagination(const FilterVariant &conditions)
    {
        return limit(conditions);
    }

    std::string insertValues(const std::map<DbVariant, DbVariant> &properties, bool skipUpdates,
                             std::vector<std::string> *columns = nullptr)
    {
        std::vector<std::string> placeholders;
        for (const auto &[key, value] : properties) {
            if (skipUpdates && key.isUpdate()) {
                continue;
            }
            if (columns != nullptr) {
                columns->push_back(quote(key));
            }
            placeholders.push_back(bind(value));
        }
        return "(" + join(placeholders, ",") + ")";
    }

    std::string updates(const std::map<DbVariant, DbVariant> &properties, bool onlyMarked)
    {
        std::vector<std::string> assignments;
        for (const auto &[key, value] : properties) {
            if (onlyMarked && !key.isUpdate()) {
                continue;
            }
            assignments.push_back(quote(key) + "=" + bind(value));
        }
        return join(assignments, ",");
    }

    std::string identifiers(const std::map<DbVariant, DbVariant> &properties)
    {
        std::vector<std::string> columns;
        for (const auto &[key, value] : properties) {
            columns.push_back(key.toString() == "*" ? "*" : quote(key));
        }
        return join(columns, ",");
    }

    std::string conflictKeys(const std::vector<std::string> &keys)
    {
        std::vector<std::string> columns;
        for (const auto &key : keys) {
            columns.push_back(quote(key));
        }
        return join(columns, ",");
    }
};

}

ParameterizedQuery AbstractLazy::parameterizedQuery(ParameterDialect dialect) const
{
    ParameterizedQuery result;
    ParameterizedBuilder builder(dialect, result);
    const bool postgreSQL = dialect == ParameterDialect::PostgreSQL;
    const bool mariaDB = dialect == ParameterDialect::MariaDB;
    const bool sqlite = dialect == ParameterDialect::SQLite;
    const auto appendFilters = [&]() {
        result.sql.append(getFilter().groupByString());
        result.sql.append(builder.having(mHavingFilter));
        result.sql.append(getFilter().orderByString());
        result.sql.append(builder.pagination(getFilter().limitConditions()));
    };

    switch (mQueryType) {
    case INSERT:
    case INSERT_IGNORE: {
        std::vector<std::string> columns;
        if (mQueryType == INSERT_IGNORE) {
            result.sql = mariaDB ? "INSERT IGNORE INTO " : (sqlite ? "INSERT OR IGNORE INTO " : "INSERT INTO ");
        } else {
            result.sql = "INSERT INTO ";
        }
        result.sql.append(mTabeName + " (");
        const std::string values = builder.insertValues(mProperties, false, &columns);
        result.sql.append(join(columns, ",") + ") VALUES " + values + " ");
        if (postgreSQL && mQueryType == INSERT_IGNORE) {
            result.sql.append("ON CONFLICT DO NOTHING");
            if (!mPrimaryKey.empty()) {
                result.sql.append(" returning " + mPrimaryKey);
            }
        } else if (postgreSQL && !mPrimaryKey.empty()) {
            result.sql.append(" returning " + mPrimaryKey);
        }
        result.sql.append(";");
        return result;
    }
    case INSERT_UPDATE: {
        std::vector<std::string> columns;
        result.sql = "INSERT INTO " + mTabeName + " (";
        const std::string values = builder.insertValues(mProperties, true, &columns);
        result.sql.append(join(columns, ",") + ") VALUES " + values + " ");
        const bool conflictSyntax = postgreSQL || sqlite;
        const std::string updates = (mariaDB || !mUniqueKeys.empty()) ? builder.updates(mProperties, true) : "";
        if (!updates.empty() && (conflictSyntax ? !mUniqueKeys.empty() : true)) {
            result.sql.append(conflictSyntax ? "ON CONFLICT (" : "ON DUPLICATE KEY UPDATE ");
            if (conflictSyntax) {
                result.sql.append(builder.conflictKeys(mUniqueKeys) + ") DO UPDATE SET ");
            }
            result.sql.append(updates);
        }
        result.sql.append(";");
        return result;
    }
    case BULK_INSERT: {
        if (mBatchProperties.empty()) {
            return {};
        }

        const auto &firstRow = mBatchProperties.front();
        if (firstRow.empty()) {
            return {};
        }
        std::vector<std::string> columns;
        std::vector<std::string> columnNames;
        for (const auto &[key, value] : firstRow) {
            columns.push_back(postgreSQL ? key.setDoubleQuote() : key.setBackTick());
            columnNames.push_back(key.toCleanString());
        }

        std::vector<std::string> rows;
        for (const auto &row : mBatchProperties) {
            if (row.size() != firstRow.size()) {
                return {};
            }
            auto column = columnNames.begin();
            std::vector<std::string> placeholders;
            for (const auto &[key, value] : row) {
                if (column == columnNames.end() || key.toCleanString() != *column) {
                    return {};
                }
                ++column;
                placeholders.push_back(builder.value(value));
            }
            rows.push_back("(" + join(placeholders, ",") + ")");
        }

        result.sql = "INSERT INTO " + mTabeName + " (" + join(columns, ",") + ") VALUES " +
                     join(rows, ",") + " ;";
        return result;
    }
    case BULK_UPDATE: {
        std::vector<DbVariant> columns, updateColumns;
        if (!prepareBulkUpdate(columns, updateColumns)) {
            return {};
        }

        const QuoteType tableQuote = postgreSQL ? DoubleQuote : BackTick;
        const std::string targetTable = bulkUpdateTargetTable(tableQuote);
        const std::string targetRef = bulkUpdateTargetRef(tableQuote);
        const auto quoteIdentifier = [&](const DbVariant &identifier) {
            return postgreSQL ? identifier.setDoubleQuote() : identifier.setBackTick();
        };
        const std::string quotedStaging = quoteIdentifier(DbVariant(bulkUpdateStagingName()));
        const std::string quotedPrimaryKey = quoteIdentifier(DbVariant(mPrimaryKey));

        std::vector<std::string> quotedColumns;
        for (const auto &column : columns) {
            quotedColumns.push_back(quoteIdentifier(column));
        }

        // Batch values are bound row-major; the WHERE filter values follow.
        std::vector<std::string> rows;
        for (const auto &row : mBatchProperties) {
            std::vector<std::string> placeholders;
            for (const auto &[key, value] : row) {
                placeholders.push_back(builder.value(value));
            }
            rows.push_back("(" + join(placeholders, ",") + ")");
        }

        std::string dropSql;
        std::string sql;
        if (postgreSQL) {
            dropSql = "DROP TABLE IF EXISTS pg_temp." + quotedStaging + " ;";
            sql = dropSql;
            sql.append(" CREATE TEMPORARY TABLE " + quotedStaging + " (LIKE " + targetTable + ") ;");
        } else if (mariaDB) {
            dropSql = "DROP TEMPORARY TABLE IF EXISTS " + quotedStaging + " ;";
            sql = dropSql;
            sql.append(" CREATE TEMPORARY TABLE " + quotedStaging + " LIKE " + targetTable + " ;");
        } else {
            dropSql = "DROP TABLE IF EXISTS temp." + quotedStaging + " ;";
            sql = dropSql;
            sql.append(" CREATE TEMPORARY TABLE " + quotedStaging + " (" + join(quotedColumns, ",") + ") ;");
        }

        sql.append(" INSERT INTO " + quotedStaging + " (" + join(quotedColumns, ",") +
                   ") VALUES " + join(rows, ",") + " ;");

        if (postgreSQL) {
            std::vector<std::string> assignments;
            for (const auto &column : updateColumns) {
                const std::string quotedColumn = quoteIdentifier(column);
                assignments.push_back(quotedColumn + " = u." + quotedColumn);
            }
            sql.append(" UPDATE " + targetTable + " t SET " + join(assignments, ",") +
                       " FROM " + quotedStaging + " u WHERE t." + quotedPrimaryKey +
                       " = u." + quotedPrimaryKey);
            sql.append(builder.whereAliased(mWhereFilter, "t", " AND "));
            sql.append(" ;");
        } else if (mariaDB) {
            std::vector<std::string> assignments;
            for (const auto &column : updateColumns) {
                const std::string quotedColumn = quoteIdentifier(column);
                assignments.push_back("t." + quotedColumn + " = u." + quotedColumn);
            }
            sql.append(" UPDATE " + targetTable + " t JOIN " + quotedStaging +
                       " u ON t." + quotedPrimaryKey + " = u." + quotedPrimaryKey +
                       " SET " + join(assignments, ","));
            sql.append(builder.whereAliased(mWhereFilter, "t"));
            sql.append(" ;");
        } else {
            // SQLite has no UPDATE ... JOIN and UPDATE ... FROM needs 3.33+;
            // correlated subqueries work on every SQLite 3.x version. The
            // EXISTS guard ensures target rows absent from the staging table
            // are never overwritten with NULL.
            std::vector<std::string> correlated;
            for (const auto &column : updateColumns) {
                const std::string quotedColumn = quoteIdentifier(column);
                correlated.push_back(quotedColumn + " = (SELECT u." + quotedColumn +
                                     " FROM " + quotedStaging + " u WHERE u." + quotedPrimaryKey +
                                     " = " + targetRef + "." + quotedPrimaryKey + ")");
            }
            sql.append(" UPDATE " + targetTable + " SET " + join(correlated, ","));
            sql.append(" WHERE EXISTS (SELECT 1 FROM " + quotedStaging + " u WHERE u." +
                       quotedPrimaryKey + " = " + targetRef + "." + quotedPrimaryKey + ")");
            sql.append(mWhereFilter.empty() ? " " : builder.where(mWhereFilter, " AND "));
            sql.append(" ;");
        }

        sql.append(" " + dropSql);

        result.sql = sql;
        return result;
    }
    case SELECT:
    case SELECT_DISTINCT: {
        result.sql = mQueryType == SELECT_DISTINCT ? "SELECT DISTINCT " : "SELECT ";
        result.sql.append(builder.identifiers(mProperties));
        if (!mCounts.empty()) {
            std::vector<std::string> counts;
            for (const auto &count : count_queries()) {
                counts.push_back(count);
            }
            if (!mProperties.empty()) {
                result.sql.append(",");
            }
            result.sql.append(join(counts, ","));
        }
        result.sql.append(" FROM " + mTabeName);
        result.sql.append(builder.where(mWhereFilter));
        appendFilters();
        result.sql.append(";");
        return result;
    }
    case UPDATE:
        result.sql = "UPDATE " + mTabeName + " SET " + builder.updates(mProperties, false) + " ";
        result.sql.append(builder.where(mWhereFilter));
        appendFilters();
        result.sql.append(";");
        return result;
    case DELETE:
        result.sql = "DELETE FROM " + mTabeName + " ";
        result.sql.append(builder.where(mWhereFilter));
        appendFilters();
        result.sql.append(" ;");
        return result;
    case CONTAINS:
        result.sql = "SELECT CASE WHEN COUNT(*) > 0 THEN TRUE ELSE FALSE END AS \"contains\" FROM " + mTabeName;
        result.sql.append(builder.where(mWhereFilter));
        result.sql.append(";");
        return result;
    case COUNT:
        result.sql = "SELECT COUNT(*) AS \"count\" FROM " + mTabeName;
        result.sql.append(builder.where(mWhereFilter));
        result.sql.append(";");
        return result;
    case RAW_QUERY: {
        result.sql = mTabeName;
        std::vector<DbVariant> values;
        for (const auto &[key, value] : mProperties) {
            const bool keyParameter = value.typeName() == "string" && value.toString().empty();
            values.push_back(keyParameter ? key : value);
        }
        if (!postgreSQL) {
            result.params = std::move(values);
            return result;
        }

        size_t largestIndex = 0;
        for (size_t position = 0; position < result.sql.size(); ++position) {
            if (result.sql[position] != '$' || position + 1 >= result.sql.size() ||
                !std::isdigit(static_cast<unsigned char>(result.sql[position + 1]))) {
                continue;
            }
            size_t index = 0;
            const auto [end, error] = std::from_chars(result.sql.data() + position + 1,
                                                       result.sql.data() + result.sql.size(), index);
            if (error == std::errc()) {
                largestIndex = std::max(largestIndex, index);
                position = static_cast<size_t>(end - result.sql.data()) - 1;
            }
        }
        result.params.resize(largestIndex);
        for (size_t i = 0; i < largestIndex && i < values.size(); ++i) {
            result.params[i] = values[i];
        }
        return result;
    }
    default:
        return {};
    }
}

}
