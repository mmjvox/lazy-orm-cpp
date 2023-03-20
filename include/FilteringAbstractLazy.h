#ifndef FILTERINGABSTRACTLAZY_H
#define FILTERINGABSTRACTLAZY_H

#include "FilterVariant.h"
#include <string>
#include <variant>
#include <list>
#include <vector>

namespace LazyOrm {


class FilteringAbstractLazy
{
private:
    struct FilterTypeToWhere
    {
      std::vector<WhereFilter> operator()(const std::vector<WhereFilter> &value){return value;}
      std::vector<WhereFilter> operator()(const std::vector<DbVariant> &value){return {};}
      std::vector<WhereFilter> operator()(const DbVariant &value){return {};}
    };

protected:
    std::string filterStr(Filters f);
    WhereFilter mWhereConditions;
    FilterVariant mLimitConditions;
    FilterVariant mOrderConditions;
    FilterVariant mGroupConditions;
    std::vector<FilterVariant> mHavingConditions;
//    std::vector<FilterTypes> mFetchConditions;
    //

protected:
    //
    virtual void setWhereConditions(const Filters &filter, const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    //
    virtual void appendWhere(std::string &retStr) = 0;
    virtual void appendOrderby(std::string &retStr) = 0;
    virtual void appendLimit(std::string &retStr) = 0;
    virtual void appendGroup(std::string &retStr) = 0;
    //
    virtual void nestedWhereToString(WhereType<WhereFilter> whereItem, std::string &retStr, Filters whereFilter, bool firstItem=false) = 0;

    virtual std::string where_conditions() = 0;

public:
    FilteringAbstractLazy();
    void setFilter(std::initializer_list<LazyOrm::FilterVariant> f);
//    template <typename T>
//    void setFilter(const Filters &filter, T&& arg);
    void setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterVariant> filtersList);
//    void setFilter(const Filters &filter, LazyOrm::WhereTypes &f);
    void setFilter(const Filters &filter, LazyOrm::FilterVariant f);
//    void test_init(std::initializer_list<std::vector<filterTypes>> f);
//    filterTypes & operator[](const Filters &filter);



    void setWhereFilter(WhereFilter whereFilter){
        mWhereConditions=whereFilter;
    }

    std::string test_string()
    {
        return where_conditions();
    }
};
}
#endif // FILTERINGABSTRACTLAZY_H
