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
    mQueryType = abstractLaz.queryType();
    mTabeName = abstractLaz.tabeName();
    mProperties = abstractLaz.properties();
    mBatchProperties = abstractLaz.batchProperties();
    mWhereFilter = abstractLaz.whereFilter();
    mFilter = abstractLaz.getFilter();
    mPrimaryKey = abstractLaz.primaryKey();
}

SqliteLazy::SqliteLazy(const std::string &table, const Query &queryType)
{
  setTabeName(table);
  setQueryType(queryType);
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
    queryString = "SELECT ";
    queryString.append(string_join(",",keys));
    queryString.append(" FROM ");
    queryString.append(mTabeName);
    queryString.append(mWhereFilter.toString());
    queryString.append(mFilter.filter_conditions());
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
    queryString.append(mFilter.filter_conditions());
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
    queryString.append(mFilter.filter_conditions());
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
    // TODO:
  return {};
}

std::string SqliteLazy::insert_ignore_query() const
{
    // TODO:
  return {};
}

std::string SqliteLazy::bulk_update_query() const
{
    // TODO:
  return {};
}


}
