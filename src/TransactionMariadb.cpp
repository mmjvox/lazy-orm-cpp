#include "TransactionMariadb.h"

namespace LazyOrm {

TransactionMariadb::TransactionMariadb() {}

TransactionMariadb::TransactionMariadb(const std::initializer_list<LazyOrm> queries)
{
    for(const auto &query : queries)
    {
        append(query);
    }
}

TransactionMariadb::TransactionMariadb(const Transaction &transaction)
{
    *this = transaction;
}

void TransactionMariadb::operator=(const Transaction &transaction)
{
    mQueries = transaction.queries();
}

std::string TransactionMariadb::queryString() const
{
    std::string retStr;
    retStr.append("START TRANSACTION; ");
    for(const auto &query : mQueries)
    {
        retStr.append(static_cast<MariadbLazy>(query).queryString());
        retStr.append(" ");
    }
    retStr.append("COMMIT;");
    return retStr;
}

std::string TransactionMariadb::queryStringWithoutBegining() const
{
    std::string retStr;
    for(const auto &query : mQueries)
    {
        retStr.append(static_cast<MariadbLazy>(query).queryString());
        retStr.append(" ");
    }
    return retStr;
}

}
