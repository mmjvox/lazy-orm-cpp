#include "LazyOrm.h"

namespace LazyOrm {

LazyOrm::LazyOrm()
{

}

std::string LazyOrm::queryString(const DBMS_TYPE &dbms)
{
    switch (dbms) {
    case MariaDB:
        return static_cast<MariadbLazy*>(this)->queryString();
    case Postgre:
        return static_cast<PostgreLazy*>(this)->queryString();
    case Sqlite3:
        return static_cast<SqliteLazy*>(this)->queryString();
    }
}

}
