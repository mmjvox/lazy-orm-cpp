#include "PostgreLazy.h"
#include "LazyOrm.h"

namespace LazyOrm
{

PostgreLazy::PostgreLazy()
{
}

PostgreLazy::PostgreLazy(const AbstractLazy &abstractLaz)
{
    *this = abstractLaz;
}

void PostgreLazy::operator=(const AbstractLazy &abstractLaz)
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

PostgreLazy::PostgreLazy(const std::string &table, const Query &queryType)
{
  setTabeName(table);
  setQueryType(queryType);
}

void PostgreLazy::setFilter(const PostgreFilteringLazy filter)
{
  mFilter=filter;
}

std::string PostgreLazy::insert_query() const
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
  queryString.append(" returning "+mPrimaryKey);
  queryString.append(";");
  return queryString;
}

std::string PostgreLazy::select_query() const
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

std::string PostgreLazy::update_query() const
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

std::string PostgreLazy::delete_query() const
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

std::string PostgreLazy::bulk_insert_query() const
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

void PostgreLazy::appendFilter(const Filters &filter, DbVariant dbVariant)
{
    mFilter.appendFilter(filter, dbVariant);
}

std::string PostgreLazy::insert_update_query() const
{
    // TODO:
  return {};
}

std::string PostgreLazy::insert_ignore_query() const
{
    // TODO:
  return {};
}

std::string PostgreLazy::bulk_update_query() const
{
    // TODO:
  return {};
}


}
