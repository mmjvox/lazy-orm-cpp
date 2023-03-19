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
//typedef std::variant<DbVariant, std::vector<WherePair>> WhereTypes;

class WherePair : public std::vector<std::variant<DbVariant, std::vector<WherePair>>> {

public:
    using std::vector<std::variant<DbVariant, std::vector<WherePair>>>::vector;
    Filters filter;


    WherePair(){}
    WherePair( Filters filter,
            std::vector<std::variant<DbVariant, std::vector<WherePair>>> whereFilters)
        : std::vector<std::variant<DbVariant, std::vector<WherePair>>>(whereFilters), filter{filter}
    {
    }

    // TODO: replace this with correct depth
    WherePair * depth(int index){return this;}


    struct FilterVariantToString
    {
      std::string operator()(const std::vector<WherePair> &value);
      std::string operator()(const DbVariant &value);
    };

    // TODO: replace this with correct depth
    std::string stringValue(size_t index){
        return std::visit(FilterVariantToString{}, this->at(index));
    }
};

class FilterVariant : public std::variant<DbVariant, std::vector<DbVariant>, std::vector<WherePair>>
{
public:
    using std::variant<DbVariant, std::vector<DbVariant>, std::vector<WherePair>>::variant;

    struct FilterVariantToString
    {
      std::string operator()(const std::vector<WherePair> &value);
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
