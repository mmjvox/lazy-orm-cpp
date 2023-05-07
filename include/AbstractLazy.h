#ifndef ABSTRACTLAZY_H
#define ABSTRACTLAZY_H

#include "DbVariant.h"
#include "FilteringAbstractLazy.h"

#include <list>
#include <vector>
#include <map>
#include <variant>
#include <string>
#include <initializer_list>
#include <iostream>


namespace LazyOrm {

enum Query
{
  UNDEFINED = -1,
  INIT      = 0,
  INSERT    = 10,
  SELECT,
  UPDATE,
  DELETE,
  INSERT_OR_UPDATE,
  BATCH_INSERT,
  INSERT_IGNORE
};

class AbstractLazy
{
private:
  Query mQueryType=UNDEFINED;

protected:
  std::string mTabeName;
  std::map<std::string, DbVariant> mProperties;
  std::list<std::map<std::string, DbVariant>> mBatchProperties;
  std::string string_join(const std::string &delimiter, const std::vector<std::string> &container) const;

  virtual std::string insert_query() const = 0;
  virtual std::string select_query() const = 0;
  virtual std::string update_query() const = 0;
  virtual std::string delete_query() const = 0;
  virtual std::string insert_update_query() const = 0;
  virtual std::string batch_insert_query() const = 0;
  virtual std::string insert_ignore_query() const = 0;

  virtual FilteringAbstractLazy& getCurrentFilters() = 0;

public:
  std::string queryString() const;

  void setTabeName(const std::string &name);
  void setQueryType(LazyOrm::Query queryType);
  std::string & operator[](const LazyOrm::Query &queryType);

//  std::string operator[](const std::string &key) const;
  void setProperty(const std::string &key, const DbVariant value);
  DbVariant & operator[](const std::string &key);

  void setProperties(const std::vector<std::string> &keys);
  AbstractLazy & operator<<(const std::string &key);
  void setProperties(const std::initializer_list<std::pair<std::string,DbVariant>> items);
  AbstractLazy & operator<<(const std::pair<std::string,DbVariant> &key_value);

  void setProperties(const std::initializer_list<std::map<std::string, DbVariant>> list);
  void setProperties(const std::list<std::map<std::string, LazyOrm::DbVariant>> &list);

  FilteringAbstractLazy& operator[](const LazyOrm::Filters &filter);
//  void operator=(const LazyOrm::FilterVariant &variant);
};
}

#endif // ABSTRACTLAZY_H
