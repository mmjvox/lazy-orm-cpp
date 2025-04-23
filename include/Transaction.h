#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "AbstractLazy.h"
#include <string>
#include <vector>
#include <type_traits>

namespace LazyOrm {

template <class DBMStype>
class Transaction : public AbstractLazy
{
private:
    std::vector<DBMStype> mQueries;

    std::string forMariadb(bool beginTrans) const;
    std::string forPostgre(bool beginTrans) const;
    std::string forSqlite(bool beginTrans) const;

private:
    std::string insert_query() const override{return{};}
    std::string select_query() const override{return{};}
    std::string update_query() const override{return{};}
    std::string delete_query() const override{return{};}
    std::string insert_update_query() const override{return{};}
    std::string bulk_insert_query() const override{return{};}
    std::string insert_ignore_query() const override{return{};}
    std::string bulk_update_query() const override{return{};}
    void appendFilter(const Filters &filter, DbVariant dbVariant) override {}

    FilteringAbstractLazy& getCurrentFilters() override
    {
        throw std::logic_error("Cannot get FilteringAbstractLazy object");
    }

public:
    Transaction();
    Transaction(std::initializer_list<DBMStype> queries);
    void append(DBMStype query);
    std::string queryString() const override;
    std::string queryStringWithoutBegining() const;

    const FilteringAbstractLazy& getFilter() const override{
        throw std::logic_error("Cannot get FilteringAbstractLazy object");
    }


    virtual std::string query_with_trim_consecutive_spaces();
};

}

#endif // TRANSACTION_H
