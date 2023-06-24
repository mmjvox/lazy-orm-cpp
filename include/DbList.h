#ifndef DBLIST_H
#define DBLIST_H

#include "DbVariant.h"


namespace LazyOrm {

class DbList : public DbVariant
{
public:
    DbList();
    DbList(std::initializer_list<DbVariant> input);
};

}

#endif // DBLIST_H
