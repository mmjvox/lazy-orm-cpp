#ifndef FILTERINGLAZY_H
#define FILTERINGLAZY_H

#include "MariadbFilteringLazy.h"
#include "PostgreFilteringLazy.h"
#include "SqliteFilteringLazy.h"

namespace LazyOrm {

class FilteringLazy : public MariadbFilteringLazy, public PostgreFilteringLazy, public SqliteFilteringLazy
{
protected:
    void setHavingConditions(const std::vector<FilterVariant> &filtersList) override;
    void setLimitConditions(const std::initializer_list<FilterVariant> &filtersList) override;
    void setOrderConditions(const std::initializer_list<FilterVariant> &filtersList) override;
    void setGroupConditions(const std::initializer_list<FilterVariant> &filtersList) override;
    void appendOrderby(std::string &retStr) const override;
    void appendLimit(std::string &retStr) const override;
    void appendGroup(std::string &retStr) const override;
    void appendHaving(std::string &retStr) const override;
    std::string filter_conditions() const override;

public:
    FilteringLazy();

    // copy contructors:;
    FilteringLazy(const FilteringAbstractLazy& abstractLaz);
    void operator=(const FilteringAbstractLazy& abstractLaz);
};

}


#endif // FILTERINGLAZY_H
