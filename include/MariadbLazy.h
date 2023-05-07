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
  std::string insert_query() const override;
  std::string select_query() const override;
  std::string update_query() const override;
  std::string delete_query() const override;
  std::string batch_insert_query() const override;
  std::string insert_update_query() const override;
  std::string insert_ignore_query() const override;

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
