#ifndef MARIADBFILTERINGLAZY_H
#define MARIADBFILTERINGLAZY_H

#include "FilteringAbstractLazy.h"

namespace LazyOrm {
class MariadbFilteringLazy : virtual public FilteringAbstractLazy
{
protected:

    void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;

public:
    MariadbFilteringLazy();

    // copy contructors
    MariadbFilteringLazy(const FilteringAbstractLazy& abstractLaz);
    void operator=(const FilteringAbstractLazy& abstractLaz);

    std::string orderbyString() const override;
    std::string limitString() const override;
    std::string groupString() const override;

};
}

#endif // MARIADBFILTERINGLAZY_H
