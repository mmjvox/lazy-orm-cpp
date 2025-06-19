#ifndef MARIADBFILTERINGLAZY_H
#define MARIADBFILTERINGLAZY_H

#include "FilteringAbstractLazy.h"

namespace LazyOrm {
class MariadbFilteringLazy : virtual public FilteringAbstractLazy
{
protected:

    void setHavingConditions(const std::vector<LazyOrm::FilterVariant> &filtersList) override;
    void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    //
    // TODO: support ASC and DESC in order
    void appendOrderby(std::string &retStr) const override;
    void appendLimit(std::string &retStr) const override;
    void appendGroup(std::string &retStr) const override;
    void appendHaving(std::string &retStr) const override;

public:
    MariadbFilteringLazy();
    //
    std::string filter_conditions() const override;

    // copy contructors
    MariadbFilteringLazy(const FilteringAbstractLazy& abstractLaz);
    void operator=(const FilteringAbstractLazy& abstractLaz);

};
}

#endif // MARIADBFILTERINGLAZY_H
