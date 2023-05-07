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
  SqliteLazy();
  SqliteLazy(const std::string &table, const Query &queryType);

  void setFilter(const SqliteFilteringLazy filter);

};
}
#endif // SQLITELAZY_H
