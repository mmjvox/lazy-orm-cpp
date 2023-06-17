#ifndef POSTGREFILTERINGLAZY_H
#define POSTGREFILTERINGLAZY_H

#include "FilteringAbstractLazy.h"

namespace LazyOrm {
class PostgreFilteringLazy : public FilteringAbstractLazy
{
protected:

    void setWhereConditions(const Filters &filter, const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setHavingConditions(const std::vector<LazyOrm::FilterVariant> &filtersList) override;
    void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    //
    void appendWhere(std::string &retStr) const override;
    void appendOrderby(std::string &retStr) const override;
    void appendLimit(std::string &retStr) const override;
    void appendGroup(std::string &retStr) const override;
    void appendHaving(std::string &retStr) const override;
    //
    void nestedWhereToString(WhereFilter whereItem, std::string &retStr, Filters whereFilter, bool firstItem=false) const override;

public:
    PostgreFilteringLazy();
    //
    std::string where_conditions() const override;
};
}

#endif // POSTGREFILTERINGLAZY_H
