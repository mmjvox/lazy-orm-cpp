#include "Transaction.h"

#include "MariadbLazy.h"
#include "PostgreLazy.h"
#include "SqliteLazy.h"

namespace LazyOrm {

template<class DBMStype>
std::string Transaction<DBMStype>::forMariadb(bool beginTrans) const
{
    std::string retStr;
    if(beginTrans)
    {
        retStr.append("START TRANSACTION; ");
    }
    for(const auto &query : mQueries)
    {
        retStr.append(query.queryString());
        retStr.append(" ");
    }
    if(beginTrans)
    {
        retStr.append("COMMIT;");
    }
    return retStr;
}

template<class DBMStype>
std::string Transaction<DBMStype>::forPostgre(bool beginTrans) const
{
    std::string retStr;
    if(beginTrans)
    {
        retStr.append("BEGIN TRANSACTION; ");
    }
    for(const auto &query : mQueries)
    {
        retStr.append(query.queryString());
        retStr.append(" ");
    }
    if(beginTrans)
    {
        retStr.append("COMMIT TRANSACTION;");
    }
    return retStr;
}

template<class DBMStype>
std::string Transaction<DBMStype>::forSqlite(bool beginTrans) const
{
    std::string retStr="BEGIN TRANSACTION; ";
    if(beginTrans)
    {
        retStr.append("BEGIN TRANSACTION; ");
    }
    for(const auto &query : mQueries)
    {
        retStr.append(query.queryString());
        retStr.append(" ");
    }
    if(beginTrans)
    {
        retStr.append("COMMIT;");
    }
    return retStr;
}

template<class DBMStype>
Transaction<DBMStype>::Transaction(){}

template<class DBMStype>
Transaction<DBMStype>::Transaction(std::initializer_list<DBMStype> queries)
{
    for(const auto &query : queries)
    {
        mQueries.push_back(query);
    }
}

template<class DBMStype>
void Transaction<DBMStype>::append(DBMStype query)
{
    mQueries.push_back(query);
}

template<class DBMStype>
std::string Transaction<DBMStype>::queryString() const
{
    return queryString(false);
}

template<class DBMStype>
std::string Transaction<DBMStype>::queryString(bool beginTrans) const
{
    if(mQueries.size()<1)
    {
        return {};
    }
    auto firstQuery = mQueries.front();
    if constexpr(std::is_same_v<decltype(firstQuery) , MariadbLazy>)
    {
        return forMariadb(beginTrans);
    }
    if constexpr(std::is_same_v<decltype(firstQuery) , PostgreLazy>)
    {
        return forPostgre(beginTrans);
    }
    if constexpr(std::is_same_v<decltype(firstQuery) , SqliteLazy>)
    {
        return forSqlite(beginTrans);
    }
    return {};
}

template class Transaction<MariadbLazy>;
template class Transaction<PostgreLazy>;
template class Transaction<SqliteLazy>;
}
