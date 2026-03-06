#ifndef TRANSACTIONMARIADB_H
#define TRANSACTIONMARIADB_H

#include "Transaction.h"

namespace LazyOrm {

class TransactionMariadb : public Transaction
{
public:
    TransactionMariadb();
    TransactionMariadb(const std::initializer_list<LazyOrm> queries);

    // copy contructors:;
    TransactionMariadb(const Transaction& transaction);
    void operator=(const Transaction& transaction);

    // Transaction interface
public:
    std::string queryString() const override;
    std::string queryStringWithoutBegining() const override;
};

}

#endif // TRANSACTIONMARIADB_H
