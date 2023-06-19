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
    std::vector<DbVariant> conditions;
    for(const auto& item : filtersList)
    {
        bool singleVal =
        std::visit([=, &conditions](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
//            if constexpr (std::is_same_v<T, WhereFilter>) {
//                mWhereConditions = arg;
//                return true;
//            }
//            else
            if constexpr (std::is_same_v<T, DbVariant>) {
                conditions.push_back(arg);
            }
            return false;
        }, item);

        if(singleVal)
        {
          break;
        }
    }
//    mWhereConditions = WhereFilter(filter, conditions);
}

void MariadbFilteringLazy::setHavingConditions(const std::vector<FilterVariant> &filtersList)
{
    mHavingConditions=filtersList;
}


//void MariadbFilteringLazy::nestedWhereToString(WhereFilter whereItem, std::string &retStr, Filters whereFilter, bool firstItem) const
//{
//    if(!firstItem)
//    {
//        retStr.append(filterStr(whereFilter));
//        retStr.append(" ");
//    }

//    std::visit([=,&retStr](auto&& arg){
//        using T = std::decay_t<decltype(arg)>;
//        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {

//            if(whereFilter==Filters::AND || whereFilter==Filters::OR)
//            {
//                if(arg.size()==1)
//                {
//                    retStr.append(arg.at(0).toCleanString());
//                }
//                else if(arg.size()==2)
//                {
//                    retStr.append(arg.at(0).setBackTick());
//                    retStr.append(" = ");
//                    retStr.append(arg.at(1).setQuote());
//                    retStr.append(" ");
//                }
//                else if(arg.size()==3)
//                {
//                    retStr.append(arg.at(0).setBackTick());
//                    retStr.append(" ");
//                    retStr.append(arg.at(1).toString());
//                    retStr.append(" ");
//                    retStr.append(arg.at(2).setQuote());
//                    retStr.append(" ");
//                }
//            }
//        }
//        else if constexpr (std::is_same_v<T, std::vector<WhereFilter>>) {

//            retStr.append(" (");
//            bool firstItem=true;
//            for(const auto& vectorItem : arg)
//            {
//                for(const auto& item : vectorItem)
//                {
//                    nestedWhereToString(item, retStr, vectorItem.filter, firstItem);
//                    firstItem=false;
//                }
//            }
//            retStr.append(") ");
//        }
//    }, whereItem);
//}

void MariadbFilteringLazy::appendWhere(std::string &retStr) const
{
//    if(mWhereConditions.empty())
//    {
//        return;
//    }

//    retStr.append(" WHERE ");

//    bool firstItem=true;
//    for(const auto& whereItem : mWhereConditions)
//    {
//        nestedWhereToString(whereItem, retStr, mWhereConditions.filter, firstItem);
//        firstItem=false;
//    }

//    std::visit([=,&retStr](auto&& arg){
//        using T = std::decay_t<decltype(arg)>;
//        if constexpr (std::is_same_v<T, WhereFilter>) {
//            bool firstItem=true;
//            for(const auto& whereItem : arg)
//            {
//                nestedWhereToString(whereItem, retStr, arg.filter, firstItem);
//                firstItem=false;
//            }
//        }
//    }, mWhereConditions);
}

void MariadbFilteringLazy::appendOrderby(std::string &retStr) const
{
    if(mOrderConditions.empty())
    {
        return;
    }

    retStr.append("ORDER BY ");

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

void MariadbFilteringLazy::appendLimit(std::string &retStr) const
{
    if(mLimitConditions.empty())
    {
        return;
    }

    retStr.append("LIMIT ");

    std::visit([&retStr, this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<DbVariant>>) {
            retStr.append(string_join(",",arg));
        }
        else if constexpr (std::is_same_v<T, DbVariant>) {
            retStr.append(arg.toString());
        }
    }, mLimitConditions);

    retStr.append(" ");
}

void MariadbFilteringLazy::appendGroup(std::string &retStr) const
{
    if(mGroupConditions.empty())
    {
        return;
    }

    retStr.append("GROUP BY ");

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

void MariadbFilteringLazy::appendHaving(std::string &retStr) const
{
  if(mHavingConditions.empty())
  {
      return;
  }

  retStr.append("HAVING ");

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
                retStr.append(arg.at(1).setQuote());
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


std::string MariadbFilteringLazy::where_conditions() const
{
    std::string retStr;
    // AND OR
    appendWhere(retStr);

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

}
