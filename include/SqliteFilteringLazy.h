#ifndef SQLITEFILTERINGLAZY_H
#define SQLITEFILTERINGLAZY_H

#include "FilteringAbstractLazy.h"

namespace LazyOrm {
class SqliteFilteringLazy : public FilteringAbstractLazy
{
protected:

    void setWhereConditions(const Filters &filter, const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setHavingConditions(const std::vector<LazyOrm::FilterVariant> &filtersList) override;
    void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    //
    void appendWhere(std::string &retStr) override;
    void appendOrderby(std::string &retStr) override;
    void appendLimit(std::string &retStr) override;
    void appendGroup(std::string &retStr) override;
    void appendHaving(std::string &retStr) override;
    //
    void nestedWhereToString(WhereType<WhereFilter> whereItem, std::string &retStr, Filters whereFilter, bool firstItem=false) override;

public:
    SqliteFilteringLazy();
    //
    std::string where_conditions() override;
};
}

#endif // SQLITEFILTERINGLAZY_H
