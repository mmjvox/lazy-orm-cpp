#ifndef LAZYORM_H
#define LAZYORM_H

#include "MariadbLazy.h"
#include "PostgreLazy.h"
#include "SqliteLazy.h"



namespace LazyOrm {

class LazyOrm : public MariadbLazy, public PostgreLazy, public SqliteLazy
{
public:
    enum DBMS_TYPE
    {
        MariaDB =1,
        Postgre,
        Sqlite
    };

    LazyOrm();

    std::string queryString(const DBMS_TYPE &dbms);
};

}


#endif // LAZYORM_H
