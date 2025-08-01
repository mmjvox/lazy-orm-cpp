#include "AbstractLazy.h"
#include <algorithm>
#include <sstream>

namespace LazyOrm {

const char *AbstractLazy::version = lazy_orm_version;

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

void AbstractLazy::enableDistinctSelect(bool distinct){
    if(mQueryType == SELECT and distinct){
        mQueryType = SELECT_DISTINCT;
    }
    else if(mQueryType == SELECT_DISTINCT and !distinct){
        mQueryType = SELECT;
    }
}

void AbstractLazy::setCountType(std::initializer_list<LazyOrm::DbVariant> countFields)
{
    mCounts = countFields;
}

std::list<LazyOrm::DbVariant> &AbstractLazy::operator[](const Count &count){
    return mCounts;
}

void AbstractLazy::appendFilter(const Filters &filter, DbVariant dbVariant)
{
    // TODO: implement later if needed
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

bool AbstractLazy::appendPropAsCount(DbVariant prop)
{
    std::string countKey = prop.toString();
    countKey.erase(std::remove_if(countKey.begin(), countKey.end(), ::isspace), countKey.end());
    if(DbVariant(countKey).startsWith("count(", false)){
        std::string propStr = prop.toString();
        size_t start = propStr.find('(');
        size_t end = propStr.find(')');
        if (start != std::string::npos && end != std::string::npos && start < end) {
            mCounts.push_back( propStr.substr(start + 1, end - start - 1) );
            return true;
        }
    }

    return false;
}

std::string AbstractLazy::contains_query() const
{
    std::string queryString = "SELECT CASE WHEN COUNT(*) > 0 THEN TRUE ELSE FALSE END AS \"contains\" FROM ";
    queryString.append(mTabeName);
    queryString.append(mWhereFilter.toString());
    queryString.append(";");
    return queryString;
}

std::string AbstractLazy::count_query() const
{
    std::string queryString = "SELECT COUNT(*) AS \"count\" FROM ";
    queryString.append(mTabeName);
    queryString.append(mWhereFilter.toString());
    queryString.append(";");
    return queryString;
}

std::string AbstractLazy::raw_query() const
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
            keys.push_back(key.setQuote());
        }
    }

    std::string queryString(mTabeName);

    return [&queryString, &keys] {
        std::ostringstream oss;
        size_t pos = 0, last = 0, idx = 0;
        while ((pos = queryString.find('?', last)) != std::string::npos && idx < keys.size()) {
            oss << queryString.substr(last, pos - last) << keys[idx++].toString();
            last = pos + 1;
        }
        oss << queryString.substr(last);
        return oss.str();
    }();
}

WhereFilter AbstractLazy::whereFilter() const
{
    return mWhereFilter;
}

HavingFilter AbstractLazy::havingFilter() const
{
    return mHavingFilter;
}

std::string AbstractLazy::queryString() const
{
  switch (mQueryType) {
  case INSERT:
      return insert_query();
  case SELECT:
  case SELECT_DISTINCT:
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
  case CONTAINS:
      return contains_query();
  case COUNT:
      return count_query();
  case RAW_QUERY:
      return raw_query();
  default:
  break;
  }

  return {};
}

void AbstractLazy::setProperty(const std::string key, const DbVariant value)
{
    if(!appendPropAsCount(key)){
        mProperties[key]=value;
    }
}

void AbstractLazy::setProperties(const std::vector<std::string> &keys)
{
    for(const auto &key : keys)
    {
        if(!appendPropAsCount(key)){
            mProperties[key]="";
        }
    }
}

AbstractLazy & AbstractLazy::operator<<(const std::string &key)
{
    if(!appendPropAsCount(key)){
        mProperties[key]="";
    }
    return *this;
}

void AbstractLazy::setProperties(const std::initializer_list<std::pair<DbVariant, DbVariant>> items)
{
  for(const auto &item : items)
    {
        if(!appendPropAsCount(item.first)){
            mProperties[item.first] = item.second.toString();
        }
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

AbstractLazy &AbstractLazy::operator<<(const std::pair<DbVariant, DbVariant> &key_value)
{
    if(!appendPropAsCount(key_value.first)){
        mProperties[key_value.first] = key_value.second;
    }

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

void AbstractLazy::setNestedWhere(LazyOrm::WhereFilter nestedWhere){
    if(mWhereFilter != nestedWhere){
        mWhereFilter = nestedWhere;
    }
}

HavingFilter& AbstractLazy::operator[](const LazyOrm::Having &nestedHaving)
{
    return mHavingFilter;
}

void AbstractLazy::setNestedHaving(LazyOrm::HavingFilter nestedHaving){
    if(mHavingFilter != nestedHaving){
        mHavingFilter = nestedHaving;
    }
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

void AbstractLazy::trim_consecutive_spaces(std::string &str) {
    auto new_end = std::unique(str.begin(), str.end(), [](char a, char b) {
        return a == ' ' && b == ' ';
    });
    str.erase(new_end, str.end());
}

std::string AbstractLazy::query_with_trim_consecutive_spaces()
{
    std::string str = queryString();
    auto new_end = std::unique(str.begin(), str.end(), [](char a, char b) {
        return a == ' ' && b == ' ';
    });
    str.erase(new_end, str.end());
    return str;
}

std::list<std::string> AbstractLazy::count_queries() const
{
    const std::string distinct = "[DISTINCT]";

    std::list<std::string> list;
    for(const auto &countItem : mCounts){
        if(countItem.startsWith(distinct)){
            DbVariant newCountItem = countItem.toString().substr(distinct.size());
            list.push_back( "COUNT(DISTINCT "+newCountItem.setBackTick()+")" );
        } else {
            list.push_back( "COUNT("+countItem.setBackTick()+")" );
        }
    }
    return list;
}


//void AbstractLazy::operator=(const LazyOrm::FilterVariant &variant)
//{
////  mFilter.setFilter(variant);
//}

}
