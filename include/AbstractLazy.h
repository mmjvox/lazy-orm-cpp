#ifndef ABSTRACTLAZY_H
#define ABSTRACTLAZY_H

#include "DbVariant.h"

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
  BATCH_INSERT
};

class AbstractLazy
{
private:
  Query mQueryType=UNDEFINED;

protected:
  std::string mQueryString;
  std::string mTabeName;
  std::map<std::string, DbVariant> mProperties;
  std::list<std::map<std::string, DbVariant>> mBatchProperties;
  std::string string_join(const std::string &delimiter, const std::vector<std::string> &container);

  virtual void insert_query() = 0;
  virtual void select_query() = 0;
  virtual void update_query() = 0;
  virtual void delete_query() = 0;
  virtual void insert_update_query() = 0;
  virtual void batch_insert_query() = 0;

public:
  std::string queryString();

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

};
}

#endif // ABSTRACTLAZY_H
