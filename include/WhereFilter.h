#ifndef WHEREFILTER_H
#define WHEREFILTER_H

#include "DbVariant.h"

namespace LazyOrm {

class WhereFilter{

private:
    std::vector<std::vector<DbVariant>> mNestedDbVariant;
    std::vector<WhereFilter> mNestedWhereFilters;
    bool isWhereFilter() const;
    bool isVariantVaector() const;
    std::string toString(WhereFilter wf) const;
    std::string string_join(const std::string &delimiter, const std::vector<DbVariant> &container) const;


public:
    WhereFilter();
    ~WhereFilter();

    WhereFilter(std::string str);
    WhereFilter(DbVariant variant);
    WhereFilter(std::initializer_list<WhereFilter> wfs);
    WhereFilter(std::initializer_list<std::variant<std::vector<DbVariant>,DbVariant>> variants);
    WhereFilter(std::vector<DbVariant> variants);
    WhereFilter(std::vector<WhereFilter> wfs);

    void operator=(const WhereFilter &wf);

    bool empty() const;

    std::string toString() const;

    void append(std::vector<DbVariant> variants);
};

}

#endif // WHEREFILTER_H
