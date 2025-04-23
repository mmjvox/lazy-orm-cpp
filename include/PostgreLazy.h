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
  PostgreLazy();
  PostgreLazy(const std::string &table, const Query &queryType);

  void setFilter(const PostgreFilteringLazy filter);

  // copy contructors:;
  PostgreLazy(const AbstractLazy& abstractLaz);
  void operator=(const AbstractLazy& abstractLaz);

  const FilteringAbstractLazy& getFilter() const override
  {
      return mFilter;
  }

  void appendFilter(const Filters &filter, LazyOrm::DbVariant dbVariant) override;

};
}
#endif // POSTGRELAZY_H
