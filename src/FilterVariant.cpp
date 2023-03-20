#include "FilterVariant.h"

// WherePair
std::string LazyOrm::WhereFilter::FilterVariantToString::operator()(const WhereFilter &value){return {};}

std::string LazyOrm::WhereFilter::FilterVariantToString::operator()(const std::vector<DbVariant> &value){return "dddddd";}

// FilterVariant
std::string LazyOrm::FilterVariant::FilterVariantToString::operator()(const WhereFilter &value){return {};}

std::string LazyOrm::FilterVariant::FilterVariantToString::operator()(const std::vector<DbVariant> &value){return {};}

std::string LazyOrm::FilterVariant::FilterVariantToString::operator()(const DbVariant &value){return value.toString();}

std::string LazyOrm::FilterVariant::toString(){
    return std::visit(FilterVariant::FilterVariantToString{}, *this);
}

bool LazyOrm::FilterVariant::empty()
{
    return std::visit([=](auto&& arg) -> bool {

        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, WhereFilter>) {
            if(!arg.empty())
            {
                return false;
            }
        }

        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            if(!arg.empty())
            {
                return false;
            }
        }

        if constexpr (std::is_same_v<T, DbVariant>) {
            if(!arg.toString().empty())
            {
                return false;
            }
        }

        return true;

    }, *this);
}

LazyOrm::WhereFilter::WhereFilter(){}

LazyOrm::WhereFilter::WhereFilter(Filters filter, std::vector<WhereType<WhereFilter> > whereFilters)
    : std::vector<WhereType<WhereFilter>>(whereFilters), filter{filter}
{
}

LazyOrm::WhereFilter::WhereFilter(Filters filter, std::vector<DbVariant> whereFilters)
    : filter{filter}
{
    this->push_back(whereFilters);
}

LazyOrm::WhereFilter::WhereFilter(std::vector<DbVariant> whereFilters)
{
    filter = AND;
    this->push_back(whereFilters);
}
