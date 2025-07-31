#include "SqliteFilteringLazy.h"

namespace LazyOrm {
SqliteFilteringLazy::SqliteFilteringLazy()
{

}

void SqliteFilteringLazy::setLimitConditions(const std::initializer_list<FilterVariant> &filtersList)
{
    std::vector<DbVariant> filters;
    for(const auto& filter : filtersList)
    {
        DbVariant dt = std::visit([=](auto&& arg) -> DbVariant {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, DbVariant>) {
                    return arg;
                }
                else {return {};}
            }, filter);

        filters.push_back(dt);
    }
    if(filters.size()==2)
    {
        mLimitConditions=filters;
    }
    else if(filters.size()==1)
    {
        mLimitConditions=filters[0];
    }
}

void SqliteFilteringLazy::setOrderConditions(const std::initializer_list<FilterVariant> &filtersList)
{
    std::vector<DbVariant> filters;
    for(const auto& filter : filtersList)
    {
        DbVariant dt = std::visit([=](auto&& arg) -> DbVariant {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, DbVariant>) {
                    return arg;
                }
                else {return {};}
            }, filter);

        filters.push_back(dt);
    }
    mOrderConditions=filters;
}

void SqliteFilteringLazy::setGroupConditions(const std::initializer_list<FilterVariant> &filtersList)
{
    std::vector<DbVariant> filters;
    for(const auto& filter : filtersList)
    {
        DbVariant dt = std::visit([=](auto&& arg) -> DbVariant {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, DbVariant>) {
                    return arg;
                }
                else {return {};}
            }, filter);

        filters.push_back(dt);
    }
    mGroupConditions=filters;
}

std::string SqliteFilteringLazy::orderbyString() const
{
    if(mOrderConditions.empty())
    {
        return {};
    }

    std::string retStr(" ORDER BY ");

    std::visit([&retStr, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            retStr.append(string_join(",",arg,true));
        }
        else if constexpr (std::is_same_v<T, DbVariant>) {
            retStr.append(setBackTickForOrderType(arg));
        }
    }, mOrderConditions);

    retStr.append(" ");

    return retStr;
}

std::string SqliteFilteringLazy::limitString() const
{
    if(mLimitConditions.empty())
    {
        return {};
    }

    std::string retStr(" LIMIT ");

    std::visit([&retStr, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            retStr.append(string_join(" OFFSET ",arg));
        }
        else if constexpr (std::is_same_v<T, DbVariant>) {
            retStr.append(arg.toString());
        }
    }, mLimitConditions);

    retStr.append(" ");

    return retStr;
}

std::string SqliteFilteringLazy::groupString() const
{
    if(mGroupConditions.empty())
    {
        return {};
    }

    std::string retStr(" GROUP BY ");

    std::visit([&retStr, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            retStr.append(string_join(",",arg,true));
        }
        else if constexpr (std::is_same_v<T, DbVariant>) {
            retStr.append(arg.setBackTick());
        }
    }, mGroupConditions);

    retStr.append(" ");

    return retStr;
}

SqliteFilteringLazy::SqliteFilteringLazy(const FilteringAbstractLazy &abstractLaz)
{
    *this = abstractLaz;
}

void SqliteFilteringLazy::operator=(const FilteringAbstractLazy &abstractLaz)
{
    mReservedFilter = abstractLaz.reservedFilter();
    mLimitConditions = abstractLaz.limitConditions();
    mOrderConditions = abstractLaz.orderConditions();
    mGroupConditions = abstractLaz.groupConditions();
}

}
