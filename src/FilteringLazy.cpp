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

std::string FilteringLazy::orderbyString() const
{
    return {};
}

std::string FilteringLazy::limitString() const
{
    return {};
}

std::string FilteringLazy::groupString() const
{
    return {};
}


}
