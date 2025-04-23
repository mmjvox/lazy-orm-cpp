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

void LazyOrm::FilterVariant::append(DbVariant &dbVariant)
{
    std::visit([this, &dbVariant](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<LazyOrm::DbVariant>>) {
            arg.push_back(dbVariant);
        }
        else if constexpr (std::is_same_v<T, LazyOrm::DbVariant>) {
            if(arg.empty()){
                *this = std::vector<LazyOrm::DbVariant>({dbVariant});
            } else {
                *this = std::vector<LazyOrm::DbVariant>({arg, dbVariant});
            }
        }
        else {
            *this = dbVariant;
        }
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
