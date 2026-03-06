#include "Transaction.h"

#include <algorithm>

namespace LazyOrm {

Transaction::Transaction(){}

Transaction::Transaction(const std::initializer_list<LazyOrm> queries)
{
    for(const auto &query : queries)
    {
        append(query);
    }
}

void Transaction::append(const LazyOrm query)
{
    mQueries.push_back(query);
}

std::string Transaction::query_with_trim_consecutive_spaces()
{
    std::string str = queryString();
    auto new_end = std::unique(str.begin(), str.end(), [](char a, char b) {
        return a == ' ' && b == ' ';
    });
    str.erase(new_end, str.end());
    return str;
}

std::vector<LazyOrm> Transaction::queries() const
{
    return mQueries;
}

}
