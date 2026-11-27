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
    copyStateFrom(abstractLaz);
    mFilter = abstractLaz.getFilter();
}

void LazyOrm::operator=(const LazyOrm &other)
{
    *this = static_cast<const AbstractLazy &>(other);
}

std::string LazyOrm::insert_query() const {return {};}
std::string LazyOrm::select_query() const {return {};}
std::string LazyOrm::update_query() const {return {};}
std::string LazyOrm::delete_query() const {return {};}
std::string LazyOrm::insert_update_query() const {return {};}
std::string LazyOrm::bulk_insert_query() const {return {};}
std::string LazyOrm::insert_ignore_query() const {return {};}
std::string LazyOrm::bulk_update_query() const {return {};}
void LazyOrm::appendFilter(const Filters &filter, DbVariant dbVariant)
{
    mFilter.appendFilter(filter, dbVariant);
}


ParameterizedQuery LazyOrm::buildParameterized(const DBMS_TYPE &dbms) const
{
    switch (dbms) {
    case MariaDB:
        return static_cast<MariadbLazy>(*this).buildParameterized();
    case Postgres:
        return static_cast<PostgreLazy>(*this).buildParameterized();
    case Sqlite3:
        return static_cast<SqliteLazy>(*this).buildParameterized();
    }
    return {};
}

std::string LazyOrm::queryString(const DBMS_TYPE &dbms) const
{
    switch (dbms) {
    case MariaDB:
        return static_cast<MariadbLazy>(*this).queryString();
    case Postgres:
        return static_cast<PostgreLazy>(*this).queryString();
    case Sqlite3:
        return static_cast<SqliteLazy>(*this).queryString();
    }
    return {};
}

}
