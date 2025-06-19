#ifndef DBVARIANT_H
#define DBVARIANT_H

#include <string>
#include <variant>
#include <map>
#include <list>
#include <vector>

namespace LazyOrm {

enum Filters
{
    None = -10,
    ORDERBY=1,
    ORDERBY_DESC,
    ORDERBY_ASC,
    LIMIT,
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

enum class Having
{
    NNone = -10,
    HAVING = 0,
    AND = 1,
    OR,
    NOT,
};

using UnsignedIntegerVariant = std::variant<unsigned short, unsigned int, unsigned long, unsigned long long>;
using SignedIntegerVariant = std::variant<short, int, long, long long>;
using SignedFloatingPointVariant = std::variant<float, double, long double>;

class DbVariant : public std::variant<std::monostate,std::string,UnsignedIntegerVariant,SignedIntegerVariant,SignedFloatingPointVariant,bool>
{
#ifdef DEBUG_MODE
public:
#else
private:
#endif
    UnsignedIntegerVariant toUnsignedIntegerVariant() const;
    bool isUnsignedIntegerVariant() const;
    unsigned long long toUnsignedInteger() const;
    bool can_UInt_Fit_Int(unsigned long long value) const;
    bool can_UInt_Fit_Float(unsigned long long value) const;

    SignedIntegerVariant toSignedIntegerVariant() const;
    bool isSignedIntegerVariant() const;
    long long toSignedInteger() const;
    bool can_Int_Fit_UInt(long long value) const;
    bool can_Int_Fit_Float(long long value) const;

    SignedFloatingPointVariant toSignedFloatingPointVariant() const;
    bool isSignedFloatingPointVariant() const;
    long double toSignedFloatingPoint() const;
    bool can_Float_Fit_Int(long double value) const;
    bool can_Float_Fit_UInt(long double value) const;

    std::string toFixedString(long double value) const;
    std::string toFixedString(const std::string &str) const;

    long long safeStringToLL(std::string value) const;
    unsigned long long safeStringToULL(std::string value) const;
    long double safeStringToLD(std::string value) const;
    bool isScientific(const std::string &value) const;
    bool isScientific(const double &value) const;


public:
    using std::variant<std::monostate,std::string,UnsignedIntegerVariant,SignedIntegerVariant,SignedFloatingPointVariant,bool>::variant;

    DbVariant& set(DbVariant newVariant);

    std::string typeName() const;
    std::string toString() const;
    unsigned long long toULongLong() const;
    long long toLongLong() const;
    long double toLongDouble() const;
    bool toBool() const;

    std::string toLowerString() const;
    std::string toUpperString() const;

    bool empty();
    const size_t lenght() const;
    bool contains(std::string substr) const;
    bool startsWith(std::string prefix, bool caseSensitive=true) const;
    std::string trim() const;

    std::string setQuote() const;
    std::string setBackTick() const;
    std::string toCleanString() const;
    bool isUpdate() const;

    DbVariant alterStringToBestMatchType();
    DbVariant alterStringToNumber();

    const std::map<std::string, LazyOrm::Filters> getFiltersToStringMap() const;
};

typedef std::pair<std::string,LazyOrm::DbVariant> pair;

}

#endif // DBVARIANT_H
