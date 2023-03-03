#ifndef MARIADBFILTERINGLAZY_H
#define MARIADBFILTERINGLAZY_H

#include "FilteringAbstractLazy.h"

namespace LazyOrm {
class MariadbFilteringLazy : public FilteringAbstractLazy
{
protected:

    void setWhereConditions(const Filters &filter, const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    //
    void appendWhere(std::string &retStr) override;
    void appendOrderby(std::string &retStr) override;
    void appendLimit(std::string &retStr) override;
    void appendGroup(std::string &retStr) override;
    //
    std::string where_conditions() override;

public:
    MariadbFilteringLazy();
};
}

#endif // MARIADBFILTERINGLAZY_H