#ifndef FILTERVARIANT_H
#define FILTERVARIANT_H

#include "DbVariant.h"

#include <string>
#include <variant>
#include <list>
#include <vector>


namespace LazyOrm {

enum Filters
{
  AND   = 0,
  OR    = 2,
  ORDERBY,
  LIMIT,
  HAVING,
  GROUPBY
};

//typedef std::pair<Filters,std::vector<LazyOrm::DbVariant>> WherePair;
template <typename WF>
using WhereType = std::variant<std::vector<DbVariant>, std::vector<WF>>;

class WhereFilter : public std::vector<WhereType<WhereFilter>> {

public:
    using std::vector<WhereType<WhereFilter>>::vector;
    Filters filter;


    WhereFilter(){}
    WhereFilter( Filters filter, std::vector<WhereType<WhereFilter>> whereFilters)
        : std::vector<WhereType<WhereFilter>>(whereFilters), filter{filter}
    {
    }

    WhereFilter( Filters filter, std::vector<DbVariant> whereFilters)
        : filter{filter}
    {
        this->push_back(whereFilters);
    }

    WhereFilter(std::vector<DbVariant> whereFilters)
    {
        filter = AND;
        this->push_back(whereFilters);
    }

    // TODO: replace this with correct depth
    WhereFilter * depth(){return this;}


    struct FilterVariantToString
    {
      std::string operator()(const std::vector<WhereFilter> &value);
      std::string operator()(const std::vector<DbVariant> &value);
    };

    // TODO: replace this with correct depth
    std::string stringValue(size_t index){
        return std::visit(FilterVariantToString{}, this->at(index));
    }
};

class FilterVariant : public std::variant<DbVariant, std::vector<DbVariant>, std::vector<WhereFilter>>
{
public:
    using std::variant<DbVariant, std::vector<DbVariant>, std::vector<WhereFilter>>::variant;

    struct FilterVariantToString
    {
      std::string operator()(const std::vector<WhereFilter> &value);
      std::string operator()(const std::vector<DbVariant> &value);
      std::string operator()(const DbVariant &value);
    };

    std::string toString();

    bool empty();

    template <typename FT>
    std::vector<FT> filterTypesToVector()
    {
        return std::visit([=](auto&& arg) -> std::vector<FT> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<FT>>) {
                return arg;
            }
            return {};
        }, *this);
    }
};
}

#endif // FILTERVARIANT_H
