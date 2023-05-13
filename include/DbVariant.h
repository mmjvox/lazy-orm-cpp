#ifndef DBVARIANT_H
#define DBVARIANT_H

#include <string>
#include <variant>
#include <list>
#include <vector>

namespace LazyOrm {

class DbVariant : public std::variant<std::string,uint64_t,int,double,float,bool>
{
private:
    struct DbVariantToString
    {
      std::string operator()(const std::string &value);
      //numbers
      std::string operator()(const uint64_t &value);
      std::string operator()(const int &value);
      std::string operator()(const double &value);
      std::string operator()(const float &value);
      // boolean
      std::string operator()(const bool &value);
    };

public:
    using std::variant<std::string,uint64_t,int,double,float,bool>::variant;

    std::string toString() const;

    std::string toLowerString() const;

    bool empty();
};

typedef std::pair<std::string,LazyOrm::DbVariant> pair;

}

#endif // DBVARIANT_H
