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
  std::string insert_query() const override;
  std::string select_query() const override;
  std::string update_query() const override;
  std::string delete_query() const override;
  std::string bulk_insert_query() const override;
  std::string insert_update_query() const override;
  std::string insert_ignore_query() const override;
  std::string bulk_update_query() const override;

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
