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
void FilteringAbstractLazy::setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterTypes> filters)
{
//    for (const auto& filter : filters) {
//            if (std::holds_alternative<std::vector<WherePair>>(filter)) {
//                std::cout << "Found a std::vector<WherePair> filter\n";
//            } else if (std::holds_alternative<std::vector<dbTypes>>(filter)) {
//                std::cout << "Found a std::vector<dbTypes> filter\n";
//            } else {
//                std::cout << "Found a dbTypes filter\n";
//            }
//        }


    switch (filter) {
    case OR:
    case AND:
//        static_cast<std::vector<WhereTypes>>(arg)
//        mWhereConditions.push_back({filter, f});
        break;
    case ORDERBY:
        break;
    case LIMIT:
        if(filters.size()==2)
        {
            std::vector<dbTypes> limitF;
            for(const auto& filter : filters)
            {
                dbTypes dt = std::visit([=](auto&& arg) -> dbTypes {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, dbTypes>) {
                            return arg;
                        }
                        else {return {};}
                    }, filter);

                limitF.push_back(dt);
            }
            mLimitConditions.push_back(limitF);
        }
        else if(filters.size()==1)
        {
            if(std::holds_alternative<LazyOrm::dbTypes>(*filters.begin()))
            {
                mLimitConditions.push_back(*filters.begin());
            }
        }
        break;
    case HAVING:
        break;
    case GROUPBY:
        break;
    }
}

void FilteringAbstractLazy::setFilter(const Filters &filter, FilterTypes f)
{
    switch (filter) {
    case LIMIT:
        mLimitConditions.push_back(f);
        break;
    case ORDERBY:
        mOrderConditions.push_back(f);
        break;
    case HAVING:
        mHavingConditions.push_back(f);
        break;
    case GROUPBY:
        mGroupConditions.push_back(f);
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

    // LIMIT
    conditionsSize = mLimitConditions.size();
    for(int i=0;  i<conditionsSize; i++)
    {
        auto &item = mLimitConditions.at(i);
        retStr.append("LIMIT ");
        if (std::holds_alternative<std::vector<dbTypes>>(item))
        {
            std::vector<dbTypes> dt = std::visit([=](auto&& arg) -> std::vector<dbTypes> {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::vector<dbTypes>>) {
                        return arg;
                    }
                    else {return {};}
                }, item);
            retStr.append(toStringVal(dt.at(0)));
            retStr.append(",");
            retStr.append(toStringVal(dt.at(1)));
        }
        else if(std::holds_alternative<dbTypes>(item))
        {
            retStr.append(toStringVal(item));
        }
        retStr.append("\n ");
    }
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
