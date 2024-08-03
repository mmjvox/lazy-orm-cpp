#include "LazyOrm.h"

namespace LazyOrm {

LazyOrm::LazyOrm()
{
}

LazyOrm::LazyOrm(const AbstractLazy &mariadbLazy)
{
    *this = mariadbLazy;
}

void LazyOrm::operator=(const AbstractLazy &abstractLaz)
{
    mQueryType = abstractLaz.queryType();
    mTabeName = abstractLaz.tabeName();
    mProperties = abstractLaz.properties();
    mBatchProperties = abstractLaz.batchProperties();
    mWhereFilter = abstractLaz.whereFilter();
    mFilter = abstractLaz.getFilter();
    mPrimaryKey = abstractLaz.primaryKey();
}

std::string LazyOrm::insert_query() const {return {};}
std::string LazyOrm::select_query() const {return {};}
std::string LazyOrm::update_query() const {return {};}
std::string LazyOrm::delete_query() const {return {};}
std::string LazyOrm::insert_update_query() const {return {};}
std::string LazyOrm::bulk_insert_query() const {return {};}
std::string LazyOrm::insert_ignore_query() const {return {};}
std::string LazyOrm::bulk_update_query() const {return {};}


std::string LazyOrm::queryString(const DBMS_TYPE &dbms) const
{
    switch (dbms) {
    case MariaDB:
        return MariadbLazy(*this).queryString();
    case Postgres:
        return PostgreLazy(*this).queryString();
    case Sqlite3:
        return SqliteLazy(*this).queryString();
    }
}

}
