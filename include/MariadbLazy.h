#ifndef MARIADBLAZY_H
#define MARIADBLAZY_H

#include "AbstractLazy.h"

namespace LazyOrm
{
class MariadbLazy : public AbstractLazy
{
protected:
  void insert_query() override;
  void select_query() override;

public:
  MariadbLazy();
  MariadbLazy(const std::string &table, const Query &queryType);
};
}
#endif // MARIADBLAZY_H
