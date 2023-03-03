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

bool FilteringAbstractLazy::empty(const FilterTypes &value)
{
    return std::visit([=](auto&& arg) -> bool {

        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::vector<WherePair>>) {
            if(!arg.empty())
            {
                return false;
            }
        }

        if constexpr (std::is_same_v<T, std::vector<dbTypes>>) {
            if(!arg.empty())
            {
                return false;
            }
        }

        if constexpr (std::is_same_v<T, dbTypes>) {
            if(!toStringVal(arg).empty())
            {
                return false;
            }
        }

        return true;

    }, value);
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
    // AND OR
    appendWhere(retStr);

    // GROUP BY
    appendGroup(retStr);

    // ORDER BY
    appendOrderby(retStr);

    // LIMIT
    appendLimit(retStr);

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
