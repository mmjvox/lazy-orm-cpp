#ifndef JOINMARIADB_H
#define JOINMARIADB_H

#include "JoinAbstract.h"


namespace LazyOrm {

class JoinMariadb : public JoinAbstract
{
public:
    JoinMariadb();

    std::string propertiesToJoinedString(const LazyOrm &lazyOrm) const override;
    std::string joinedToString(const Joined &joined) const override;

private:
    std::string  makeCondition (const Joined &j) const;
};

}

#endif // JOINMARIADB_H
