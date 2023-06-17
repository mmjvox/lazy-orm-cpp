#include "WhereFilter.h"

namespace LazyOrm {

LazyOrm::WhereFilter::WhereFilter(){}
LazyOrm::WhereFilter::WhereFilter(Filters filter):filter{filter}{}

LazyOrm::WhereFilter::WhereFilter(std::initializer_list<DbVariant> whereFilters)
    : mData{{whereFilters}}
{
}

LazyOrm::WhereFilter::WhereFilter(Filters filter, std::vector<WhereFilter> whereFilters)
    : vector{whereFilters}, filter{filter}
{
}

LazyOrm::WhereFilter::WhereFilter(Filters filter, std::vector<DbVariant> whereFilters)
    : mData{{whereFilters}}, filter{filter}
{
}

LazyOrm::WhereFilter::WhereFilter(Filters filter, std::vector<std::vector<DbVariant>> whereFilters)
    : mData{whereFilters}, filter{filter}
{
}

//LazyOrm::WhereFilter::WhereFilter(Filters filter, std::vector<std::vector<WhereFilter> > whereFilters)
////    : vector{whereFilters}, filter{filter}
//{
//}

LazyOrm::WhereFilter::WhereFilter(std::vector<DbVariant> whereFilters)
    : mData{{whereFilters}}, filter{Filters::AND}
{
}

LazyOrm::WhereFilter::WhereFilter(std::vector<WhereFilter> whereFilters)
    : vector{whereFilters}, filter{Filters::AND}
{
}


}
