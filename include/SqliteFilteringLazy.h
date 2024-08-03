#ifndef SQLITEFILTERINGLAZY_H
#define SQLITEFILTERINGLAZY_H

#include "FilteringAbstractLazy.h"

namespace LazyOrm {
class SqliteFilteringLazy : virtual public FilteringAbstractLazy
{
protected:

    void setHavingConditions(const std::vector<LazyOrm::FilterVariant> &filtersList) override;
    void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    //
    void appendOrderby(std::string &retStr) const override;
    void appendLimit(std::string &retStr) const override;
    void appendGroup(std::string &retStr) const override;
    void appendHaving(std::string &retStr) const override;
    //

public:
    SqliteFilteringLazy();
    //
    std::string filter_conditions() const override;

    // copy contructors:;
    SqliteFilteringLazy(const FilteringAbstractLazy& abstractLaz);
    void operator=(const FilteringAbstractLazy& abstractLaz);

};
}

#endif // SQLITEFILTERINGLAZY_H
