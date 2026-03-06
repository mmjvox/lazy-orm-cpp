#ifndef TRANSACTIONPOSTGRE_H
#define TRANSACTIONPOSTGRE_H

#include "Transaction.h"

namespace LazyOrm {

class TransactionPostgre : public Transaction
{
public:
    TransactionPostgre();
    TransactionPostgre(const std::initializer_list<LazyOrm> queries);

    // copy contructors:;
    TransactionPostgre(const Transaction& transaction);
    void operator=(const Transaction& transaction);

    // Transaction interface
public:
    std::string queryString() const override;
    std::string queryStringWithoutBegining() const override;
};

}

#endif // TRANSACTIONPOSTGRE_H
