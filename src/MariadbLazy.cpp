#include "MariadbLazy.h"

namespace LazyOrm
{

MariadbLazy::MariadbLazy()
{
}

MariadbLazy::MariadbLazy(const AbstractLazy &abstractLaz)
{
    *this = abstractLaz;
}

void MariadbLazy::operator=(const AbstractLazy &abstractLaz)
{
    copyStateFrom(abstractLaz);
    mFilter = abstractLaz.getFilter();
}

void MariadbLazy::operator=(const MariadbLazy &other)
{
    *this = static_cast<const AbstractLazy &>(other);
}

MariadbLazy::MariadbLazy(const std::string &table, const Query &queryType)
{
  setTabeName(table);
  setQueryType(queryType);
}

ParameterizedQuery MariadbLazy::buildParameterized() const
{
    auto result = parameterizedQuery(ParameterDialect::MariaDB);
    if (queryType() != RAW_QUERY) {
        trim_consecutive_spaces(result.sql);
    }
    return result;
}

void MariadbLazy::setFilter(const MariadbFilteringLazy filter)
{
  mFilter=filter;
}

std::string MariadbLazy::insert_query() const
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

std::string MariadbLazy::select_query() const
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

std::string MariadbLazy::update_query() const
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

std::string MariadbLazy::delete_query() const
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

std::string MariadbLazy::bulk_insert_query() const
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

std::string MariadbLazy::insert_update_query() const
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
  if(!updates.empty())
  {
    queryString.append("ON DUPLICATE KEY UPDATE ");
    queryString.append(string_join(",",updates));
  }
  queryString.append(";");
  return queryString;
}

std::string MariadbLazy::insert_ignore_query() const
{
  std::vector<std::string> keys, values;

  for(const auto &[key, value] : mProperties)
  {
    keys.push_back(key.setBackTick());
    values.push_back(value.setQuote());
  }

  std::string queryString;
  queryString = "INSERT IGNORE INTO ";
  queryString.append(mTabeName);
  queryString.append(" ("+string_join(",",keys)+") ");
  queryString.append("VALUES");
  queryString.append(" ("+string_join(",",values)+") ");
  queryString.append(";");
  return queryString;
}

void MariadbLazy::appendFilter(const Filters &filter, DbVariant dbVariant)
{
    mFilter.appendFilter(filter, dbVariant);
}

std::string MariadbLazy::bulk_update_query() const
{
    std::vector<DbVariant> columns, updateColumns;
    if(!prepareBulkUpdate(columns, updateColumns))
    {
        return {};
    }

    const std::string targetTable = bulkUpdateTargetTable(BackTick);
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

    std::string updateQuery;
    updateQuery.append("UPDATE "+ targetTable +" t JOIN "+ quotedStaging);
    updateQuery.append(" u ON t."+ quotedPrimaryKey +" = u."+ quotedPrimaryKey);
    updateQuery.append(" SET ");
    std::vector<std::string> assignments;
    for(const auto &column : updateColumns)
    {
        const auto &quotedColumn = column.setBackTick();
        assignments.push_back("t."+ quotedColumn +" = u."+ quotedColumn);
    }
    updateQuery.append(string_join(",", assignments));
    updateQuery.append(bulkUpdateWhereString("t", " WHERE ", BackTick));
    updateQuery.append(";");

    const std::string &dropTempTableQuery = "DROP TEMPORARY TABLE IF EXISTS "+ quotedStaging +";";
    const std::string &createTempTableQuery = "CREATE TEMPORARY TABLE "+ quotedStaging +" LIKE "+ targetTable +";";
    const std::string &insertQuery = "INSERT INTO "+ quotedStaging
            +" ("+string_join(",",keys)+") VALUES "+ string_join(",", rows) +" ;";

    return dropTempTableQuery +" \n"+ createTempTableQuery +" \n"+ insertQuery +" \n"+ updateQuery +" \n"+ dropTempTableQuery;
}


}
