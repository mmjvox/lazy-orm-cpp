#ifndef ABSTRACTLAZY_H
#define ABSTRACTLAZY_H

#include <list>
#include <vector>
#include <map>
#include <variant>
#include <string>

namespace LazyOrm {

typedef std::variant<int,double,float,bool,std::string> dbTypes;

enum Query
{
  UNDEFINED = -1,
  INIT      = 0,
  INSERT    = 10,
  SELECT,
  UPDATE,
  DELETE,
  INSERT_OR_UPDATE,
};

class AbstractLazy
{
private:
  Query mQueryType=UNDEFINED;

  struct dbTypeToString
  {
//    std::string operator()(const dbTypes &value){
//      using T = std::decay_t<decltype(value)>;
//      if constexpr (std::is_same_v<T, std::string>){
//        return "str";
//      }
//      return "value";
//    }
    std::string operator()(const std::string &value){return value;}
    //numbers
    std::string operator()(const int &value){return std::to_string(value);}
    std::string operator()(const double &value){return std::to_string(value);}
    std::string operator()(const float &value){return std::to_string(value);}
    // boolean
    std::string operator()(const bool &value){return value?"true":"false";}
  };

protected:
  std::string mQueryString;
  std::string mTabeName;
  std::map<std::string, dbTypes> mProperties;
  std::string toString(const dbTypes &value);
  std::string string_join(const std::string &delimiter, const std::vector<std::string> &container);

  virtual void insert_query() = 0;
  virtual void select_query() = 0;

public:
  std::string queryString();

  void setTabeName(const std::string &name);
  void setQueryType(LazyOrm::Query queryType);
  std::string & operator[](const LazyOrm::Query &queryType);

//  std::string operator[](const std::string &key) const;
  void setProperty(const std::string &key, const dbTypes value);
  dbTypes & operator[](const std::string &key);

  void setProperties(const std::vector<std::string> &keys);
  AbstractLazy & operator<<(const std::string &key);

};
}

#endif // ABSTRACTLAZY_H
