#ifndef POSTGREFILTERINGLAZY_H
#define POSTGREFILTERINGLAZY_H

#include "FilteringAbstractLazy.h"

namespace LazyOrm {
class PostgreFilteringLazy : virtual public FilteringAbstractLazy
{
protected:

    void setLimitConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setOrderConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;
    void setGroupConditions(const std::initializer_list<LazyOrm::FilterVariant> &filtersList) override;

public:
    PostgreFilteringLazy();

    // copy contructors:;
    PostgreFilteringLazy(const FilteringAbstractLazy& abstractLaz);
    void operator=(const FilteringAbstractLazy& abstractLaz);

    std::string orderbyString() const override;
    std::string limitString() const override;
    std::string groupString() const override;

};
}

#endif // POSTGREFILTERINGLAZY_H
