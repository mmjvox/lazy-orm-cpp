#ifndef FILTERVARIANT_H
#define FILTERVARIANT_H

#include "DbVariant.h"


namespace LazyOrm {

class FilterVariant : public std::variant<DbVariant, std::vector<DbVariant>>
{
public:
    using std::variant<DbVariant, std::vector<DbVariant>>::variant;

    FilterVariant();
    FilterVariant(std::vector<DbVariant> v);

    bool empty() const;

    std::vector<DbVariant> toDbVariants() const;
};
}

#endif // FILTERVARIANT_H
