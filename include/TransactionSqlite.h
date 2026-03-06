#ifndef TRANSACTIONSQLITE_H
#define TRANSACTIONSQLITE_H

#include "Transaction.h"


namespace LazyOrm {

class TransactionSqlite : public Transaction
{
public:
    TransactionSqlite();
    TransactionSqlite(const std::initializer_list<LazyOrm> queries);

    // copy contructors:;
    TransactionSqlite(const Transaction& transaction);
    void operator=(const Transaction& transaction);

    // Transaction interface
public:
    std::string queryString() const override;
    std::string queryStringWithoutBegining() const override;
};

}

#endif // TRANSACTIONSQLITE_H
