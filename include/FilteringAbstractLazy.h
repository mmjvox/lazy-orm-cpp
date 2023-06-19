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
    Filters mReservedFilter=Filters::None;
    NestedWhere mReservedWhere=NestedWhere::NNone;

    struct FilterTypeToWhere
    {
      std::vector<WhereFilter> operator()(const std::vector<WhereFilter> &value){return value;}
      std::vector<WhereFilter> operator()(const std::vector<DbVariant> &value){return {};}
      std::vector<WhereFilter> operator()(const DbVariant &value){return {};}
    };

protected:
    std::string filterStr(Filters f) const;
    //FilterVariant mWhereConditions;
    FilterVariant mLimitConditions;
    FilterVariant mOrderConditions;
    FilterVariant mGroupConditions;
    std::vector<FilterVariant> mHavingConditions;
//    std::vector<FilterTypes> mFetchConditions;
    //

protected:
    //
    virtual void setWhereConditions(const Filters &filter, const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setHavingConditions(const std::vector<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    //
    virtual void appendWhere(std::string &retStr) const = 0;
    virtual void appendOrderby(std::string &retStr) const = 0;
    virtual void appendLimit(std::string &retStr) const = 0;
    virtual void appendGroup(std::string &retStr) const = 0;
    virtual void appendHaving(std::string &retStr) const = 0;
    //
//    virtual void nestedWhereToString(WhereFilter whereItem, std::string &retStr, Filters whereFilter, bool firstItem=false) const = 0;

    virtual std::string where_conditions() const = 0;

public:
    FilteringAbstractLazy();
    void setFilterForReserved(const LazyOrm::FilterVariant &variant);
    void setFilterForReserved(const std::vector<LazyOrm::FilterVariant> &variantList);
    void setFilter(std::initializer_list<LazyOrm::FilterVariant> filterVariantList);
    void setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterVariant> filterVariantList);
    void setFilter(const Filters &filter, LazyOrm::FilterVariant filterVariant);
//    void setFilter(const Filters &filter, LazyOrm::WhereFilter whereFilter);
    FilteringAbstractLazy& operator[](const LazyOrm::Filters &filter);
    FilteringAbstractLazy& operator[](const LazyOrm::NestedWhere &nestedWhere);
    void operator=(const LazyOrm::FilterVariant &variant);
//    void setWhereFilter(WhereFilter whereFilter);


    std::string string_join(const std::string &delimiter, const std::vector<DbVariant> &container) const;
};
}
#endif // FILTERINGABSTRACTLAZY_H
