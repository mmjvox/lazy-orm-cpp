#ifndef WHEREFILTER_H
#define WHEREFILTER_H

#include "DbVariant.h"

namespace LazyOrm {

class WhereFilter /*: public std::vector<WhereFilter>*/ {

private:
    std::vector<std::vector<DbVariant>> mNestedDbVariant;
    std::vector<WhereFilter> nestedClasses_;


public:
//    using std::vector<WhereFilter>::vector;
    int filter=NNone;
    ~WhereFilter();

    WhereFilter(std::initializer_list<WhereFilter> input);

    WhereFilter();
    WhereFilter(Filters filter);


    WhereFilter(std::initializer_list<std::variant<std::vector<DbVariant>,DbVariant>> whereFilters);
    WhereFilter(std::vector<DbVariant> whereFilters);
    WhereFilter(std::vector<WhereFilter> whereFilters);

//    WhereFilter( Filters filter, std::vector<DbVariant> whereFilters);
//    WhereFilter( Filters filter, std::vector<WhereFilter> whereFilters);
//    WhereFilter( Filters filter, std::vector<std::vector<DbVariant>> whereFilters);
    //WhereFilter( Filters filter, std::vector<std::vector<WhereFilter>> whereFilters);

    bool emptyData() const
    {
        return mNestedDbVariant.empty();
    }

    void printNestedVectors() const;
    void printNestedVectors(WhereFilter wf) const;
};

}

#endif // WHEREFILTER_H
