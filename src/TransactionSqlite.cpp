#include "TransactionSqlite.h"

namespace LazyOrm {

TransactionSqlite::TransactionSqlite() {}

TransactionSqlite::TransactionSqlite(const std::initializer_list<LazyOrm> queries)
{
    for(const auto &query : queries)
    {
        append(query);
    }
}

TransactionSqlite::TransactionSqlite(const Transaction &transaction)
{
    *this = transaction;
}

void TransactionSqlite::operator=(const Transaction &transaction)
{
    mQueries = transaction.queries();
}

std::string TransactionSqlite::queryString() const
{
    std::string retStr;
    retStr.append("BEGIN TRANSACTION; ");
    for(const auto &query : mQueries)
    {
        retStr.append(static_cast<SqliteLazy>(query).queryString());
        retStr.append(" ");
    }
    retStr.append("COMMIT;");
    return retStr;
}

std::string TransactionSqlite::queryStringWithoutBegining() const
{
    std::string retStr;
    for(const auto &query : mQueries)
    {
        retStr.append(static_cast<SqliteLazy>(query).queryString());
        retStr.append(" ");
    }
    return retStr;
}

}
