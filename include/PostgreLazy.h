#ifndef POSTGRELAZY_H
#define POSTGRELAZY_H

#include "PostgreFilteringLazy.h"
#include "AbstractLazy.h"

namespace LazyOrm
{
class PostgreLazy : public AbstractLazy
{
private:
  PostgreFilteringLazy mFilter;

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
  PostgreLazy();
  PostgreLazy(const std::string &table, const Query &queryType);

  void setFilter(const PostgreFilteringLazy filter);

};
}
#endif // POSTGRELAZY_H
