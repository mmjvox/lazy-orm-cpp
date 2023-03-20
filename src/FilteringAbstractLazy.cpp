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

void FilteringAbstractLazy::setFilter(std::initializer_list<FilterVariant> f)
{
//    WherePair wp;
//    wp.first = Filters::AND;
}

//template <typename T>
//void FilteringAbstractLazy::setFilter(const Filters &filter, T&& arg)
void FilteringAbstractLazy::setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterVariant> filtersList)
{




    switch (filter) {
    case OR:
    case AND:
        setWhereConditions(filter, filtersList);
        break;
    case HAVING:
        break;
    case ORDERBY:
        setOrderConditions(filtersList);
        break;
    case GROUPBY:
        setGroupConditions(filtersList);
        break;
    case LIMIT:
        setLimitConditions(filtersList);
        break;
    }
}

void FilteringAbstractLazy::setFilter(const Filters &filter, FilterVariant f)
{
    switch (filter) {
    case OR:
    case AND:
//        mWhereConditions.push_back(f);
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
