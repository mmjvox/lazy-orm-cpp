#ifndef PARAMETERIZEDQUERY_H
#define PARAMETERIZEDQUERY_H

#include "DbVariant.h"

#include <string>
#include <vector>

namespace LazyOrm {

struct ParameterizedQuery
{
    std::string sql;
    std::vector<DbVariant> params;
};

}

#endif // PARAMETERIZEDQUERY_H
