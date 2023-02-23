#ifndef MARIADBFILTERINGLAZY_H
#define MARIADBFILTERINGLAZY_H

#include "FilteringAbstractLazy.h"

namespace LazyOrm {
class MariadbFilteringLazy : public FilteringAbstractLazy
{
protected:
    std::string where_conditions() override;
public:
    MariadbFilteringLazy();
};
}

#endif // MARIADBFILTERINGLAZY_H
