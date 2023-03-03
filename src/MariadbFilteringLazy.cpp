#include "MariadbFilteringLazy.h"

namespace LazyOrm {
MariadbFilteringLazy::MariadbFilteringLazy()
{

}

void MariadbFilteringLazy::setLimitConditions(const std::initializer_list<FilterTypes> &filtersList)
{
    std::vector<dbTypes> filters;
    for(const auto& filter : filtersList)
    {
        dbTypes dt = std::visit([=](auto&& arg) -> dbTypes {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, dbTypes>) {
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

void MariadbFilteringLazy::setOrderConditions(const std::initializer_list<FilterTypes> &filtersList)
{
    std::vector<dbTypes> filters;
    for(const auto& filter : filtersList)
    {
        dbTypes dt = std::visit([=](auto&& arg) -> dbTypes {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, dbTypes>) {
                    return arg;
                }
                else {return {};}
            }, filter);

        filters.push_back(dt);
    }
    mOrderConditions=filters;
}

void MariadbFilteringLazy::setGroupConditions(const std::initializer_list<FilterTypes> &filtersList)
{
    std::vector<dbTypes> filters;
    for(const auto& filter : filtersList)
    {
        dbTypes dt = std::visit([=](auto&& arg) -> dbTypes {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, dbTypes>) {
                    return arg;
                }
                else {return {};}
            }, filter);

        filters.push_back(dt);
    }
    mGroupConditions=filters;
}

void MariadbFilteringLazy::setWhereConditions(const Filters &filter, const std::initializer_list<FilterTypes> &filtersList)
{
    std::vector<dbTypes> filters;
    for(const auto& item : filtersList)
    {
        dbTypes dt = std::visit([=](auto&& arg) -> dbTypes {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, dbTypes>) {
                    return arg;
                }
                else {return {};}
            }, item);

        filters.push_back(dt);
    }
    WherePair wp;
    wp.first=filter;
    wp.second=filters;
    std::vector<WherePair> vwp;
    vwp.push_back(wp);
    mWhereConditions.push_back(vwp);
}



void MariadbFilteringLazy::appendWhere(std::string &retStr)
{
    if(mWhereConditions.empty())
    {
        return;
    }

    size_t conditionsSize = mWhereConditions.size();
    for(int i=0;  i<conditionsSize; i++)
    {
        auto whereItems = filterTypesToVector<WherePair>(mWhereConditions.at(i));// std::visit(FilterTypeToWhere{}, mWhereConditions.at(i));
        for(const auto& item :whereItems)
        {
            if(item.first==Filters::AND || item.first==Filters::OR)
            {
                if(i==0)
                {
                    retStr.append("WHERE ");
                } else {
                    retStr.append(filterStr(item.first));
                    retStr.append(" ");
                }

    //            std::vector<filterPair> nested ;//= std::visit(filterTypeToNested{}, item);
    //            if(!nested.empty())
    //            {
    //                sss.append("nested \n");
    //            }
    //            else
                if(item.second.size()==2)
                {
                    retStr.append("`");
                    retStr.append(toStringVal(item.second.at(0)));
                    retStr.append("` = '");
                    retStr.append(toStringVal(item.second.at(1)));
                    retStr.append("' ");
                }
                else if(item.second.size()==3)
                {
                    retStr.append("`");
                    retStr.append(toStringVal(item.second.at(0)));
                    retStr.append("` ");
                    retStr.append(toStringVal(item.second.at(1)));
                    retStr.append(" '");
                    retStr.append(toStringVal(item.second.at(2)));
                    retStr.append("' ");
                }
            }
        }
        retStr.append("\n ");
    }
}

void MariadbFilteringLazy::appendOrderby(std::string &retStr)
{
    if(empty(mOrderConditions))
    {
        return;
    }

    retStr.append("ORDER BY ");
    if (std::holds_alternative<std::vector<dbTypes>>(mOrderConditions))
    {
        std::vector<dbTypes> dt = filterTypesToVector<dbTypes>(mOrderConditions);
        retStr.append(toStringVal(dt.at(0)));
        retStr.append(",");
        retStr.append(toStringVal(dt.at(1)));
    }
    else if(std::holds_alternative<dbTypes>(mOrderConditions))
    {
        retStr.append(toStringVal(mOrderConditions));
    }
    retStr.append("\n ");
}

void MariadbFilteringLazy::appendLimit(std::string &retStr)
{
    if(empty(mLimitConditions))
    {
        return;
    }

    retStr.append("LIMIT ");
    if (std::holds_alternative<std::vector<dbTypes>>(mLimitConditions))
    {
        std::vector<dbTypes> dt = filterTypesToVector<dbTypes>(mLimitConditions);
        retStr.append(toStringVal(dt.at(0)));
        retStr.append(",");
        retStr.append(toStringVal(dt.at(1)));
    }
    else if(std::holds_alternative<dbTypes>(mLimitConditions))
    {
        retStr.append(toStringVal(mLimitConditions));
    }
    retStr.append("\n ");
}

void MariadbFilteringLazy::appendGroup(std::string &retStr)
{
    if(empty(mGroupConditions))
    {
        return;
    }

    retStr.append("GROUP BY ");
    if (std::holds_alternative<std::vector<dbTypes>>(mGroupConditions))
    {
        std::vector<dbTypes> dt = filterTypesToVector<dbTypes>(mGroupConditions);
        retStr.append(toStringVal(dt.at(0)));
        retStr.append(",");
        retStr.append(toStringVal(dt.at(1)));
    }
    else if(std::holds_alternative<dbTypes>(mGroupConditions))
    {
        retStr.append(toStringVal(mGroupConditions));
    }
    retStr.append("\n ");
}

std::string MariadbFilteringLazy::where_conditions()
{
    return "WHERE ... mariadb... ";
}
}
