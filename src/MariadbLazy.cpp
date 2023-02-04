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


}
