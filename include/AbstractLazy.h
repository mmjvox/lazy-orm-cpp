#ifndef ABSTRACTLAZY_H
#define ABSTRACTLAZY_H

#include "DbVariant.h"
#include "FilteringAbstractLazy.h"
#include "WhereFilter.h"

#include <list>
#include <vector>
#include <map>
#include <variant>
#include <string>
#include <initializer_list>


namespace LazyOrm {

enum Query
{
  UNDEFINED = -1,
  INIT      = 0,
  INSERT    = 10,
  SELECT,
  UPDATE,
  DELETE,
  INSERT_UPDATE,
  BULK_INSERT,
  INSERT_IGNORE,
  BULK_UPDATE,
  //TODO: BATCH_INSERT_UPDATE
  //TODO: has or contains
  SELECT_DISTINCT
};

enum Count{
    COUNT=0
};

enum Primary_Key{
    PrimaryKey=0
};

class AbstractLazy
{
protected:
  Query mQueryType=UNDEFINED;
  std::string mTabeName;
  std::map<DbVariant, DbVariant> mProperties;
  std::list<std::map<DbVariant, DbVariant>> mBatchProperties;
  std::string mPrimaryKey="id";
  std::list<DbVariant> mCounts;
  std::string string_join(const std::string &delimiter, const std::vector<std::string> &container) const;
  std::string string_join(const std::string &delimiter, const std::vector<DbVariant> &container) const;
  bool appendPropAsCount(DbVariant prop);

  virtual std::string insert_query() const = 0;
  virtual std::string select_query() const = 0;
  virtual std::string update_query() const = 0;
  virtual std::string delete_query() const = 0;
  virtual std::string insert_update_query() const = 0;
  virtual std::string bulk_insert_query() const = 0;
  virtual std::string insert_ignore_query() const = 0;
  virtual std::string bulk_update_query() const = 0;

  virtual std::list<std::string> count_queries() const;

  virtual FilteringAbstractLazy& getCurrentFilters() = 0;
  virtual void appendFilter(const Filters &filter, LazyOrm::DbVariant dbVariant) = 0;

  WhereFilter mWhereFilter;

public:
  const static char* version;
  virtual std::string queryString() const;

  void setTabeName(const std::string &name);
  void setQueryType(LazyOrm::Query queryType);
  std::string & operator[](const LazyOrm::Query &queryType);

  void setProperty(const std::string key, const DbVariant value);
  DbVariant & operator[](const std::string &key);

  void setProperties(const std::vector<std::string> &keys);
  AbstractLazy & operator<<(const std::string &key);
  void setProperties(const std::initializer_list<std::pair<DbVariant,DbVariant>> items);
  AbstractLazy & operator<<(const std::pair<DbVariant,DbVariant> &key_value);

  void setProperties(const std::initializer_list<std::map<DbVariant, DbVariant> > list);
  void setProperties(const std::list<std::map<DbVariant, LazyOrm::DbVariant>> &list);

  FilteringAbstractLazy& operator[](const LazyOrm::Filters &filter);
  WhereFilter& operator[](const LazyOrm::NestedWhere &nestedWhere);
  void setNestedWhere(LazyOrm::WhereFilter nestedWhere);
//  void operator=(const LazyOrm::FilterVariant &variant);

  void setPrimaryKey(const std::string &primaryKey);
  std::string & operator[](const LazyOrm::Primary_Key &primaryKey);

  void setCountType(std::initializer_list<DbVariant> countFields);
  std::list<DbVariant> & operator[](const LazyOrm::Count &count);

  // convert simple SELECT to SELECT DISTINCT
  void enableDistinctSelect(bool distinct=true);

  //getters
  std::string tabeName() const;
  std::map<DbVariant, DbVariant> properties() const;
  std::list<std::map<DbVariant, DbVariant> > batchProperties() const;
  WhereFilter whereFilter() const;
  Query queryType() const;
  virtual const FilteringAbstractLazy& getFilter() const = 0;
  std::string primaryKey() const;


  static void trim_consecutive_spaces(std::string& str);
  virtual std::string query_with_trim_consecutive_spaces();

};
}

#endif // ABSTRACTLAZY_H
