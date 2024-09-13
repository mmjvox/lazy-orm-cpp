#include "Transaction.h"

#include "MariadbLazy.h"
#include "PostgreLazy.h"
#include "SqliteLazy.h"
#include <algorithm>

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
std::string Transaction<DBMStype>::queryStringWithoutBegining() const
{
    if(mQueries.size()<1)
    {
        return {};
    }
    auto firstQuery = mQueries.front();
    if constexpr(std::is_same_v<decltype(firstQuery) , MariadbLazy>)
    {
        return forMariadb(false);
    }
    if constexpr(std::is_same_v<decltype(firstQuery) , PostgreLazy>)
    {
        return forPostgre(false);
    }
    if constexpr(std::is_same_v<decltype(firstQuery) , SqliteLazy>)
    {
        return forSqlite(false);
    }
    return {};
}

template<class DBMStype>
std::string Transaction<DBMStype>::queryString() const
{
    if(mQueries.size()<1)
    {
        return {};
    }
    auto firstQuery = mQueries.front();
    if constexpr(std::is_same_v<decltype(firstQuery) , MariadbLazy>)
    {
        return forMariadb(true);
    }
    if constexpr(std::is_same_v<decltype(firstQuery) , PostgreLazy>)
    {
        return forPostgre(true);
    }
    if constexpr(std::is_same_v<decltype(firstQuery) , SqliteLazy>)
    {
        return forSqlite(true);
    }
    return {};
}

template<class DBMStype>
std::string Transaction<DBMStype>::query_with_trim_consecutive_spaces()
{
    std::string str = queryString();
    auto new_end = std::unique(str.begin(), str.end(), [](char a, char b) {
        return a == ' ' && b == ' ';
    });
    str.erase(new_end, str.end());
    return str;
}


template class Transaction<MariadbLazy>;
template class Transaction<PostgreLazy>;
template class Transaction<SqliteLazy>;
}
