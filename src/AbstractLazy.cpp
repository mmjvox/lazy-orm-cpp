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

std::string AbstractLazy::tabeName() const
{
    return mTabeName;
}

std::map<DbVariant, DbVariant> AbstractLazy::properties() const
{
    return mProperties;
}

std::list<std::map<DbVariant, DbVariant> > AbstractLazy::batchProperties() const
{
    return mBatchProperties;
}

Query AbstractLazy::queryType() const
{
    return mQueryType;
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

std::string AbstractLazy::string_join(const std::string &delimiter, const std::vector<DbVariant> &container) const
{
  size_t size = container.size();
  size_t endPos = container.size()-1;
  std::string output;
  for(size_t i = 0; i < size; ++i) {
      output.append(container[i].toString());
      if(i!=endPos){
          output.append(delimiter);
      }
  }
  return output;
}

WhereFilter AbstractLazy::whereFilter() const
{
    return mWhereFilter;
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
  case INSERT_UPDATE:
    return insert_update_query();
  case BULK_INSERT:
    return bulk_insert_query();
  case INSERT_IGNORE:
    return insert_ignore_query();
  case BULK_UPDATE:
    return bulk_update_query();
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

void AbstractLazy::setProperties(const std::initializer_list<std::pair<DbVariant, DbVariant>> items)
{
  for(const auto &item : items)
    {
        mProperties.insert_or_assign(item.first, item.second.toString());
    }
}

void AbstractLazy::setProperties(const std::initializer_list<std::map<DbVariant, DbVariant> > list)
{
  mBatchProperties = list;
}

void AbstractLazy::setProperties(const std::list<std::map<DbVariant, DbVariant> > &list)
{
  mBatchProperties = list;
}

AbstractLazy &AbstractLazy::operator<<(const std::pair<DbVariant, DbVariant> &key_value){
    mProperties.insert_or_assign(key_value.first, key_value.second);
    return *this;
}

FilteringAbstractLazy& AbstractLazy::operator[](const LazyOrm::Filters &filter)
{
  return getCurrentFilters()[filter];
}

WhereFilter& AbstractLazy::operator[](const LazyOrm::NestedWhere &nestedWhere)
{
  return mWhereFilter;
}


void AbstractLazy::setPrimaryKey(const std::string &primaryKey)
{
    this->mPrimaryKey = primaryKey;
}

std::string &AbstractLazy::operator[](const LazyOrm::Primary_Key &primaryKey)
{
    return mPrimaryKey;
}

std::string AbstractLazy::primaryKey() const
{
    return mPrimaryKey;
}


//void AbstractLazy::operator=(const LazyOrm::FilterVariant &variant)
//{
////  mFilter.setFilter(variant);
//}

}
