#ifndef HAVINGFILTER_H
#define HAVINGFILTER_H

#include "WhereFilter.h"

namespace LazyOrm {

class HavingFilter : public WhereFilter
{
private:
    std::string leadingString() const override {
        return " HAVING ";
    }

public:
    using WhereFilter::WhereFilter;
};

}

#endif // HAVINGFILTER_H
