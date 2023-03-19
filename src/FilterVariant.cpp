#include "FilterVariant.h"

// WherePair
std::string LazyOrm::WherePair::FilterVariantToString::operator()(const std::vector<WherePair> &value){return {};}

std::string LazyOrm::WherePair::FilterVariantToString::operator()(const DbVariant &value){return value.toString();}

// FilterVariant
std::string LazyOrm::FilterVariant::FilterVariantToString::operator()(const std::vector<WherePair> &value){return {};}

std::string LazyOrm::FilterVariant::FilterVariantToString::operator()(const std::vector<DbVariant> &value){return {};}

std::string LazyOrm::FilterVariant::FilterVariantToString::operator()(const DbVariant &value){return value.toString();}

std::string LazyOrm::FilterVariant::toString(){
    return std::visit(FilterVariant::FilterVariantToString{}, *this);
}

bool LazyOrm::FilterVariant::empty()
{
    return std::visit([=](auto&& arg) -> bool {

        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::vector<WherePair>>) {
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
