#include "AbstractLazy.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <set>
#include <sstream>

namespace LazyOrm {

const char *AbstractLazy::version = lazy_orm_version;

void AbstractLazy::copyStateFrom(const AbstractLazy &other)
{
  mQueryType = other.mQueryType;
  mTabeName = other.mTabeName;
  mProperties = other.mProperties;
  mBatchProperties = other.mBatchProperties;
  mPrimaryKey = other.mPrimaryKey;
  mUniqueKeys = other.mUniqueKeys;
  mCounts = other.mCounts;
  // WhereFilter::operator= appends instead of replacing (existing public
  // behavior used when building filters). Copy-construct the nested filter
  // vectors and swap them in, so assignment faithfully replaces any previous
  // filter state instead of merging into it.
  mWhereFilter.mNestedDbVariant = other.mWhereFilter.mNestedDbVariant;
  std::vector<WhereFilter> whereFilters(other.mWhereFilter.mNestedWhereFilters.begin(),
                                        other.mWhereFilter.mNestedWhereFilters.end());
  mWhereFilter.mNestedWhereFilters.swap(whereFilters);
  mHavingFilter.mNestedDbVariant = other.mHavingFilter.mNestedDbVariant;
  std::vector<WhereFilter> havingFilters(other.mHavingFilter.mNestedWhereFilters.begin(),
                                         other.mHavingFilter.mNestedWhereFilters.end());
  mHavingFilter.mNestedWhereFilters.swap(havingFilters);
}

void AbstractLazy::setTabeName(const std::string &name)
{
  mTabeName = name;
}


//std::string AbstractLazy::operator[](const std::string &key) const
//{

//  return "";
//}


DbVariant &AbstractLazy::operator[](const std::string &key)
{
    return mProperties[key];
}

void AbstractLazy::setUniqueKeys(const std::initializer_list<std::string> &uniqueKeys)
{
    mUniqueKeys = uniqueKeys;
}

std::vector<std::string> & AbstractLazy::operator[](const LazyOrm::Unique_Keys &uniqueKeys){
    return mUniqueKeys;
}

void AbstractLazy::enableDistinctSelect(bool distinct){
    if(mQueryType == SELECT and distinct){
        mQueryType = SELECT_DISTINCT;
    }
    else if(mQueryType == SELECT_DISTINCT and !distinct){
        mQueryType = SELECT;
    }
}

void AbstractLazy::setCountType(std::initializer_list<LazyOrm::DbVariant> countFields)
{
    mCounts = countFields;
}

std::list<LazyOrm::DbVariant> &AbstractLazy::operator[](const Count &count){
    return mCounts;
}

void AbstractLazy::setQueryType(LazyOrm::Query queryType)
{
  mQueryType = queryType;
}

std::string & AbstractLazy::operator[](const Query &queryType)
{
  mQueryType = queryType;
  return mTabeName;
}

std::string AbstractLazy::tabeName() const
{
    return mTabeName;
}

std::map<DbVariant, DbVariant> AbstractLazy::properties() const
{
    return mProperties;
}

std::list<std::map<DbVariant, DbVariant> > AbstractLazy::batchProperties() const
{
    return mBatchProperties;
}

Query AbstractLazy::queryType() const
{
    return mQueryType;
}

std::string AbstractLazy::string_join(const std::string &delimiter, const std::vector<std::string> &container, QuoteType quote) const
{
  size_t size = container.size();
  size_t endPos = container.size()-1;
  std::string output;
  for(size_t i = 0; i < size; ++i) {
      switch (quote) {
      case SingleQuote:
          output.append("'"+container[i]+"'");
          break;
      case DoubleQuote:
          output.append("\""+container[i]+"\"");
          break;
      case BackTick:
          output.append("`"+container[i]+"`");
          break;
      case NoQuote:
      default:
          output.append(container[i]);
          break;
      }
      if(i!=endPos){
          output.append(delimiter);
      }
  }
  return output;
}

std::string AbstractLazy::string_join(const std::string &delimiter, const std::vector<DbVariant> &container, QuoteType quote) const
{
  size_t size = container.size();
  size_t endPos = container.size()-1;
  std::string output;
  for(size_t i = 0; i < size; ++i) {
      switch (quote) {
      case SingleQuote:
          output.append(container[i].setQuote());
          break;
      case DoubleQuote:
          output.append(container[i].setDoubleQuote());
          break;
      case BackTick:
          output.append(container[i].setBackTick());
          break;
      case NoQuote:
      default:
          output.append(container[i].toString());
          break;
      }
      if(i!=endPos){
          output.append(delimiter);
      }
  }
  return output;
}

bool AbstractLazy::appendPropAsCount(DbVariant prop)
{
    std::string countKey = prop.toString();
    countKey.erase(std::remove_if(countKey.begin(), countKey.end(), ::isspace), countKey.end());
    if(DbVariant(countKey).startsWith("count(", false)){
        std::string propStr = prop.toString();
        size_t start = propStr.find('(');
        size_t end = propStr.find(')');
        if (start != std::string::npos && end != std::string::npos && start < end) {
            mCounts.push_back( propStr.substr(start + 1, end - start - 1) );
            return true;
        }
    }

    return false;
}

namespace {

bool isValidBulkIdentifierComponent(const std::string &component)
{
    if(component.empty())
    {
        return false;
    }
    const unsigned char first = static_cast<unsigned char>(component.front());
    if(!(std::isalpha(first) || first == '_'))
    {
        return false;
    }
    return std::all_of(component.begin() + 1, component.end(), [](unsigned char ch){
        return std::isalnum(ch) || ch == '_';
    });
}

bool parseBulkTableIdentifier(const std::string &tableName,
                              std::string &schema,
                              std::string &table)
{
    schema.clear();
    table.clear();
    if(tableName.empty())
    {
        return false;
    }

    const size_t dot = tableName.find('.');
    if(dot == std::string::npos)
    {
        if(!isValidBulkIdentifierComponent(tableName))
        {
            return false;
        }
        table = tableName;
        return true;
    }

    // Support at most one schema qualifier: schema.table.
    if(tableName.find('.', dot + 1) != std::string::npos)
    {
        return false;
    }

    const std::string schemaPart = tableName.substr(0, dot);
    const std::string tablePart = tableName.substr(dot + 1);
    if(!isValidBulkIdentifierComponent(schemaPart) || !isValidBulkIdentifierComponent(tablePart))
    {
        return false;
    }

    schema = schemaPart;
    table = tablePart;
    return true;
}

std::string quoteBulkComponent(const std::string &component, int quoteType)
{
    switch (quoteType) {
    case 2: // DoubleQuote
        return "\"" + component + "\"";
    case 3: // BackTick
        return "`" + component + "`";
    default:
        return component;
    }
}

std::string fnv1aHex8(const std::string &input)
{
    std::uint64_t hash = 14695981039346656037ULL;
    for(unsigned char ch : input)
    {
        hash ^= ch;
        hash *= 1099511628211ULL;
    }
    std::ostringstream oss;
    oss << std::hex << std::setw(8) << std::setfill('0')
        << static_cast<std::uint32_t>(hash ^ (hash >> 32));
    return oss.str();
}

bool isDisallowedBulkRawMarker(const std::string &text)
{
    return text.starts_with("[no']") ||
           text.starts_with("[no`]") ||
           text.starts_with("[func]") ||
           text.starts_with("[update]");
}

bool isValidBulkValue(const DbVariant &value)
{
    const std::string type = value.typeName();
    if(type == "list")
    {
        return false;
    }
    if(type == "string")
    {
        const std::string text = value.toString();
        if(text.starts_with("[no']") || text.starts_with("[no`]"))
        {
            return false;
        }
    }
    return true;
}

bool isValidBulkWhereOperator(const std::string &op)
{
    std::string lower;
    lower.reserve(op.size());
    for(unsigned char ch : op)
    {
        lower.push_back(static_cast<char>(std::tolower(ch)));
    }
    return lower == "=" || lower == "!=" || lower == "<>" ||
           lower == "<" || lower == "<=" || lower == ">" || lower == ">=" ||
           lower == "like" || lower == "not like" ||
           lower == "ilike" || lower == "not ilike" ||
           lower == "in" || lower == "not in" ||
           lower == "is" || lower == "is not";
}

bool isValidBulkLogicalOperator(const std::string &op)
{
    std::string upper;
    upper.reserve(op.size());
    for(unsigned char ch : op)
    {
        upper.push_back(static_cast<char>(std::toupper(ch)));
    }
    return upper == "AND" || upper == "OR" || upper == "NOT" ||
           upper == "AND NOT" || upper == "OR NOT";
}

bool isValidBulkWhereOperand(const DbVariant &operand)
{
    const std::string type = operand.typeName();
    if(type == "string")
    {
        return !isDisallowedBulkRawMarker(operand.toString());
    }
    if(type == "list")
    {
        for(const auto &elem : operand.toList())
        {
            if(elem.typeName() == "string" && isDisallowedBulkRawMarker(elem.toString()))
            {
                return false;
            }
        }
    }
    return true;
}

bool isValidBulkWhere(const WhereFilter &filter)
{
    if(!filter.nestedWhereFilters().empty())
    {
        for(const auto &child : filter.nestedWhereFilters())
        {
            if(!isValidBulkWhere(child))
            {
                return false;
            }
        }
        return true;
    }

    for(const auto &items : filter.nestedDbVariant())
    {
        if(items.empty())
        {
            return false;
        }
        if(items.size() == 1)
        {
            if(items[0].typeName() != "string" || !isValidBulkLogicalOperator(items[0].toString()))
            {
                return false;
            }
            continue;
        }
        if(items[0].typeName() != "string" || !isValidBulkIdentifierComponent(items[0].toString()))
        {
            return false;
        }
        if(items.size() == 2)
        {
            if(!isValidBulkWhereOperand(items[1]))
            {
                return false;
            }
            continue;
        }
        if(items.size() == 3)
        {
            if(items[1].typeName() != "string" ||
               !isValidBulkWhereOperator(items[1].toString()) ||
               !isValidBulkWhereOperand(items[2]))
            {
                return false;
            }
            continue;
        }
        if(items.size() == 5 &&
           items[1].typeName() == "string" && items[1].toLowerString() == "between" &&
           items[3].typeName() == "string" && items[3].toLowerString() == "and")
        {
            if(!isValidBulkWhereOperand(items[2]) || !isValidBulkWhereOperand(items[4]))
            {
                return false;
            }
            continue;
        }
        return false;
    }

    return true;
}

std::string escapeBulkSingleQuotes(const std::string &in)
{
    std::string out;
    out.reserve(in.size());
    size_t bsRun = 0;
    for(char ch : in)
    {
        if(ch == '\\')
        {
            out.push_back('\\');
            ++bsRun;
        }
        else if(ch == '\'')
        {
            if(bsRun % 2 == 0)
            {
                out.push_back('\\');
            }
            out.push_back('\'');
            bsRun = 0;
        }
        else
        {
            out.push_back(ch);
            bsRun = 0;
        }
    }
    return out;
}

// Primary-key contract for Bulk UPDATE:
// All primary-key values in a batch must belong to a single SQL type category
// (Integer, Float, String, Boolean, or Blob). Mixing categories (e.g. int 1
// and string "1", or int 1 and double 1.0) is rejected conservatively because
// staging-table column coercion at the database can collapse mixed C++ types
// into duplicate keys and cause nondeterministic UPDATE FROM / JOIN results.
enum class BulkPrimaryKeyKind { Integer, Float, String, Boolean, Blob };

using BulkPrimaryKey = std::variant<std::string, long long, unsigned long long, long double, bool, BlobType>;

bool toBulkPrimaryKey(const DbVariant &value, BulkPrimaryKeyKind &kind, BulkPrimaryKey &out)
{
    const std::string type = value.typeName();
    if(type.empty() || type == "list")
    {
        return false;
    }
    if(type == "string")
    {
        if(value.toLowerString() == "null")
        {
            return false;
        }
        kind = BulkPrimaryKeyKind::String;
        out = value.toString();
        return true;
    }
    if(type == "int")
    {
        kind = BulkPrimaryKeyKind::Integer;
        const long long signedVal = value.toLongLong();
        if(signedVal < 0)
        {
            out = signedVal;
        }
        else
        {
            out = static_cast<unsigned long long>(signedVal);
        }
        return true;
    }
    if(type == "uint")
    {
        kind = BulkPrimaryKeyKind::Integer;
        out = value.toULongLong();
        return true;
    }
    if(type == "float")
    {
        long double floatVal = value.toLongDouble();
        if(std::isnan(floatVal))
        {
            return false;
        }
        if(floatVal == 0.0L)
        {
            floatVal = 0.0L;
        }
        kind = BulkPrimaryKeyKind::Float;
        out = floatVal;
        return true;
    }
    if(type == "boolean")
    {
        kind = BulkPrimaryKeyKind::Boolean;
        out = value.toBool();
        return true;
    }
    if(type == "blob")
    {
        kind = BulkPrimaryKeyKind::Blob;
        out = value.toBlob();
        return true;
    }
    return false;
}

} // namespace

bool AbstractLazy::prepareBulkUpdate(std::vector<DbVariant> &columns,
                                     std::vector<DbVariant> &updateColumns) const
{
    columns.clear();
    updateColumns.clear();

    std::string schema, table;
    if(!parseBulkTableIdentifier(mTabeName, schema, table) ||
       !isValidBulkIdentifierComponent(mPrimaryKey) ||
       mBatchProperties.empty() ||
       !isValidBulkWhere(mWhereFilter))
    {
        return false;
    }

    const auto &firstRow = mBatchProperties.front();
    if(firstRow.empty())
    {
        return false;
    }

    for(const auto &[key, value] : firstRow)
    {
        if(key.typeName() != "string" || !isValidBulkIdentifierComponent(key.toString()))
        {
            return false;
        }
        columns.push_back(key);
    }

    const bool hasPrimaryKey = std::any_of(columns.begin(), columns.end(),
        [&](const DbVariant &column){ return column.toString() == mPrimaryKey; });
    if(!hasPrimaryKey)
    {
        return false;
    }

    if(columns.size() < 2)
    {
        // Primary key only: nothing to update.
        return false;
    }

    for(const auto &row : mBatchProperties)
    {
        if(row.size() != columns.size())
        {
            return false;
        }
        auto column = columns.begin();
        for(const auto &[key, value] : row)
        {
            if(key.typeName() != "string" ||
               key.toString() != column->toString() ||
               !isValidBulkValue(value))
            {
                return false;
            }
            ++column;
        }
    }

    // Reject NULL, mixed-type, or duplicate primary-key values.
    bool hasPrimaryKeyKind = false;
    BulkPrimaryKeyKind expectedKind = BulkPrimaryKeyKind::Integer;
    std::set<BulkPrimaryKey> seenPrimaryKeys;
    for(const auto &row : mBatchProperties)
    {
        for(const auto &[key, value] : row)
        {
            if(key.toString() == mPrimaryKey)
            {
                BulkPrimaryKeyKind kind;
                BulkPrimaryKey pk;
                if(!toBulkPrimaryKey(value, kind, pk))
                {
                    return false;
                }
                if(!hasPrimaryKeyKind)
                {
                    expectedKind = kind;
                    hasPrimaryKeyKind = true;
                }
                else if(kind != expectedKind)
                {
                    return false;
                }
                if(!seenPrimaryKeys.insert(pk).second)
                {
                    return false;
                }
                break;
            }
        }
    }

    for(const auto &column : columns)
    {
        if(column.toString() != mPrimaryKey)
        {
            updateColumns.push_back(column);
        }
    }

    return true;
}

std::string AbstractLazy::bulkUpdateStagingName() const
{
    std::string schema, table;
    if(!parseBulkTableIdentifier(mTabeName, schema, table))
    {
        return {};
    }

    std::string name = "lazyormtemp_";
    if(!schema.empty())
    {
        name.append(schema).append("_");
    }
    name.append(table);

    // Keep within PostgreSQL's 63-byte NAMEDATALEN-1 limit (also valid in
    // MariaDB's 64-char limit and SQLite).
    constexpr size_t maxIdentifierLen = 63;
    if(name.size() > maxIdentifierLen)
    {
        name = name.substr(0, maxIdentifierLen - 9) + "_" + fnv1aHex8(mTabeName);
    }
    return name;
}

std::string AbstractLazy::bulkUpdateTargetTable(QuoteType quote) const
{
    std::string schema, table;
    if(!parseBulkTableIdentifier(mTabeName, schema, table))
    {
        return {};
    }
    if(schema.empty())
    {
        return quoteBulkComponent(table, static_cast<int>(quote));
    }
    return quoteBulkComponent(schema, static_cast<int>(quote)) + "." +
           quoteBulkComponent(table, static_cast<int>(quote));
}

std::string AbstractLazy::bulkUpdateTargetRef(QuoteType quote) const
{
    std::string schema, table;
    if(!parseBulkTableIdentifier(mTabeName, schema, table))
    {
        return {};
    }
    return quoteBulkComponent(table, static_cast<int>(quote));
}

std::string AbstractLazy::bulkUpdateValue(const DbVariant &value) const
{
    const std::string type = value.typeName();
    if(type.empty())
    {
        return "NULL";
    }
    if(type == "string")
    {
        return "'" + escapeBulkSingleQuotes(value.toString()) + "'";
    }
    if(type == "boolean")
    {
        return value.toBool() ? "true" : "false";
    }
    return value.setQuote();
}

namespace {

enum class BulkIdentifierQuote { None, Double, BackTick };

std::string bulkUpdateIdentifier(const DbVariant &identifier, const std::string &alias,
                                 BulkIdentifierQuote quote)
{
    std::string quoted;
    switch (quote) {
    case BulkIdentifierQuote::Double:
        quoted = identifier.setDoubleQuote();
        break;
    case BulkIdentifierQuote::BackTick:
        quoted = identifier.setBackTick();
        break;
    default:
        quoted = identifier.toString();
        break;
    }
    return alias.empty() ? quoted : alias + "." + quoted;
}

// Mirrors WhereFilter::string_join() but qualifies identifiers with `alias`.
std::string bulkUpdateCondition(const std::vector<DbVariant> &container, const std::string &alias,
                                BulkIdentifierQuote quote)
{
    if(container.size()==1){
        return container[0].toString();
    }

    if(container.size()==2){
        const std::string &equalityOperator = container[1].toLowerString()=="null"? "IS" : "=";
        return bulkUpdateIdentifier(container[0], alias, quote) + " " + equalityOperator + " " + container[1].setQuote();
    }

    if(container.size()==3){
        return bulkUpdateIdentifier(container[0], alias, quote) + " " + container[1].toString() + " " + container[2].setQuote();
    }

    if(container.size()==5 && container[1].toLowerString()=="between"){
        return bulkUpdateIdentifier(container[0], alias, quote) + " " + container[1].toString() + " " + container[2].setQuote() + " " + container[3].toString() + " " + container[4].setQuote();
    }

    size_t size = container.size();
    size_t endPos = container.size()-1;
    std::string output;
    for(size_t i = 0; i < size; ++i) {
        output.append(container[i].toString());
        if(i!=endPos){
            output.append(" ");
        }
    }
    return output;
}

// Mirrors WhereFilter::toString(WhereFilter) with aliased identifiers,
// continuing traversal across all sibling conditions and logical operators.
std::string bulkUpdateNestedWhere(const WhereFilter &filter, const std::string &alias,
                                  BulkIdentifierQuote quote)
{
    if(!filter.nestedWhereFilters().empty())
    {
        std::string retStr;
        for (const auto& whereFilter : filter.nestedWhereFilters()) {
            retStr.append( bulkUpdateNestedWhere(whereFilter, alias, quote) );
        }
        if(filter.nestedWhereFilters().size() > 1){
            return "(" + retStr + ")";
        }
        return retStr;
    }

    std::string retStr;
    for (const auto& nestedVectorVariant : filter.nestedDbVariant()) {
        if(nestedVectorVariant.size() == 1)
        {
            retStr.append(" " + nestedVectorVariant.at(0).toString() + " ");
        }
        else
        {
            retStr.append("(");
            retStr.append(bulkUpdateCondition(nestedVectorVariant, alias, quote));
            retStr.append(")");
        }
    }
    return retStr;
}

}

std::string AbstractLazy::bulkUpdateWhereString(const std::string &alias,
                                                const std::string &leading,
                                                QuoteType quote) const
{
    if(mWhereFilter.empty())
    {
        return {};
    }

    BulkIdentifierQuote identifierQuote = BulkIdentifierQuote::None;
    switch (quote) {
    case DoubleQuote:
        identifierQuote = BulkIdentifierQuote::Double;
        break;
    case BackTick:
        identifierQuote = BulkIdentifierQuote::BackTick;
        break;
    default:
        break;
    }

    std::string body;
    if(!mWhereFilter.nestedWhereFilters().empty()){
        for (const auto& whereFilter : mWhereFilter.nestedWhereFilters()) {
            body.append(bulkUpdateNestedWhere(whereFilter, alias, identifierQuote));
        }
        if(mWhereFilter.nestedWhereFilters().size() > 1){
            body = "(" + body + ")";
        }
    }
    else if(!mWhereFilter.nestedDbVariant().empty()){
        for (const auto& nestedVectorVariant : mWhereFilter.nestedDbVariant()) {
            if(nestedVectorVariant.size() == 1){
                body.append(" " + nestedVectorVariant.at(0).toString() + " ");
            } else {
                body.append("(");
                body.append(bulkUpdateCondition(nestedVectorVariant, alias, identifierQuote));
                body.append(")");
            }
        }
    }

    return leading + body + " ";
}


std::string AbstractLazy::contains_query() const
{
    std::string queryString = "SELECT CASE WHEN COUNT(*) > 0 THEN TRUE ELSE FALSE END AS \"contains\" FROM ";
    queryString.append(mTabeName);
    queryString.append(mWhereFilter.toString());
    queryString.append(";");
    return queryString;
}

std::string AbstractLazy::count_query() const
{
    std::string queryString = "SELECT COUNT(*) AS \"count\" FROM ";
    queryString.append(mTabeName);
    queryString.append(mWhereFilter.toString());
    queryString.append(";");
    return queryString;
}

// std::string AbstractLazy::raw_query() const
// {
//     std::vector<DbVariant> keys;
//     for(const auto &[key, value] : mProperties)
//     {
//         if(key.toString()=="*")
//         {
//             keys.push_back(key);
//         }
//         else
//         {
//             keys.push_back(key.setQuote());
//         }
//     }

//     std::string queryString(mTabeName);

//     return [&queryString, &keys] {
//         std::ostringstream oss;
//         size_t pos = 0, last = 0, idx = 0;
//         while ((pos = queryString.find('?', last)) != std::string::npos && idx < keys.size()) {
//             oss << queryString.substr(last, pos - last) << keys[idx++].toString();
//             last = pos + 1;
//         }
//         oss << queryString.substr(last);
//         return oss.str();
//     }();
// }


std::string AbstractLazy::raw_query() const {
    std::vector<DbVariant> keys;
    for(const auto &[key, value] : mProperties) {
        if(key.toString()=="*") {
            keys.push_back(key);
        } else {
            keys.push_back(key.setQuote());
        }
    }
    std::string queryString(mTabeName);
    return [&queryString, &keys] {
        std::ostringstream oss;
        size_t pos = 0, last = 0;
        if (queryString.find('$') != std::string::npos)
        {
            size_t idx = 0;
            while ((pos = queryString.find('$', last)) != std::string::npos)
            {
                oss << queryString.substr(last, pos - last);
                size_t digitStart = pos + 1;
                if (digitStart < queryString.length() && std::isdigit(static_cast<unsigned char>(queryString[digitStart])))
                {
                    size_t digitEnd = digitStart;
                    while (digitEnd < queryString.length() && std::isdigit(static_cast<unsigned char>(queryString[digitEnd])))
                    {
                        digitEnd++;
                    }
                    last = digitEnd;
                    if (idx < keys.size())
                    {
                        oss << keys[idx++].toString();
                    }
                    else
                    {
                        oss << queryString.substr(pos, digitEnd - pos);
                    }
                }
                else
                {
                    oss << '$'; last = pos + 1;
                }
            }
            oss << queryString.substr(last);
        }
        else
        {
            size_t idx = 0;
            while ((pos = queryString.find('?', last)) != std::string::npos && idx < keys.size())
            {
                oss << queryString.substr(last, pos - last) << keys[idx++].toString(); last = pos + 1;
            }
            oss << queryString.substr(last);
        }
        return oss.str();
    }();
}

WhereFilter AbstractLazy::whereFilter() const
{
    return mWhereFilter;
}

HavingFilter AbstractLazy::havingFilter() const
{
    return mHavingFilter;
}

ParameterizedQuery AbstractLazy::buildParameterized() const
{
    return {};
}

std::string AbstractLazy::queryString() const
{
  switch (mQueryType) {
  case INSERT:
      return insert_query();
  case SELECT:
  case SELECT_DISTINCT:
      return select_query();
  case UPDATE:
      return update_query();
  case DELETE:
    return delete_query();
  case INSERT_UPDATE:
    return insert_update_query();
  case BULK_INSERT:
    return bulk_insert_query();
  case INSERT_IGNORE:
    return insert_ignore_query();
  case BULK_UPDATE:
    return bulk_update_query();
  case CONTAINS:
      return contains_query();
  case COUNT:
      return count_query();
  case RAW_QUERY:
      return raw_query();
  default:
  break;
  }

  return {};
}

void AbstractLazy::setProperty(const std::string key, const DbVariant value)
{
    if(!appendPropAsCount(key)){
        mProperties[key]=value;
    }
}

void AbstractLazy::setProperties(const std::vector<std::string> &keys)
{
    for(const auto &key : keys)
    {
        if(!appendPropAsCount(key)){
            mProperties[key]="";
        }
    }
}

AbstractLazy & AbstractLazy::operator<<(const std::string &key)
{
    if(!appendPropAsCount(key)){
        mProperties[key]="";
    }
    return *this;
}

void AbstractLazy::setProperties(const std::initializer_list<std::pair<DbVariant, DbVariant>> items)
{
  for(const auto &item : items)
    {
        if(!appendPropAsCount(item.first)){
            mProperties[item.first] = item.second;
        }
    }
}

void AbstractLazy::setProperties(const std::initializer_list<std::map<DbVariant, DbVariant> > list)
{
  mBatchProperties = list;
}

void AbstractLazy::setProperties(const std::list<std::map<DbVariant, DbVariant> > &list)
{
  mBatchProperties = list;
}

AbstractLazy &AbstractLazy::operator<<(const std::pair<DbVariant, DbVariant> &key_value)
{
    if(!appendPropAsCount(key_value.first)){
        mProperties[key_value.first] = key_value.second;
    }

    return *this;
}

FilteringAbstractLazy& AbstractLazy::operator[](const LazyOrm::Filters &filter)
{
  return getCurrentFilters()[filter];
}

WhereFilter& AbstractLazy::operator[](const LazyOrm::NestedWhere &nestedWhere)
{
  return mWhereFilter;
}

void AbstractLazy::setNestedWhere(LazyOrm::WhereFilter nestedWhere){
    if(mWhereFilter != nestedWhere){
        mWhereFilter = nestedWhere;
    }
}

HavingFilter& AbstractLazy::operator[](const LazyOrm::Having &nestedHaving)
{
    return mHavingFilter;
}

void AbstractLazy::setNestedHaving(LazyOrm::HavingFilter nestedHaving){
    if(mHavingFilter != nestedHaving){
        mHavingFilter = nestedHaving;
    }
}

void AbstractLazy::setPrimaryKey(const std::string &primaryKey)
{
    this->mPrimaryKey = primaryKey;
}

std::string &AbstractLazy::operator[](const LazyOrm::Primary_Key &primaryKey)
{
    return mPrimaryKey;
}

std::string AbstractLazy::primaryKey() const
{
    return mPrimaryKey;
}

std::vector<std::string> AbstractLazy::uniqueKeys() const
{
    return mUniqueKeys;
}

void AbstractLazy::trim_consecutive_spaces(std::string &str) {
    auto new_end = std::unique(str.begin(), str.end(), [](char a, char b) {
        return a == ' ' && b == ' ';
    });
    str.erase(new_end, str.end());
}

std::string AbstractLazy::query_with_trim_consecutive_spaces() const
{
    std::string str = queryString();
    auto new_end = std::unique(str.begin(), str.end(), [](char a, char b) {
        return a == ' ' && b == ' ';
    });
    str.erase(new_end, str.end());
    return str;
}

std::list<std::string> AbstractLazy::count_queries() const
{
    const std::string distinct = "[DISTINCT]";

    std::list<std::string> list;
    for(const auto &countItem : mCounts){
        if(countItem.startsWith(distinct)){
            DbVariant newCountItem = countItem.toString().substr(distinct.size());
            list.push_back( "COUNT(DISTINCT "+newCountItem.setBackTick()+")" );
        } else {
            list.push_back( "COUNT("+countItem.setBackTick()+")" );
        }
    }
    return list;
}


//void AbstractLazy::operator=(const LazyOrm::FilterVariant &variant)
//{
////  mFilter.setFilter(variant);
//}

}
