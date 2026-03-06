#include "TransactionPostgre.h"

namespace LazyOrm {

TransactionPostgre::TransactionPostgre() {}

TransactionPostgre::TransactionPostgre(const std::initializer_list<LazyOrm> queries)
{
    for(const auto &query : queries)
    {
        append(query);
    }
}

TransactionPostgre::TransactionPostgre(const Transaction &transaction)
{
    *this = transaction;
}

void TransactionPostgre::operator=(const Transaction &transaction)
{
    mQueries = transaction.queries();
}

std::string TransactionPostgre::queryString() const
{
    std::string retStr;
    retStr.append("BEGIN TRANSACTION; ");
    for(const auto &query : mQueries)
    {
        retStr.append(static_cast<PostgreLazy>(query).queryString());
        retStr.append(" ");
    }
    retStr.append("COMMIT TRANSACTION;");
    return retStr;
}

std::string TransactionPostgre::queryStringWithoutBegining() const
{
    std::string retStr;
    for(const auto &query : mQueries)
    {
        retStr.append(static_cast<PostgreLazy>(query).queryString());
        retStr.append(" ");
    }
    return retStr;
}

}
