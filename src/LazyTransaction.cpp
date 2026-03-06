#include "LazyTransaction.h"
#include "TransactionMariadb.h"
#include "TransactionPostgre.h"
#include "TransactionSqlite.h"

namespace LazyOrm {

LazyTransaction::LazyTransaction() {}

LazyTransaction::LazyTransaction(const std::initializer_list<LazyOrm> queries)
{
    for(const auto &query : queries)
    {
        append(query);
    }
}

std::string LazyTransaction::queryString(const DBMS_TYPE &dbms) const
{
    switch (dbms) {
    case MariaDB:
        return static_cast<TransactionMariadb>(*this).queryString();
    case Postgres:
        return static_cast<TransactionPostgre>(*this).queryString();
    case Sqlite3:
        return static_cast<TransactionSqlite>(*this).queryString();
    }
}

std::string LazyTransaction::queryStringWithoutBegining(const DBMS_TYPE &dbms) const
{
    switch (dbms) {
    case MariaDB:
        return static_cast<TransactionMariadb>(*this).queryStringWithoutBegining();
    case Postgres:
        return static_cast<TransactionPostgre>(*this).queryStringWithoutBegining();
    case Sqlite3:
        return static_cast<TransactionSqlite>(*this).queryStringWithoutBegining();
    }
}

std::string LazyTransaction::query_with_trim_consecutive_spaces(const DBMS_TYPE &dbms) const
{
    switch (dbms) {
    case MariaDB:
        return static_cast<TransactionMariadb>(*this).query_with_trim_consecutive_spaces();
    case Postgres:
        return static_cast<TransactionPostgre>(*this).query_with_trim_consecutive_spaces();
    case Sqlite3:
        return static_cast<TransactionSqlite>(*this).query_with_trim_consecutive_spaces();
    }
}

std::string LazyTransaction::queryString() const
{
    return {};
}

std::string LazyTransaction::queryStringWithoutBegining() const
{
    return {};
}

}
