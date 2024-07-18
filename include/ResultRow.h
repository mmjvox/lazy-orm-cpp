#ifndef RESULTROW_H
#define RESULTROW_H


#include <string>
#include <unordered_map>

namespace LazyOrm {

class DbVariant;

class ResultRow : public std::unordered_map<std::string,DbVariant>
{
public:
    using std::unordered_map<std::string,DbVariant>::unordered_map;

    std::string toString() const;
    std::string toIndentedString() const;
};

}

#endif // RESULTROW_H
