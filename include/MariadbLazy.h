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
  void insert_query() const override;
  void select_query() const override;
  void update_query() const override;
  void delete_query() const override;
  void batch_insert_query() const override;
  void insert_update_query() const override;
  void insert_ignore_query() const override;

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
