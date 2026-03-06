#ifndef LAZYTRANSACTION_H
#define LAZYTRANSACTION_H

#include "Transaction.h"


namespace LazyOrm {

class LazyTransaction : public Transaction
{
public:

    enum DBMS_TYPE
    {
        Postgres=0,
        MariaDB,
        Sqlite3
    };

    LazyTransaction();
    LazyTransaction(const std::initializer_list<LazyOrm> queries);

    std::string queryString(const DBMS_TYPE &dbms) const;
    std::string queryStringWithoutBegining(const DBMS_TYPE &dbms) const;
    std::string query_with_trim_consecutive_spaces(const DBMS_TYPE &dbms) const;

    // Transaction interface
public:
    std::string queryString() const;
    std::string queryStringWithoutBegining() const;
};

}

#endif // LAZYTRANSACTION_H
