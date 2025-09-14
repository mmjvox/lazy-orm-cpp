#include "FilteringAbstractLazy.h"
#include <algorithm>

namespace LazyOrm {


Filters FilteringAbstractLazy::reservedFilter() const
{
    return mReservedFilter;
}

std::string FilteringAbstractLazy::filterStr(Filters f) const
{
    switch (f) {
    case Filters::LIMIT:
        return "LIMIT";
    case Filters::ORDERBY:
    case Filters::ORDERBY_DESC:
    case Filters::ORDERBY_ASC:
        return "ORDER BY";
    case Filters::GROUPBY:
        return "GROUP BY";
    default:
        return {};
    }
    return {};
}

const std::string FilteringAbstractLazy::setBackTickForOrderType(DbVariant var) const
{
    const auto &lowercaseArg = var.toLowerString();

    const auto &descPos = lowercaseArg.find("[desc]");
    if(descPos!=std::string::npos){
        return DbVariant(var.toString().substr(0, descPos)).setBackTick().append(" DESC");
    }

    const auto &ascPos = lowercaseArg.find("[asc]");
    if(ascPos!=std::string::npos){
        return DbVariant(var.toString().substr(0, ascPos)).setBackTick().append(" ASC");
    }

    if(mOrderByType==ORDERBY_DESC){
       return var.setBackTick().append(" DESC");
    }

    if(mOrderByType==ORDERBY_ASC){
      return  var.setBackTick().append(" ASC");
    }

    return var.setBackTick();
}

std::string FilteringAbstractLazy::filter_conditions() const
{
    return

        // GROUP BY
        groupString() +

        // ORDER BY
        orderbyString() +

        // LIMIT
        limitString();
}



std::string FilteringAbstractLazy::filter_conditions_with_trim_consecutive_spaces(){
    std::string str = filter_conditions();
    auto new_end = std::unique(str.begin(), str.end(), [](char a, char b) {
        return a == ' ' && b == ' ';
    });
    str.erase(new_end, str.end());
    return str;
}

FilterVariant FilteringAbstractLazy::groupConditions() const
{
    return mGroupConditions;
}

FilterVariant FilteringAbstractLazy::orderConditions() const
{
    return mOrderConditions;
}

FilterVariant FilteringAbstractLazy::limitConditions() const
{
    return mLimitConditions;
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
  // TODO: implement later if needed
}

void FilteringAbstractLazy::setFilter(std::initializer_list<LazyOrm::FilterVariant> filterVariantList)
{
    std::vector<DbVariant> conditions;
    for(const auto& item : filterVariantList)
    {
        bool singleVal =
        std::visit([=, &conditions](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
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

    if(filter=="orderby")
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
}

//template <typename T>
//void FilteringAbstractLazy::setFilter(const Filters &filter, T&& arg)
void FilteringAbstractLazy::setFilter(const Filters &filter, std::initializer_list<LazyOrm::FilterVariant> filterVariantList)
{
    switch (filter) {
    case Filters::ORDERBY:
    case Filters::ORDERBY_DESC:
    case Filters::ORDERBY_ASC:{
            mOrderByType = filter;
            setOrderConditions(filterVariantList);
        }
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
    case Filters::ORDERBY_DESC:
    case Filters::ORDERBY_ASC:
        mOrderConditions=filterVariant;
        break;
    case Filters::GROUPBY:
        mGroupConditions=filterVariant;
        break;
    default:
        break;
    }
}

FilteringAbstractLazy& FilteringAbstractLazy::operator[](const Filters &filter)
{
    mReservedFilter = filter;
    if(filter == ORDERBY || filter == ORDERBY_DESC || filter == ORDERBY_ASC){
        mOrderByType = filter;
    }
    return *this;
}

void FilteringAbstractLazy::operator=(const FilterVariant &variant)
{
  setFilterForReserved(variant);
}

void FilteringAbstractLazy::appendFilter(const Filters &filter, DbVariant &dbVariant)
{
    switch (filter) {
    case None:
        break;
    case ORDERBY:
    case ORDERBY_DESC:
    case ORDERBY_ASC:
        mOrderConditions.append(dbVariant);
        break;
    case LIMIT:
        mLimitConditions.append(dbVariant);
        break;
    case GROUPBY:
        mGroupConditions.append(dbVariant);
        break;
    }
}

void FilteringAbstractLazy::setDESC(bool desc)
{
    if(mOrderByType == ORDERBY and desc){
        mOrderByType = ORDERBY_DESC;
    }
    else if(mOrderByType == ORDERBY_DESC and !desc){
        mOrderByType = ORDERBY;
    }
}

void FilteringAbstractLazy::setASC(bool asc)
{
    if(mOrderByType == ORDERBY and asc){
        mOrderByType = ORDERBY_ASC;
    }
    else if(mOrderByType == ORDERBY_ASC and !asc){
        mOrderByType = ORDERBY;
    }
}

std::string FilteringAbstractLazy::string_join(const std::string &delimiter, const std::vector<DbVariant> &container, QuoteFor setQuote) const
{
  size_t size = container.size();
  size_t endPos = container.size()-1;
  std::string output;
  for(size_t i = 0; i < size; ++i) {
        if(setQuote==GroupType){
            output.append(container[i].setBackTick());
        }
        else if(setQuote==OrderType){
            output.append(setBackTickForOrderType(container[i]));
        }
        else if(setQuote==LimitType){
            output.append(container[i].toString());
        }
        else {
            output.append(container[i].toString());
        }
        if(i!=endPos){
          output.append(delimiter);
        }
  }
  return output;
}

}
