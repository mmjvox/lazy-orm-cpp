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


DbVariant &AbstractLazy::operator[](const std::string &key)
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

std::string AbstractLazy::string_join(const std::string &delimiter, const std::vector<std::string> &container) const
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

std::string AbstractLazy::queryString() const
{
  switch (mQueryType) {
  case INSERT:
    return insert_query();
  case SELECT:
    return select_query();
  case UPDATE:
    return update_query();
  case DELETE:
    return delete_query();
  case INSERT_OR_UPDATE:
    return insert_update_query();
  case BATCH_INSERT:
    return batch_insert_query();
  case INSERT_IGNORE:
    return insert_ignore_query();
  default:
  break;
  }

  return {};
}

void AbstractLazy::setProperty(const std::string &key, const DbVariant value)
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

void AbstractLazy::setProperties(const std::initializer_list<std::pair<std::string, DbVariant>> items)
{
  for(const auto &item : items)
    {
        mProperties.insert_or_assign(item.first, item.second.toString());
    }
}

void AbstractLazy::setProperties(const std::initializer_list<std::map<std::string, DbVariant> > list)
{
  mBatchProperties = list;
}

void AbstractLazy::setProperties(const std::list<std::map<std::string, DbVariant> > &list)
{
  mBatchProperties = list;
}

AbstractLazy &AbstractLazy::operator<<(const std::pair<std::string, DbVariant> &key_value){
    mProperties.insert_or_assign(key_value.first, key_value.second);
    return *this;
}

FilteringAbstractLazy& AbstractLazy::operator[](const LazyOrm::Filters &filter)
{
  return getCurrentFilters()[filter];
}

//void AbstractLazy::operator=(const LazyOrm::FilterVariant &variant)
//{
////  mFilter.setFilter(variant);
//}

}
