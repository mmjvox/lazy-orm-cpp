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

typedef std::pair<Filters,std::vector<LazyOrm::DbVariant>> WherePair;
//typedef std::variant<DbVariant, std::vector<WherePair>> WhereTypes;

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
