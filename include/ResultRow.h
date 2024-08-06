#ifndef RESULTROW_H
#define RESULTROW_H


#include "DbVariant.h"
#include <string>
#include <unordered_map>

namespace LazyOrm {

class ResultRow : public std::unordered_map<std::string,DbVariant>
{
public:
    using std::unordered_map<std::string,DbVariant>::unordered_map;

    std::string toString() const;
    std::string toIndentedString() const;

    DbVariant value(const std::string key, const DbVariant dbVariant={}) const;
    void insert(const std::string key, const DbVariant dbVariant);
    DbVariant operator[](const std::string key) const;

    DbVariant at(unsigned long long columnIndex) const;
    DbVariant value(unsigned long long columnIndex) const;

};

}

#endif // RESULTROW_H
