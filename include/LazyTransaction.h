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

    // Transaction interface
public:
    std::string queryString(const DBMS_TYPE &dbms) const;
    std::string queryStringWithoutBegining(const DBMS_TYPE &dbms) const;
};

}

#endif // LAZYTRANSACTION_H
