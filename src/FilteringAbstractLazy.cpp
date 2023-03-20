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

void FilteringAbstractLazy::setFilter(std::initializer_list<LazyOrm::FilterVariant> filterVariantList)
{
    std::vector<DbVariant> conditions;
    for(const auto& item : filterVariantList)
    {
        std::visit([=, &conditions](auto&& arg){
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, WhereFilter>) {
                mWhereConditions = arg;
                // TODO: return and stop
            }
            else
            if constexpr (std::is_same_v<T, DbVariant>) {
                conditions.push_back(arg);
            }
        }, item);

    }

    if(conditions.size()<1)
    {
        return;
    }

    // TODO:
    const auto filter = conditions.at(0).toLowerString();
    if(filter=="and")
    {
        conditions.erase(conditions.begin());
        setWhereConditions(AND, filterVariantList);
    }
    else if(filter=="or")
    {
        conditions.erase(conditions.begin());
        setWhereConditions(OR, filterVariantList);
    }
    else if(filter=="having")
    {
        conditions.erase(conditions.begin());
        setHavingConditions(filterVariantList);
    }
    else if(filter=="orderby")
    {
        conditions.erase(conditions.begin());
        setOrderConditions(filterVariantList);
    }
    else if(filter=="groupby")
    {
        conditions.erase(conditions.begin());
        setGroupConditions(filterVariantList);
    }
    else if(filter=="limit")
    {
        conditions.erase(conditions.begin());
        setLimitConditions(filterVariantList);
    } else {
        setWhereConditions(AND, filterVariantList);
    }
}

//template <typename T>
//void FilteringAbstractLazy::setFilter(const Filters &filter, T&& arg)
void FilteringAbstractLazy::setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterVariant> filterVariantList)
{
    switch (filter) {
    case OR:
    case AND:
        setWhereConditions(filter, filterVariantList);
        break;
    case HAVING:
        setHavingConditions(filterVariantList);
        break;
    case ORDERBY:
        setOrderConditions(filterVariantList);
        break;
    case GROUPBY:
        setGroupConditions(filterVariantList);
        break;
    case LIMIT:
        setLimitConditions(filterVariantList);
        break;
    }
}

void FilteringAbstractLazy::setFilter(const Filters &filter, FilterVariant filterVariant)
{
    switch (filter) {
    case OR:
    case AND:
        setWhereConditions(filter, {filterVariant});
        break;
    case LIMIT:
        mLimitConditions=filterVariant;
        break;
    case ORDERBY:
        mOrderConditions=filterVariant;
        break;
    case HAVING:
        mHavingConditions.push_back(filterVariant);
        break;
    case GROUPBY:
        mGroupConditions=filterVariant;
        break;
    default:
        break;
    }
}

void FilteringAbstractLazy::setFilter(const Filters &filter, WhereFilter whereFilter)
{
    switch (filter) {
    case OR:
    case AND:
        mWhereConditions=whereFilter;
        break;
    default:
        break;
    }
}

//void FilteringAbstractLazy::setFilter(const Filters &filter, WhereTypes &f)
//{
//    mWhereConditions.push_back({filter, {f}});
//}

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
