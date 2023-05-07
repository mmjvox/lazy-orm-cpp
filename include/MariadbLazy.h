#ifndef MARIADBLAZY_H
#define MARIADBLAZY_H

#include "MariadbFilteringLazy.h"
#include "AbstractLazy.h"

namespace LazyOrm
{
class MariadbLazy : public AbstractLazy
{
private:
  MariadbFilteringLazy mFilter;

protected:
  void insert_query() override;
  void select_query() override;
  void update_query() override;
  void delete_query() override;
  void batch_insert_query() override;
  void insert_update_query() override;
  void insert_ignore_query() override;

  FilteringAbstractLazy& getCurrentFilters() override
  {
    return mFilter;
  }


public:
  MariadbLazy();
  MariadbLazy(const std::string &table, const Query &queryType);

  void setFilter(const MariadbFilteringLazy filter);

};
}
#endif // MARIADBLAZY_H
