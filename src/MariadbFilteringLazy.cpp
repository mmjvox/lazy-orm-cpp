#include "MariadbFilteringLazy.h"

namespace LazyOrm {
MariadbFilteringLazy::MariadbFilteringLazy()
{

}

void MariadbFilteringLazy::setLimitConditions(const std::initializer_list<FilterVariant> &filtersList)
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

void MariadbFilteringLazy::setOrderConditions(const std::initializer_list<FilterVariant> &filtersList)
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

void MariadbFilteringLazy::setGroupConditions(const std::initializer_list<FilterVariant> &filtersList)
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

void MariadbFilteringLazy::setWhereConditions(const Filters &filter, const std::initializer_list<FilterVariant> &filtersList)
{
    std::vector<DbVariant> filters;
    for(const auto& item : filtersList)
    {
        DbVariant dt = std::visit([=](auto&& arg) -> DbVariant {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, DbVariant>) {
                    return arg;
                }
                else {return {};}
            }, item);

        filters.push_back(dt);
    }
//    WhereFilter wp;
//    wp.filter=filter;
//    wp.second=filters;
//    std::vector<WhereFilter> vwp;
//    vwp.push_back(wp);
//    mWhereConditions.push_back(vwp);
}


void MariadbFilteringLazy::nestedWhereToString(WhereType<WhereFilter> whereItem, std::string &retStr, Filters whereFilter, bool firstItem)
{
    if(!firstItem)
    {
        retStr.append(filterStr(whereFilter));
        retStr.append(" ");
    }

    std::visit([=,&retStr](auto&& arg){
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {

            if(whereFilter==Filters::AND || whereFilter==Filters::OR)
            {
                if(arg.size()==1)
                {
                    retStr.append(arg.at(0).toString());
                }
                else if(arg.size()==2)
                {
                    retStr.append("`");
                    retStr.append(arg.at(0).toString());
                    retStr.append("` = '");
                    retStr.append(arg.at(1).toString());
                    retStr.append("' ");
                }
                else if(arg.size()==3)
                {
                    retStr.append("`");
                    retStr.append(arg.at(0).toString());
                    retStr.append("` ");
                    retStr.append(arg.at(1).toString());
                    retStr.append(" '");
                    retStr.append(arg.at(2).toString());
                    retStr.append("' ");
                }
            }
        }
        else if constexpr (std::is_same_v<T, std::vector<WhereFilter>>) {

            retStr.append(" (");
            bool firstItem=true;
            for(const auto& vectorItem : arg)
            {
                for(const auto& item : vectorItem)
                {
                    nestedWhereToString(item, retStr, vectorItem.filter, firstItem);
                    firstItem=false;
                }
            }
            retStr.append(") ");
        }
    }, whereItem);
}

void MariadbFilteringLazy::appendWhere(std::string &retStr)
{
    if(mWhereConditions.empty())
    {
        return;
    }

    retStr.append("WHERE ");

    bool firstItem=true;
    for(const auto& whereItem : mWhereConditions)
    {
        nestedWhereToString(whereItem, retStr, mWhereConditions.filter, firstItem);
        firstItem=false;
    }
}

void MariadbFilteringLazy::appendOrderby(std::string &retStr)
{
    if(mOrderConditions.empty())
    {
        return;
    }

    retStr.append("ORDER BY ");
    if (std::holds_alternative<std::vector<DbVariant>>(mOrderConditions))
    {
        std::vector<DbVariant> dt = mOrderConditions.filterTypesToVector<DbVariant>();
        retStr.append(dt.at(0).toString());
        retStr.append(",");
        retStr.append(dt.at(1).toString());
    }
    else if(std::holds_alternative<DbVariant>(mOrderConditions))
    {
        retStr.append(mOrderConditions.toString());
    }
    retStr.append("\n ");
}

void MariadbFilteringLazy::appendLimit(std::string &retStr)
{
    if(mLimitConditions.empty())
    {
        return;
    }

    retStr.append("LIMIT ");
    if (std::holds_alternative<std::vector<DbVariant>>(mLimitConditions))
    {
        std::vector<DbVariant> dt = mLimitConditions.filterTypesToVector<DbVariant>();
        retStr.append(dt.at(0).toString());
        retStr.append(",");
        retStr.append(dt.at(1).toString());
    }
    else if(std::holds_alternative<DbVariant>(mLimitConditions))
    {
        retStr.append(mLimitConditions.toString());
    }
    retStr.append("\n ");
}

void MariadbFilteringLazy::appendGroup(std::string &retStr)
{
    if(mGroupConditions.empty())
    {
        return;
    }

    retStr.append("GROUP BY ");
    if (std::holds_alternative<std::vector<DbVariant>>(mGroupConditions))
    {
        std::vector<DbVariant> dt = mGroupConditions.filterTypesToVector<DbVariant>();
        retStr.append(dt.at(0).toString());
        retStr.append(",");
        retStr.append(dt.at(1).toString());
    }
    else if(std::holds_alternative<DbVariant>(mGroupConditions))
    {
        retStr.append(mGroupConditions.toString());
    }
    retStr.append("\n ");
}

std::string MariadbFilteringLazy::where_conditions()
{
    return "WHERE ... mariadb... ";
}
}
