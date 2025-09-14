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
    Filters mOrderByType=Filters::ORDERBY;
    std::string filterStr(Filters f) const;
    FilterVariant mLimitConditions;
    FilterVariant mOrderConditions;
    FilterVariant mGroupConditions;
//    std::vector<FilterTypes> mFetchConditions;
    //

protected:
    //
    enum QuoteFor
    {
        NoQuote=0,
        QuoteAll=1,
        GroupType=2,
        OrderType,
        LimitType
    };
    //
    virtual void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    virtual void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) = 0;
    //
    virtual std::string orderbyString() const = 0;
    virtual std::string limitString() const = 0;
    virtual std::string groupString() const = 0;
    //

    const std::string setBackTickForOrderType(DbVariant var) const;

    std::string filter_conditions() const;

    std::string string_join(const std::string &delimiter, const std::vector<DbVariant> &container, QuoteFor setQuote=NoQuote) const;

public:
    FilteringAbstractLazy();
    void setFilterForReserved(const LazyOrm::FilterVariant &variant);
    void setFilterForReserved(const std::vector<LazyOrm::FilterVariant> &variantList);
    void setFilter(std::initializer_list<LazyOrm::FilterVariant> filterVariantList);
    void setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterVariant> filterVariantList);
    void setFilter(const Filters &filter, LazyOrm::FilterVariant filterVariant);
    FilteringAbstractLazy& operator[](const LazyOrm::Filters &filter);
    void operator=(const LazyOrm::FilterVariant &variant);
    void appendFilter(const Filters &filter, LazyOrm::DbVariant &dbVariant);

    void setDESC(bool desc=true);
    void setASC(bool asc=true);

    // getters
    Filters reservedFilter() const;
    FilterVariant limitConditions() const;
    FilterVariant orderConditions() const;
    FilterVariant groupConditions() const;

    std::string filter_conditions_with_trim_consecutive_spaces();
};
}
#endif // FILTERINGABSTRACTLAZY_H
