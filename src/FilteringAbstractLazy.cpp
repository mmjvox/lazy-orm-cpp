#include "FilteringAbstractLazy.h"
#include <iostream>

namespace LazyOrm {


std::string FilteringAbstractLazy::filterStr(Filters f)
{
    switch (f) {
    case OR:
        return "OR";
    case AND:
        return "AND";
    case LIMIT:
        return "LIMIT";
    case HAVING:
        return "HAVING";
    case ORDERBY:
        return "ORDER BY";
    case GROUPBY:
        return "GROUP BY";
    }
    return {};
}

FilteringAbstractLazy::FilteringAbstractLazy()
{

}

std::string FilteringAbstractLazy::toStringVal(const FilterTypes &value){
  return std::visit(FilterTypeToString{}, value);
}

void FilteringAbstractLazy::setFilter(std::initializer_list<FilterTypes> f)
{
    WherePair wp;
    wp.first = Filters::AND;
//    wp.second.push_back(f.begin());
//    mWhereConditions.push_back({Filters::AND, f});
}

//template <typename T>
//void FilteringAbstractLazy::setFilter(const Filters &filter, T&& arg)
void FilteringAbstractLazy::setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterTypes> filtersList)
{




    switch (filter) {
    case OR:
    case AND:
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
        break;
    case HAVING:
        break;
    case ORDERBY:
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
        break;
    case GROUPBY:
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
        break;
    case LIMIT:
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
        break;
    }
}

void FilteringAbstractLazy::setFilter(const Filters &filter, FilterTypes f)
{
    switch (filter) {
    case OR:
    case AND:
        mWhereConditions.push_back(f);
        break;
    case LIMIT:
        mLimitConditions=f;
        break;
    case ORDERBY:
        mOrderConditions=f;
        break;
    case HAVING:
        mHavingConditions.push_back(f);
        break;
    case GROUPBY:
        mGroupConditions=f;
        break;
    default:
        break;
    }
}

//void FilteringAbstractLazy::setFilter(const Filters &filter, WhereTypes &f)
//{
//    mWhereConditions.push_back({filter, {f}});
//}


std::string FilteringAbstractLazy::testString()
{
    std::string retStr;
    size_t conditionsSize = mWhereConditions.size();
    // AND OR
    for(int i=0;  i<conditionsSize; i++)
    {
        auto whereItems = std::visit(FilterTypeToWhere{}, mWhereConditions.at(i));
        for(const auto& item :whereItems)
        {
            if(item.first==Filters::AND || item.first==Filters::OR)
            {
                if(i>0)
                {
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

    // GROUP BY
    retStr.append("GROUP BY ");
    if (std::holds_alternative<std::vector<dbTypes>>(mGroupConditions))
    {
        std::vector<dbTypes> dt = std::visit([=](auto&& arg) -> std::vector<dbTypes> {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::vector<dbTypes>>) {
                    return arg;
                }
                else {return {};}
            }, mGroupConditions);
        retStr.append(toStringVal(dt.at(0)));
        retStr.append(",");
        retStr.append(toStringVal(dt.at(1)));
    }
    else if(std::holds_alternative<dbTypes>(mGroupConditions))
    {
        retStr.append(toStringVal(mGroupConditions));
    }
    retStr.append("\n ");

    // LIMIT
    retStr.append("ORDER BY ");
    if (std::holds_alternative<std::vector<dbTypes>>(mOrderConditions))
    {
        std::vector<dbTypes> dt = std::visit([=](auto&& arg) -> std::vector<dbTypes> {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::vector<dbTypes>>) {
                    return arg;
                }
                else {return {};}
            }, mOrderConditions);
        retStr.append(toStringVal(dt.at(0)));
        retStr.append(",");
        retStr.append(toStringVal(dt.at(1)));
    }
    else if(std::holds_alternative<dbTypes>(mOrderConditions))
    {
        retStr.append(toStringVal(mOrderConditions));
    }
    retStr.append("\n ");

    // LIMIT
    retStr.append("LIMIT ");
    if (std::holds_alternative<std::vector<dbTypes>>(mLimitConditions))
    {
        std::vector<dbTypes> dt = std::visit([=](auto&& arg) -> std::vector<dbTypes> {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::vector<dbTypes>>) {
                    return arg;
                }
                else {return {};}
            }, mLimitConditions);
        retStr.append(toStringVal(dt.at(0)));
        retStr.append(",");
        retStr.append(toStringVal(dt.at(1)));
    }
    else if(std::holds_alternative<dbTypes>(mLimitConditions))
    {
        retStr.append(toStringVal(mLimitConditions));
    }
    retStr.append("\n ");

    return retStr;
}

//void FilteringAbstractLazy::test_init(std::initializer_list<filterTypes> f)
//{
//    for(const auto &item : f)
//    {
//        std::cout <<"*--> "<< std::visit(dbTypeToString{}, item) << std::endl;
//    }
//}

//filterTypes &FilteringAbstractLazy::operator[](const Filters &filter)
//{
//    return NULL;
//}
}
