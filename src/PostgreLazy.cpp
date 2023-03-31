#include "PostgreLazy.h"

namespace LazyOrm
{

PostgreLazy::PostgreLazy()
{

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

void PostgreLazy::insert_query()
{
  std::vector<std::string> keys, values;

  for(const auto &[key, value] : mProperties)
  {
    keys.push_back("`"+key+"`");
    values.push_back("\""+value.toString()+"\"");
  }

  mQueryString = "INSERT INTO ";
  mQueryString.append(mTabeName);
  mQueryString.append(" ("+string_join(",",keys)+") ");
  mQueryString.append("VALUES");
  mQueryString.append(" ("+string_join(",",values)+") ");
  mQueryString.append(";");
}

void PostgreLazy::select_query()
{
    std::vector<std::string> keys;
    for(const auto &[key, value] : mProperties)
    {
      if(key=="*")
      {
        keys.push_back(key);
      }
      else
      {
        keys.push_back("`"+key+"`");
      }
    }

    mQueryString = "SELECT ";
    mQueryString.append(string_join(",",keys));
    mQueryString.append(" FROM ");
    mQueryString.append(mTabeName);
    mQueryString.append(mFilter.where_conditions());
    mQueryString.append(";");
}

void PostgreLazy::update_query()
{
    std::vector<std::string> updates;
    for(const auto &[key, value] : mProperties)
    {
      updates.push_back("`"+key+"`='"+value.toString()+"'");
    }

    mQueryString = "UPDATE ";
    mQueryString.append(mTabeName);
    mQueryString.append(" SET ");
    mQueryString.append(string_join(",",updates));
    mQueryString.append(" ");
    mQueryString.append(mFilter.where_conditions());
    mQueryString.append(";");
}

void PostgreLazy::delete_query()
{
    mQueryString = "DELETE FROM ";
    mQueryString.append(mTabeName);
    mQueryString.append(" ");
    mQueryString.append(mFilter.where_conditions());
    mQueryString.append(" ;");
}

void PostgreLazy::batch_insert_query()
{
  if(mBatchProperties.size()<1)
  {
    return;
  }

  size_t columnsCount=0;
  std::vector<std::string> keys, values;

  auto &firstRow = mBatchProperties.front();
  columnsCount=firstRow.size();

  for(const auto &[key, value] : firstRow)
  {
    keys.push_back("`"+key+"`");
  }

  for(const auto &mapItem : mBatchProperties)
  {
    if(columnsCount!=mapItem.size())
    {
      return;
    }

    std::vector<std::string> rowValues;
    for(const auto &[key, value] : mapItem)
    {
      rowValues.push_back("\""+value.toString()+"\"");
    }
    values.push_back("("+string_join(",",rowValues)+")");
  }

  mQueryString = "INSERT INTO ";
  mQueryString.append(mTabeName);
  mQueryString.append(" ("+string_join(",",keys)+") ");
  mQueryString.append("VALUES");
  mQueryString.append(" "+string_join(",",values)+" ");
  mQueryString.append(";");
}

void PostgreLazy::insert_update_query()
{

}


}
