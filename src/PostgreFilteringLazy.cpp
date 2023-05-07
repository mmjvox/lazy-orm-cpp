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

void PostgreFilteringLazy::setWhereConditions(const Filters &filter, const std::initializer_list<FilterVariant> &filtersList)
{
    std::vector<DbVariant> conditions;
    for(const auto& item : filtersList)
    {
        bool singleVal =
        std::visit([=, &conditions](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, WhereFilter>) {
                mWhereConditions = arg;
                return true;
            }
            else
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
    mWhereConditions = WhereFilter(filter, conditions);
}

void PostgreFilteringLazy::setHavingConditions(const std::vector<FilterVariant> &filtersList)
{
    mHavingConditions=filtersList;
}


void PostgreFilteringLazy::nestedWhereToString(WhereType<WhereFilter> whereItem, std::string &retStr, Filters whereFilter, bool firstItem) const
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

void PostgreFilteringLazy::appendWhere(std::string &retStr) const
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

void PostgreFilteringLazy::appendOrderby(std::string &retStr) const
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
    retStr.append(" ");
}

void PostgreFilteringLazy::appendLimit(std::string &retStr) const
{
    if(mLimitConditions.empty())
    {
        return;
    }

    retStr.append("LIMIT ");
    if (std::holds_alternative<std::vector<DbVariant>>(mLimitConditions))
    {
        std::vector<DbVariant> dt = mLimitConditions.filterTypesToVector<DbVariant>();

        if(dt.size()==2)
        {
          retStr.append(dt.at(0).toString());
          retStr.append(" OFFSET ");
          retStr.append(dt.at(1).toString());
        } else {
          for(const auto& item : dt)
          {
            retStr.append(item.toString()+" ");
          }
        }
    }
    else if(std::holds_alternative<DbVariant>(mLimitConditions))
    {
        retStr.append(mLimitConditions.toString());
    }
    retStr.append(" ");
}

void PostgreFilteringLazy::appendGroup(std::string &retStr) const
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
    retStr.append(" ");
}

void PostgreFilteringLazy::appendHaving(std::string &retStr) const
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
    }, havingItem);
  }
}


std::string PostgreFilteringLazy::where_conditions() const
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
