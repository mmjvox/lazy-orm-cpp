#ifndef FILTERINGLAZY_H
#define FILTERINGLAZY_H

#include "MariadbFilteringLazy.h"
#include "PostgreFilteringLazy.h"
#include "SqliteFilteringLazy.h"

namespace LazyOrm {

class FilteringLazy : public MariadbFilteringLazy, public PostgreFilteringLazy, public SqliteFilteringLazy
{
protected:
    void setLimitConditions(const std::initializer_list<FilterVariant> &filtersList) override;
    void setOrderConditions(const std::initializer_list<FilterVariant> &filtersList) override;
    void setGroupConditions(const std::initializer_list<FilterVariant> &filtersList) override;

public:
    FilteringLazy();

    // copy contructors:;
    FilteringLazy(const FilteringAbstractLazy& abstractLaz);
    void operator=(const FilteringAbstractLazy& abstractLaz);

    std::string orderbyString() const override;
    std::string limitString() const override;
    std::string groupString() const override;

};

}


#endif // FILTERINGLAZY_H
