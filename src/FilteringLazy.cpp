#include "FilteringLazy.h"

namespace LazyOrm {

FilteringLazy::FilteringLazy() {

}

FilteringLazy::FilteringLazy(const FilteringAbstractLazy &abstractLaz)
{
    *this = abstractLaz;
}

void FilteringLazy::operator=(const FilteringAbstractLazy &abstractLaz)
{
    mReservedFilter = abstractLaz.reservedFilter();
    mLimitConditions = abstractLaz.limitConditions();
    mOrderConditions = abstractLaz.orderConditions();
    mGroupConditions = abstractLaz.groupConditions();
    mHavingConditions = abstractLaz.havingConditions();
}

void FilteringLazy::setHavingConditions(const std::vector<FilterVariant> &filtersList)
{

}

void FilteringLazy::setLimitConditions(const std::initializer_list<FilterVariant> &filtersList)
{

}

void FilteringLazy::setOrderConditions(const std::initializer_list<FilterVariant> &filtersList)
{

}

void FilteringLazy::setGroupConditions(const std::initializer_list<FilterVariant> &filtersList)
{

}

void FilteringLazy::appendOrderby(std::string &retStr) const
{

}

void FilteringLazy::appendLimit(std::string &retStr) const
{

}

void FilteringLazy::appendGroup(std::string &retStr) const
{

}

void FilteringLazy::appendHaving(std::string &retStr) const
{

}

std::string FilteringLazy::filter_conditions() const
{
    return {};
}

}
