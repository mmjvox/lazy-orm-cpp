#include "FilterVariant.h"

LazyOrm::FilterVariant::FilterVariant(){}

LazyOrm::FilterVariant::FilterVariant(std::vector<DbVariant> v)
    :variant(v)
{
}

bool LazyOrm::FilterVariant::empty() const
{
    return std::visit([=](auto&& arg) -> bool {

        using T = std::decay_t<decltype(arg)>;

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

std::vector<LazyOrm::DbVariant> LazyOrm::FilterVariant::toDbVariants() const
{
    return std::visit([=](auto&& arg) -> std::vector<LazyOrm::DbVariant> {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<LazyOrm::DbVariant>>) {
            return arg;
        }
        return {};
    }, *this);
}
