#ifndef WHEREFILTERPOSTGRE_H
#define WHEREFILTERPOSTGRE_H

#include "WhereFilter.h"

namespace LazyOrm {

class WhereFilterPostgre : public WhereFilter
{
private:
    std::string string_join(const std::string &delimiter, const std::vector<DbVariant> &container) const override;

public:
    WhereFilterPostgre();
    WhereFilterPostgre(const WhereFilter &wf);
};

}

#endif // WHEREFILTERPOSTGRE_H
