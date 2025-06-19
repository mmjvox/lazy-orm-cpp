#ifndef SQLITEFILTERINGLAZY_H
#define SQLITEFILTERINGLAZY_H

#include "FilteringAbstractLazy.h"

namespace LazyOrm {
class SqliteFilteringLazy : virtual public FilteringAbstractLazy
{
protected:

    void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;

public:
    SqliteFilteringLazy();

    // copy contructors:;
    SqliteFilteringLazy(const FilteringAbstractLazy& abstractLaz);
    void operator=(const FilteringAbstractLazy& abstractLaz);

    std::string orderbyString() const override;
    std::string limitString() const override;
    std::string groupString() const override;

};
}

#endif // SQLITEFILTERINGLAZY_H
