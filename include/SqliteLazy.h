#ifndef SQLITELAZY_H
#define SQLITELAZY_H

#include "SqliteFilteringLazy.h"
#include "AbstractLazy.h"

namespace LazyOrm
{
class SqliteLazy : public AbstractLazy
{
private:
  SqliteFilteringLazy mFilter;

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
  SqliteLazy();
  SqliteLazy(const std::string &table, const Query &queryType);

  void setFilter(const SqliteFilteringLazy filter);

};
}
#endif // SQLITELAZY_H
