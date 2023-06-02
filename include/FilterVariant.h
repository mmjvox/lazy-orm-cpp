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
  None = -10,
  WHERE = 0,
  AND = 1,
  OR,
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


    WhereFilter();
    WhereFilter( Filters filter, std::vector<WhereType<WhereFilter>> whereFilters);
    WhereFilter( Filters filter, std::vector<DbVariant> whereFilters);
    WhereFilter(std::vector<DbVariant> whereFilters);

    struct FilterVariantToString
    {
      std::string operator()(const WhereFilter &value);
      std::string operator()(const std::vector<DbVariant> &value);
    };
};

class FilterVariant : public std::variant<DbVariant, std::vector<DbVariant>, WhereFilter>
{
public:
    using std::variant<DbVariant, std::vector<DbVariant>, WhereFilter>::variant;

    FilterVariant();
    FilterVariant(std::vector<DbVariant> v);

    struct FilterVariantToString
    {
      std::string operator()(const WhereFilter &value);
      std::string operator()(const std::vector<DbVariant> &value);
      std::string operator()(const DbVariant &value);
    };

    std::string toString() const;

    bool empty() const;

    std::vector<DbVariant> toDbVariants() const;
};
}

#endif // FILTERVARIANT_H
