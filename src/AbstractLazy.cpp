#include "AbstractLazy.h"

namespace LazyOrm {

void AbstractLazy::setTabeName(const std::string &name)
{
  mTabeName = name;
}


//std::string AbstractLazy::operator[](const std::string &key) const
//{

//  return "";
//}


dbTypes &AbstractLazy::operator[](const std::string &key)
{
    return mProperties[key];
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

std::string AbstractLazy::toString(const dbTypes &value){
  return std::visit(dbTypeToString{}, value);
}

std::string AbstractLazy::string_join(const std::string &delimiter, const std::vector<std::string> &container)
{
  size_t size = container.size();
  size_t endPos = container.size()-1;
  std::string output;
  for(size_t i = 0; i < size; ++i) {
      output.append(container[i]);
      if(i!=endPos){
          output.append(delimiter);
      }
  }
  return output;
}

std::string AbstractLazy::queryString()
{
  switch (mQueryType) {
  case INSERT:
    insert_query();
  break;
  case SELECT:
    select_query();
  break;
  default:
  break;
  }

  return mQueryString;
}

void AbstractLazy::setProperty(const std::string &key, const dbTypes value)
{
  mProperties.insert_or_assign(key, value);
}

void AbstractLazy::setProperties(const std::vector<std::string> &keys)
{
    for(const auto &key : keys)
    {
        mProperties.insert_or_assign(key,"");
    }
}

AbstractLazy & AbstractLazy::operator<<(const std::string &key)
{
    mProperties.insert_or_assign(key,"");
    return *this;
}

void AbstractLazy::setProperties(const std::initializer_list<std::pair<std::string, dbTypes>> items)
{
    for(const auto &item : items)
    {
        mProperties.insert_or_assign(item.first,toString(item.second));
    }
}

AbstractLazy &AbstractLazy::operator<<(const std::pair<std::string, dbTypes> &key_value){
    mProperties.insert_or_assign(key_value.first, key_value.second);
    return *this;
}

}
