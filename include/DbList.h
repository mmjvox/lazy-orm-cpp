#ifndef DBLIST_H
#define DBLIST_H

#include "DbVariant.h"


namespace LazyOrm {

class DbList : public DbVariant
{
public:
    DbList();
    DbList(std::initializer_list<DbVariant> input);

    void setList(std::vector<DbVariant> list);
};

}

#endif // DBLIST_H
