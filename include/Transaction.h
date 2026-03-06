#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "LazyOrm.h"
#include <string>
#include <vector>

namespace LazyOrm {

class Transaction
{
protected:
    std::vector<LazyOrm> mQueries;

public:
    Transaction();
    Transaction(const std::initializer_list<LazyOrm> queries);
    void append(const LazyOrm query);
    virtual std::string queryString() const = 0 ;
    virtual std::string queryStringWithoutBegining() const = 0;
    std::string query_with_trim_consecutive_spaces();
    std::vector<LazyOrm> queries() const;
};

}

#endif // TRANSACTION_H
