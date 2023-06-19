#include "FilteringAbstractLazy.h"
#include <iostream>

namespace LazyOrm {


std::string FilteringAbstractLazy::filterStr(Filters f) const
{
    switch (f) {
    case Filters::LIMIT:
        return "LIMIT";
    case Filters::HAVING:
        return "HAVING";
    case Filters::ORDERBY:
        return "ORDER BY";
    case Filters::GROUPBY:
        return "GROUP BY";
    default:
        return {};
    }
    return {};
}

FilteringAbstractLazy::FilteringAbstractLazy()
{
}

void FilteringAbstractLazy::setFilterForReserved(const FilterVariant &variant)
{
  setFilter(mReservedFilter, variant);
  mReservedFilter = Filters::None;
}

void FilteringAbstractLazy::setFilterForReserved(const std::vector<FilterVariant> &variantList)
{
  switch (mReservedFilter) {
  case Filters::HAVING:
      setHavingConditions(variantList);
      break;
  default:
  break;
  }
}

void FilteringAbstractLazy::setFilter(std::initializer_list<LazyOrm::FilterVariant> filterVariantList)
{
    std::vector<DbVariant> conditions;
    for(const auto& item : filterVariantList)
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

    if(conditions.size()<1)
    {
        return;
    }

    const auto filter = conditions.at(0).toLowerString();
//    if(filter=="and")
//    {
//        conditions.erase(conditions.begin());
//        setWhereConditions(Filters::AND, filterVariantList);
//    }
//    else if(filter=="or")
//    {
//        conditions.erase(conditions.begin());
//        setWhereConditions(Filters::OR, filterVariantList);
//    }
//    else
    if(filter=="having")
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
    }
//    else {
//        setWhereConditions(Filters::AND, filterVariantList);
//    }
}

//template <typename T>
//void FilteringAbstractLazy::setFilter(const Filters &filter, T&& arg)
void FilteringAbstractLazy::setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterVariant> filterVariantList)
{
    switch (filter) {
    case Filters::HAVING:
        setHavingConditions(filterVariantList);
        break;
    case Filters::ORDERBY:
        setOrderConditions(filterVariantList);
        break;
    case Filters::GROUPBY:
        setGroupConditions(filterVariantList);
        break;
    case Filters::LIMIT:
        setLimitConditions(filterVariantList);
        break;
    default:
        break;
    }
}

void FilteringAbstractLazy::setFilter(const Filters &filter, FilterVariant filterVariant)
{
    switch (filter) {
    case Filters::LIMIT:
        mLimitConditions=filterVariant;
        break;
    case Filters::ORDERBY:
        mOrderConditions=filterVariant;
        break;
    case Filters::GROUPBY:
        mGroupConditions=filterVariant;
        break;
    default:
        break;
    }
}

//void FilteringAbstractLazy::setFilter(const Filters &filter, WhereFilter whereFilter)
//{
//    switch (filter) {
//    case Filters::OR:
//    case Filters::AND:
//        mWhereConditions=whereFilter;
//        break;
//    default:
//        break;
//    }
//}

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


FilteringAbstractLazy& FilteringAbstractLazy::operator[](const Filters &filter)
{
    mReservedFilter = filter;
    return *this;
}

FilteringAbstractLazy& FilteringAbstractLazy::operator[](const NestedWhere &nestedWhere)
{
    mReservedWhere = nestedWhere;
    return *this;
}

void FilteringAbstractLazy::operator=(const FilterVariant &variant)
{
  setFilterForReserved(variant);
}

//void FilteringAbstractLazy::setWhereFilter(WhereFilter whereFilter){
//  mWhereConditions=whereFilter;
//}

std::string FilteringAbstractLazy::string_join(const std::string &delimiter, const std::vector<DbVariant> &container) const
{
  size_t size = container.size();
  size_t endPos = container.size()-1;
  std::string output;
  for(size_t i = 0; i < size; ++i) {
        output.append(container[i].toString());
        if(i!=endPos){
          output.append(delimiter);
        }
  }
  return output;
}

}
