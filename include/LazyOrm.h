#ifndef LAZYORM_H
#define LAZYORM_H

#include "FilteringLazy.h"
#include "MariadbLazy.h"
#include "PostgreLazy.h"
#include "SqliteLazy.h"



namespace LazyOrm {

class LazyOrm : public AbstractLazy //: public MariadbLazy, public PostgreLazy, public SqliteLazy
{
private:
    FilteringLazy mFilter;

    FilteringAbstractLazy& getCurrentFilters() override
    {
        return mFilter;
    }

public:
    enum DBMS_TYPE
    {
        Postgres=0,
        MariaDB,
        Sqlite3
    };

    LazyOrm();

    std::string queryString(const DBMS_TYPE &dbms) const;


    // copy contructors:;
    LazyOrm(const AbstractLazy& abstractLaz);
    void operator=(const AbstractLazy& abstractLaz);

    const FilteringAbstractLazy& getFilter() const override
    {
        return mFilter;
    }

    // AbstractLazy interface
protected:
    std::string insert_query() const override;
    std::string select_query() const override;
    std::string update_query() const override;
    std::string delete_query() const override;
    std::string insert_update_query() const override;
    std::string bulk_insert_query() const override;
    std::string insert_ignore_query() const override;
    std::string bulk_update_query() const override;
};

}


#endif // LAZYORM_H
