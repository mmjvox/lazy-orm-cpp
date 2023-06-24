#ifndef DBVARIANT_H
#define DBVARIANT_H

#include <string>
#include <variant>
#include <list>
#include <vector>

namespace LazyOrm {

enum Filters
{
    None = -10,
//    WHERE = 0,
//    AND = 1,
//    OR,
    ORDERBY=1,
    LIMIT,
    HAVING,
    GROUPBY
};

enum NestedWhere
{
    NNone = -10,
    WHERE = 0,
    AND = 1,
    OR,
    NOT,
};

using UnsignedIntegerVariant = std::variant<unsigned short, unsigned int, unsigned long, unsigned long long>;
using SignedIntegerVariant = std::variant<short, int, long, long long>;
using SignedFloatingPointVariant = std::variant<float, double, long double>;

class DbVariant : public std::variant<std::string,UnsignedIntegerVariant,SignedIntegerVariant,SignedFloatingPointVariant,bool>
{
private:
    UnsignedIntegerVariant toUnsignedIntegerVariant() const;
    SignedIntegerVariant toSignedIntegerVariant() const;
    SignedFloatingPointVariant toSignedFloatingPointVariant() const;

public:
    using std::variant<std::string,UnsignedIntegerVariant,SignedIntegerVariant,SignedFloatingPointVariant,bool>::variant;

    std::string toString() const;
    unsigned long long toUInt64() const;
    long long toInt64() const;
    long double toLongDouble() const;
    bool toBool() const;

    std::string toLowerString() const;
    std::string toUpperString() const;

    bool empty();

    std::string setQuote() const;
    std::string setBackTick() const;
    std::string toCleanString() const;
    bool isUpdate() const;
};

typedef std::pair<std::string,LazyOrm::DbVariant> pair;

}

#endif // DBVARIANT_H
