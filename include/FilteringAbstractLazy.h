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
  ORDERBY,
  LIMIT,
  HAVING,
  GROUPBY
};

typedef std::pair<Filters,std::vector<LazyOrm::dbTypes>> WherePair;
//typedef std::variant<dbTypes, std::vector<WherePair>> WhereTypes;
typedef std::variant<dbTypes, std::vector<dbTypes>, std::vector<WherePair>> FilterTypes;

class FilteringAbstractLazy
{
private:
    struct FilterTypeToWhere
    {
      std::vector<WherePair> operator()(const std::vector<WherePair> &value){return value;}
      std::vector<WherePair> operator()(const std::vector<dbTypes> &value){return {};}
      std::vector<WherePair> operator()(const dbTypes &value){return {};}
    };
    struct FilterTypeToString
    {
      std::string operator()(const std::vector<WherePair> &value){return {};}
      std::string operator()(const std::vector<dbTypes> &value){return {};}
      std::string operator()(const dbTypes &value){return std::visit(dbType2ToString{}, value);}
    };
    struct dbType2ToString
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
    std::vector<FilterTypes> mWhereConditions;
    std::vector<FilterTypes> mOrderConditions;
    std::vector<FilterTypes> mLimitConditions;
//    std::vector<FilterTypes> mFetchConditions;
    std::vector<FilterTypes> mGroupConditions;
    std::vector<FilterTypes> mHavingConditions;
    //
    std::string toStringVal(const FilterTypes &value);

    virtual std::string where_conditions() = 0;

public:
    FilteringAbstractLazy();
    void setFilter(std::initializer_list<LazyOrm::FilterTypes> f);
//    template <typename T>
//    void setFilter(const Filters &filter, T&& arg);
    void setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterTypes> filters);
//    void setFilter(const Filters &filter, LazyOrm::WhereTypes &f);
    void setFilter(const Filters &filter, LazyOrm::FilterTypes f);
//    void test_init(std::initializer_list<std::vector<filterTypes>> f);
//    filterTypes & operator[](const Filters &filter);

    std::string testString();
};
}
#endif // FILTERINGABSTRACTLAZY_H
