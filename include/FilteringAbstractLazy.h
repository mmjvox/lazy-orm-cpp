#ifndef FILTERINGABSTRACTLAZY_H
#define FILTERINGABSTRACTLAZY_H

#include <string>
#include <variant>
#include <list>
#include <vector>

namespace LazyOrm {

typedef std::variant<
    std::string,
    std::list<std::string>
> filterTypes;

enum Filters
{
  AND      = 0,
  OR    = 2,
};

class FilteringAbstractLazy
{
protected:
    std::string filterStr(Filters f);
    std::vector<std::pair<Filters,std::vector<std::string>>> mConditions;

    virtual std::string where_conditions() = 0;

public:
    FilteringAbstractLazy();
    void setFilter(const Filters &filter, std::initializer_list<std::string> f);
//    filterTypes & operator[](const Filters &filter);

    std::string testString();
};
}
#endif // FILTERINGABSTRACTLAZY_H
