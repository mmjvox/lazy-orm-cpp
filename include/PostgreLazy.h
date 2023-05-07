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
  PostgreLazy();
  PostgreLazy(const std::string &table, const Query &queryType);

  void setFilter(const PostgreFilteringLazy filter);

};
}
#endif // POSTGRELAZY_H
