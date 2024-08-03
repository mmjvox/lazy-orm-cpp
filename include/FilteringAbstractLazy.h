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
protected:
    Filters mReservedFilter=Filters::None;
    std::string filterStr(Filters f) const;
    FilterVariant mLimitConditions;
    FilterVariant mOrderConditions;
    FilterVariant mGroupConditions;
    std::vector<FilterVariant> mHavingConditions;
//    std::vector<FilterTypes> mFetchConditions;
    //

protected:
    //
    virtual void setHavingConditions(const std::vector<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    //
    virtual void appendOrderby(std::string &retStr) const = 0;
    virtual void appendLimit(std::string &retStr) const = 0;
    virtual void appendGroup(std::string &retStr) const = 0;
    virtual void appendHaving(std::string &retStr) const = 0;
    //

    virtual std::string filter_conditions() const = 0;

public:
    FilteringAbstractLazy();
    void setFilterForReserved(const LazyOrm::FilterVariant &variant);
    void setFilterForReserved(const std::vector<LazyOrm::FilterVariant> &variantList);
    void setFilter(std::initializer_list<LazyOrm::FilterVariant> filterVariantList);
    void setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterVariant> filterVariantList);
    void setFilter(const Filters &filter, LazyOrm::FilterVariant filterVariant);
    FilteringAbstractLazy& operator[](const LazyOrm::Filters &filter);
    void operator=(const LazyOrm::FilterVariant &variant);


    std::string string_join(const std::string &delimiter, const std::vector<DbVariant> &container) const;
    // getters
    Filters reservedFilter() const;
    FilterVariant limitConditions() const;
    FilterVariant orderConditions() const;
    FilterVariant groupConditions() const;
    std::vector<FilterVariant> havingConditions() const;
};
}
#endif // FILTERINGABSTRACTLAZY_H
