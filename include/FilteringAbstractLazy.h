#ifndef FILTERINGABSTRACTLAZY_H
#define FILTERINGABSTRACTLAZY_H

#include <string>
#include <variant>
#include <list>
#include <vector>

namespace LazyOrm {

typedef std::variant<std::string,int,double,float,bool> dbTypes;

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
    FilterTypes mLimitConditions;
    FilterTypes mOrderConditions;
    FilterTypes mGroupConditions;
    std::vector<FilterTypes> mHavingConditions;
//    std::vector<FilterTypes> mFetchConditions;
    //

protected:
    template <typename FT>
    std::vector<FT> filterTypesToVector(FilterTypes &filter)
    {
        return std::visit([=](auto&& arg) -> std::vector<FT> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<FT>>) {
                return arg;
            }
            return {};
        }, filter);
    }
    //
    virtual void setWhereConditions(const Filters &filter, const std::initializer_list<LazyOrm::FilterTypes> &filtersList) = 0;
    virtual void setLimitConditions(const std::initializer_list<LazyOrm::FilterTypes> &filtersList) = 0;
    virtual void setOrderConditions(const std::initializer_list<LazyOrm::FilterTypes> &filtersList) = 0;
    virtual void setGroupConditions(const std::initializer_list<LazyOrm::FilterTypes> &filtersList) = 0;
    //
    virtual void appendWhere(std::string &retStr) = 0;
    virtual void appendOrderby(std::string &retStr) = 0;
    virtual void appendLimit(std::string &retStr) = 0;
    virtual void appendGroup(std::string &retStr) = 0;
    //
    std::string toStringVal(const FilterTypes &value);
    bool empty(const FilterTypes &value);

    virtual std::string where_conditions() = 0;

public:
    FilteringAbstractLazy();
    void setFilter(std::initializer_list<LazyOrm::FilterTypes> f);
//    template <typename T>
//    void setFilter(const Filters &filter, T&& arg);
    void setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterTypes> filtersList);
//    void setFilter(const Filters &filter, LazyOrm::WhereTypes &f);
    void setFilter(const Filters &filter, LazyOrm::FilterTypes f);
//    void test_init(std::initializer_list<std::vector<filterTypes>> f);
//    filterTypes & operator[](const Filters &filter);

    std::string testString();
};
}
#endif // FILTERINGABSTRACTLAZY_H
