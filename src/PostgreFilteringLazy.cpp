#include "PostgreFilteringLazy.h"

namespace LazyOrm {
PostgreFilteringLazy::PostgreFilteringLazy()
{

}

void PostgreFilteringLazy::setLimitConditions(const std::initializer_list<FilterVariant> &filtersList)
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

void PostgreFilteringLazy::setOrderConditions(const std::initializer_list<FilterVariant> &filtersList)
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

void PostgreFilteringLazy::setGroupConditions(const std::initializer_list<FilterVariant> &filtersList)
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

std::string PostgreFilteringLazy::orderbyString() const
{
    if(mOrderConditions.empty())
    {
        return {};
    }

    std::string retStr(" ORDER BY ");

    std::visit([&retStr, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            retStr.append(string_join(",",arg, QuoteFor::OrderType));
        }
        else if constexpr (std::is_same_v<T, DbVariant>) {
            retStr.append(setDoubleQuoteForOrderType(arg));
        }
    }, mOrderConditions);

    retStr.append(" ");

    return retStr;
}

std::string PostgreFilteringLazy::limitString() const
{
    if(mLimitConditions.empty())
    {
        return {};
    }

    std::vector<DbVariant> limits;
    std::visit([&limits](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            if(arg.size()==3 && arg[1].toString()==","){
                limits = {arg[0], arg[2]};
            } else {
                limits = arg;
            }
        }
        else if constexpr (std::is_same_v<T, DbVariant>) {
            const std::string value = arg.toString();
            const auto comma = value.find(',');
            if(comma != std::string::npos){
                limits = {DbVariant(value.substr(0, comma)).trim(), DbVariant(value.substr(comma + 1)).trim()};
            } else {
                limits = {arg};
            }
        }
    }, mLimitConditions);

    if(limits.empty()){
        return {};
    }

    std::string retStr(" LIMIT ");
    retStr.append(limits[0].trim());
    if(limits.size()>1){
        retStr.append(" OFFSET "+limits[1].trim());
    }
    retStr.append(" ");

    return retStr;
}

std::string PostgreFilteringLazy::groupString() const
{
    if(mGroupConditions.empty())
    {
        return {};
    }

    std::string retStr(" GROUP BY ");

    std::visit([&retStr, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            retStr.append(string_join(",",arg, QuoteFor::GroupType));
        }
        else if constexpr (std::is_same_v<T, DbVariant>) {
            retStr.append(arg.setDoubleQuote());
        }
    }, mGroupConditions);

    retStr.append(" ");

    return retStr;
}

PostgreFilteringLazy::PostgreFilteringLazy(const FilteringAbstractLazy &abstractLaz)
{
    *this = abstractLaz;
}

void PostgreFilteringLazy::operator=(const FilteringAbstractLazy &abstractLaz)
{
    mReservedFilter = abstractLaz.reservedFilter();
    mLimitConditions = abstractLaz.limitConditions();
    mOrderConditions = abstractLaz.orderConditions();
    mGroupConditions = abstractLaz.groupConditions();
}

std::string PostgreFilteringLazy::string_join(const std::string &delimiter, const std::vector<DbVariant> &container, QuoteFor setQuote) const
{
  size_t size = container.size();
  size_t endPos = container.size()-1;
  std::string output;
  for(size_t i = 0; i < size; ++i) {
        if(setQuote==GroupType){
            output.append(container[i].setDoubleQuote());
        }
        else if(setQuote==OrderType){
            output.append(setDoubleQuoteForOrderType(container[i]));
        }
        else if(setQuote==LimitType){
            output.append(container[i].toString());
        }
        else {
            output.append(container[i].toString());
        }
        if(i!=endPos){
          output.append(delimiter);
        }
  }
  return output;
}

}
