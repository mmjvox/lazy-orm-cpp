#include "MariadbLazy.h"

namespace LazyOrm
{

MariadbLazy::MariadbLazy()
{

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

void MariadbLazy::insert_query() const
{
  std::vector<std::string> keys, values;

  for(const auto &[key, value] : mProperties)
  {
    keys.push_back("`"+key+"`");
    values.push_back("\""+value.toString()+"\"");
  }

  std::string queryString;
  queryString = "INSERT INTO ";
  queryString.append(mTabeName);
  queryString.append(" ("+string_join(",",keys)+") ");
  queryString.append("VALUES");
  queryString.append(" ("+string_join(",",values)+") ");
  queryString.append(";");
}

void MariadbLazy::select_query() const
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

    std::string queryString;
    queryString = "SELECT ";
    queryString.append(string_join(",",keys));
    queryString.append(" FROM ");
    queryString.append(mTabeName);
    queryString.append(mFilter.where_conditions());
    queryString.append(";");
}

void MariadbLazy::update_query() const
{
    std::vector<std::string> updates;
    for(const auto &[key, value] : mProperties)
    {
      updates.push_back("`"+key+"`='"+value.toString()+"'");
    }

    std::string queryString;
    queryString = "UPDATE ";
    queryString.append(mTabeName);
    queryString.append(" SET ");
    queryString.append(string_join(",",updates));
    queryString.append(" ");
    queryString.append(mFilter.where_conditions());
    queryString.append(";");
}

void MariadbLazy::delete_query() const
{
    std::string queryString;
    queryString = "DELETE FROM ";
    queryString.append(mTabeName);
    queryString.append(" ");
    queryString.append(mFilter.where_conditions());
    queryString.append(" ;");
}

void MariadbLazy::batch_insert_query() const
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

  std::string queryString;
  queryString = "INSERT INTO ";
  queryString.append(mTabeName);
  queryString.append(" ("+string_join(",",keys)+") ");
  queryString.append("VALUES");
  queryString.append(" "+string_join(",",values)+" ");
  queryString.append(";");
}

void MariadbLazy::insert_update_query() const
{

}

void MariadbLazy::insert_ignore_query() const
{
  std::vector<std::string> keys, values;

  for(const auto &[key, value] : mProperties)
  {
    keys.push_back("`"+key+"`");
    values.push_back("\""+value.toString()+"\"");
  }

  std::string queryString;
  queryString = "INSERT IGNORE INTO ";
  queryString.append(mTabeName);
  queryString.append(" ("+string_join(",",keys)+") ");
  queryString.append("VALUES");
  queryString.append(" ("+string_join(",",values)+") ");
  queryString.append(";");
}


}
