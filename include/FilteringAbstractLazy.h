#ifndef FILTERINGABSTRACTLAZY_H
#define FILTERINGABSTRACTLAZY_H

#include <string>
#include <variant>
#include <list>
#include <vector>

namespace LazyOrm {

typedef std::variant<int,double,float,bool,std::string> dbTypes;

enum Filters
{
  AND      = 0,
  OR    = 2,
  LIMIT
};

typedef std::pair<Filters,std::vector<LazyOrm::dbTypes>> filterPair;
typedef std::variant<dbTypes, std::vector<filterPair>> filterTypes;

class FilteringAbstractLazy
{
private:
    struct filterTypeToNested
    {
      // nested
      std::vector<filterPair> operator()(const std::vector<filterPair> &value){return value;}
      // string
      std::vector<filterPair> operator()(const dbTypes &value){return {};}
    };
    struct filterTypeToVal
    {
      // nested
      std::string operator()(const std::vector<filterPair> &value){return {};}
      // string
      std::string operator()(const dbTypes &value){return std::visit(filterTypeToString{}, value);}
    };
    struct filterTypeToString
    {
      // string
      std::string operator()(const std::string &value){return value;}
      //numbers
      std::string operator()(const int &value){return std::to_string(value);}
      std::string operator()(const double &value){return std::to_string(value);}
      std::string operator()(const float &value){return std::to_string(value);}
      // boolean
      std::string operator()(const bool &value){return value?"true":"false";}
    };

protected:
    std::string filterStr(Filters f);
    std::vector<std::pair<Filters,std::vector<filterTypes>>> mConditions;
    std::string toStringVal(const filterTypes &value);

    virtual std::string where_conditions() = 0;

public:
    FilteringAbstractLazy();
    void setFilter(std::initializer_list<filterTypes> f);
    void setFilter(const Filters &filter, std::initializer_list<filterTypes> f);
    void setFilter(const Filters &filter, LazyOrm::filterTypes &f);
//    void test_init(std::initializer_list<std::vector<filterTypes>> f);
//    filterTypes & operator[](const Filters &filter);

    std::string testString();
};
}
#endif // FILTERINGABSTRACTLAZY_H
