#include "SqliteLazy.h"
#include "MariadbLazy.h"

namespace LazyOrm
{

SqliteLazy::SqliteLazy()
{
}

SqliteLazy::SqliteLazy(const AbstractLazy &abstractLaz)
{
    *this = abstractLaz;
}

void SqliteLazy::operator=(const AbstractLazy &abstractLaz)
{
    copyStateFrom(abstractLaz);
    mFilter = abstractLaz.getFilter();
}

void SqliteLazy::operator=(const SqliteLazy &other)
{
    *this = static_cast<const AbstractLazy &>(other);
}

SqliteLazy::SqliteLazy(const std::string &table, const Query &queryType)
{
  setTabeName(table);
  setQueryType(queryType);
}

ParameterizedQuery SqliteLazy::buildParameterized() const
{
    auto result = parameterizedQuery(ParameterDialect::SQLite);
    if (queryType() != RAW_QUERY) {
        trim_consecutive_spaces(result.sql);
    }
    return result;
}

void SqliteLazy::setFilter(const SqliteFilteringLazy filter)
{
  mFilter=filter;
}

std::string SqliteLazy::insert_query() const
{
  std::vector<std::string> keys, values;

  for(const auto &[key, value] : mProperties)
  {
    keys.push_back(key.setBackTick());
    values.push_back(value.setQuote());
  }

  std::string queryString;
  queryString = "INSERT INTO ";
  queryString.append(mTabeName);
  queryString.append(" ("+string_join(",",keys)+") ");
  queryString.append("VALUES");
  queryString.append(" ("+string_join(",",values)+") ");
  queryString.append(";");
  return queryString;
}

std::string SqliteLazy::select_query() const
{
    std::vector<DbVariant> keys;
    for(const auto &[key, value] : mProperties)
    {
      if(key.toString()=="*")
      {
        keys.push_back(key);
      }
      else
      {
        keys.push_back(key.setBackTick());
      }
    }

    if(!mCounts.empty()){
        for(const auto &countItem : count_queries()){
            keys.push_back(countItem);
        }
    }

    std::string queryString;
    queryString = mQueryType == SELECT_DISTINCT? "SELECT DISTINCT " : "SELECT ";
    queryString.append(string_join(",",keys));
    queryString.append(" FROM ");
    queryString.append(mTabeName);
    queryString.append(mWhereFilter.toString());
    queryString.append(mFilter.groupString());
    queryString.append(mHavingFilter.toString());
    queryString.append(mFilter.orderbyString());
    queryString.append(mFilter.limitString());
    queryString.append(";");
    return queryString;
}

std::string SqliteLazy::update_query() const
{
    std::vector<std::string> updates;
    for(const auto &[key, value] : mProperties)
    {
      updates.push_back(key.setBackTick()+"="+value.setQuote());
    }

    std::string queryString;
    queryString = "UPDATE ";
    queryString.append(mTabeName);
    queryString.append(" SET ");
    queryString.append(string_join(",",updates));
    queryString.append(" ");
    queryString.append(mWhereFilter.toString());
    queryString.append(mFilter.groupString());
    queryString.append(mHavingFilter.toString());
    queryString.append(mFilter.orderbyString());
    queryString.append(mFilter.limitString());
    queryString.append(";");
    return queryString;
}

std::string SqliteLazy::delete_query() const
{
    std::string queryString;
    queryString = "DELETE FROM ";
    queryString.append(mTabeName);
    queryString.append(" ");
    queryString.append(mWhereFilter.toString());
    queryString.append(mFilter.groupString());
    queryString.append(mHavingFilter.toString());
    queryString.append(mFilter.orderbyString());
    queryString.append(mFilter.limitString());
    queryString.append(" ;");
    return queryString;
}

std::string SqliteLazy::bulk_insert_query() const
{
  if(mBatchProperties.size()<1)
  {
      return {};
  }

  size_t columnsCount=0;
  std::vector<std::string> keys, values;

  auto &firstRow = mBatchProperties.front();
  columnsCount=firstRow.size();

  for(const auto &[key, value] : firstRow)
  {
    keys.push_back(key.setBackTick());
  }

  for(const auto &mapItem : mBatchProperties)
  {
    if(columnsCount!=mapItem.size())
    {
        return {};
    }

    std::vector<std::string> rowValues;
    for(const auto &[key, value] : mapItem)
    {
        rowValues.push_back(value.setQuote());
    }
    values.push_back("("+string_join(",",rowValues)+")");
  }

  std::string queryString;
  queryString = "INSERT INTO ";
  queryString.append(mTabeName);
  queryString.append(" ("+string_join(",",keys)+") ");
  queryString.append("VALUES");
  queryString.append(" "+string_join(",",values)+" ");
  queryString.append(";");
  return queryString;
}

void SqliteLazy::appendFilter(const Filters &filter, DbVariant dbVariant)
{
    mFilter.appendFilter(filter, dbVariant);
}

std::string SqliteLazy::insert_update_query() const
{
    std::vector<std::string> keys, values;

    for(const auto &[key, value] : mProperties)
    {
        if(key.isUpdate())
        {
            continue;
        }
        keys.push_back(key.setBackTick());
        values.push_back(value.setQuote());
    }

    std::vector<std::string> updates;
    for(const auto &[key, value] : mProperties)
    {
        if(key.isUpdate())
        {
            updates.push_back(key.setBackTick()+"="+value.setQuote());
        }
    }

    std::string queryString;
    queryString = "INSERT INTO ";
    queryString.append(mTabeName);
    queryString.append(" ("+string_join(",",keys)+") ");
    queryString.append("VALUES");
    queryString.append(" ("+string_join(",",values)+") ");
    if(!updates.empty() && !mUniqueKeys.empty())
    {
        queryString.append("ON CONFLICT ("+ string_join(",",mUniqueKeys, QuoteType::BackTick) +") DO UPDATE SET ");
        queryString.append(string_join(",",updates));
    }
    queryString.append(";");
    return queryString;
}

std::string SqliteLazy::insert_ignore_query() const
{
    std::vector<std::string> keys, values;

    for(const auto &[key, value] : mProperties)
    {
        keys.push_back(key.setDoubleQuote());
        values.push_back(value.setQuote());
    }

    std::string queryString;
    queryString = "INSERT OR IGNORE INTO ";
    queryString.append(mTabeName);
    queryString.append(" ("+string_join(",",keys)+") ");
    queryString.append("VALUES");
    queryString.append(" ("+string_join(",",values)+") ");
    queryString.append(";");
    return queryString;
}

std::string SqliteLazy::bulk_update_query() const
{
    std::vector<DbVariant> columns, updateColumns;
    if(!prepareBulkUpdate(columns, updateColumns))
    {
        return {};
    }

    const std::string targetTable = bulkUpdateTargetTable(BackTick);
    const std::string targetRef = bulkUpdateTargetRef(BackTick);
    const std::string quotedStaging = DbVariant(bulkUpdateStagingName()).setBackTick();
    const std::string quotedPrimaryKey = DbVariant(mPrimaryKey).setBackTick();

    // Populate the staging table, never the original target table.
    std::vector<std::string> keys;
    for(const auto &column : columns)
    {
        keys.push_back(column.setBackTick());
    }
    std::vector<std::string> rows;
    for(const auto &row : mBatchProperties)
    {
        std::vector<std::string> rowValues;
        for(const auto &[key, value] : row)
        {
            rowValues.push_back(bulkUpdateValue(value));
        }
        rows.push_back("("+string_join(",",rowValues)+")");
    }

    // SQLite has no UPDATE ... JOIN, and UPDATE ... FROM requires SQLite
    // 3.33+. Correlated subqueries work on every SQLite 3.x version. The
    // EXISTS guard ensures target rows absent from the staging table are
    // never overwritten with NULL.
    std::string updateQuery;
    updateQuery.append("UPDATE "+ targetTable +" SET ");
    std::vector<std::string> assignments;
    for(const auto &column : updateColumns)
    {
        const auto &quotedColumn = column.setBackTick();
        assignments.push_back(quotedColumn +" = (SELECT u."+ quotedColumn
                              +" FROM "+ quotedStaging +" u WHERE u."+ quotedPrimaryKey
                              +" = "+ targetRef +"."+ quotedPrimaryKey +")");
    }
    updateQuery.append(string_join(",", assignments));
    updateQuery.append(" WHERE EXISTS (SELECT 1 FROM "+ quotedStaging
                       +" u WHERE u."+ quotedPrimaryKey +" = "+ targetRef +"."+ quotedPrimaryKey +")");
    if(mWhereFilter.empty())
    {
        updateQuery.append(" ");
    }
    else
    {
        updateQuery.append(bulkUpdateWhereString("", " AND ", BackTick));
    }
    updateQuery.append(";");

    const std::string &dropTempTableQuery = "DROP TABLE IF EXISTS temp."+ quotedStaging +";";
    const std::string &createTempTableQuery = "CREATE TEMPORARY TABLE "+ quotedStaging
            +" ("+ string_join(",", keys) +");";
    const std::string &insertQuery = "INSERT INTO "+ quotedStaging
            +" ("+string_join(",",keys)+") VALUES "+ string_join(",", rows) +" ;";

    return dropTempTableQuery +" \n"+ createTempTableQuery +" \n"+ insertQuery +" \n"+ updateQuery +" \n"+ dropTempTableQuery;
}


}
