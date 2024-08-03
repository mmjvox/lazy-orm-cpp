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

void SqliteFilteringLazy::setHavingConditions(const std::vector<FilterVariant> &filtersList)
{
    mHavingConditions=filtersList;
}

void SqliteFilteringLazy::appendOrderby(std::string &retStr) const
{
    if(mOrderConditions.empty())
    {
        return;
    }

    retStr.append(" ORDER BY ");

    std::visit([&retStr, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            retStr.append(string_join(",",arg));
        }
        else if constexpr (std::is_same_v<T, DbVariant>) {
            retStr.append(arg.toString());
        }
    }, mOrderConditions);

    retStr.append(" ");
}

void SqliteFilteringLazy::appendLimit(std::string &retStr) const
{
    if(mLimitConditions.empty())
    {
        return;
    }

    retStr.append(" LIMIT ");

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
}

void SqliteFilteringLazy::appendGroup(std::string &retStr) const
{
    if(mGroupConditions.empty())
    {
        return;
    }

    retStr.append(" GROUP BY ");

    std::visit([&retStr, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            retStr.append(string_join(",",arg));
        }
        else if constexpr (std::is_same_v<T, DbVariant>) {
            retStr.append(arg.toString());
        }
    }, mGroupConditions);

    retStr.append(" ");
}

void SqliteFilteringLazy::appendHaving(std::string &retStr) const
{
  if(mHavingConditions.empty())
  {
      return;
  }

  retStr.append(" HAVING ");

  for(const auto &havingItem : mHavingConditions)
  {
    std::visit([=,&retStr](auto&& arg){
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            if(arg.size()==1)
            {
                retStr.append(arg.at(0).toCleanString());
            }
            else if(arg.size()==2)
            {
                retStr.append(arg.at(0).setBackTick());
                retStr.append(" = ");
                retStr.append(arg.at(1).toString());
                retStr.append(" ");
            }
            else if(arg.size()==3)
            {
                retStr.append(arg.at(0).setBackTick());
                retStr.append(" ");
                retStr.append(arg.at(1).toString());
                retStr.append(" ");
                retStr.append(arg.at(2).setQuote());
                retStr.append(" ");
            }
        }
    }, havingItem);
  }
}


std::string SqliteFilteringLazy::filter_conditions() const
{
    std::string retStr;

    // GROUP BY
    appendGroup(retStr);

    // HAVING
    appendHaving(retStr);

    // ORDER BY
    appendOrderby(retStr);

    // LIMIT
    appendLimit(retStr);

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
    mHavingConditions = abstractLaz.havingConditions();
}

}
