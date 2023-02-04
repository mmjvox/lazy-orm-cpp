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

void MariadbLazy::insert_query()
{
  std::vector<std::string> keys, values;

  for(const auto &[key, value] : mProperties)
  {
    keys.push_back("`"+key+"`");
    values.push_back("\""+toString(value)+"\"");
  }

  mQueryString = "INSERT INTO ";
  mQueryString.append(mTabeName);
  mQueryString.append(" ("+string_join(",",keys)+") ");
  mQueryString.append("VALUES");
  mQueryString.append(" ("+string_join(",",values)+") ");
  mQueryString.append(";");
}

void MariadbLazy::select_query()
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
    mQueryString.append(";");
}


}
