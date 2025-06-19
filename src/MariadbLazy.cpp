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
    mQueryType = abstractLaz.queryType();
    mTabeName = abstractLaz.tabeName();
    mProperties = abstractLaz.properties();
    mBatchProperties = abstractLaz.batchProperties();
    mWhereFilter = abstractLaz.whereFilter();
    mHavingFilter = abstractLaz.havingFilter();
    mFilter = abstractLaz.getFilter();
    mPrimaryKey = abstractLaz.primaryKey();
}

MariadbLazy::MariadbLazy(const std::string &table, const Query &queryType)
{
  setTabeName(table);
  setQueryType(queryType);
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
    if(mBatchProperties.size()<1)
    {
        return {};
    }

    const std::string &createTempTableQuery = "CREATE TEMPORARY TABLE lazyormtemp"+ mTabeName +" LIKE "+ mTabeName +";";

    const std::string &bulkInsertQuery = bulk_insert_query();


    std::string updateTableQuery ="UPDATE "+ mTabeName +" t \nJOIN updates u ON t."+mPrimaryKey+" = u."+mPrimaryKey;
    for(const auto &updateRow : mBatchProperties){
        for (const auto& [key, value] : updateRow) {
            const auto & col = key.toCleanString();
            updateTableQuery.append( "\nSET t."+ col +" = u."+ col +"; " );
        }
    }

    const std::string &dropTempTableQuery = "DROP TEMPORARY TABLE IF EXISTS lazyormtemp"+ mTabeName +";";

    return dropTempTableQuery +" \n"+ createTempTableQuery +" \n"+ bulkInsertQuery +" \n"+ updateTableQuery +" \n"+ dropTempTableQuery;
}


}
