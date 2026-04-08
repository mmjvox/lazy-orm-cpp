#ifndef JOINMARIADB_H
#define JOINMARIADB_H

#include "JoinAbstract.h"


namespace LazyOrm {

class JoinMariadb : public JoinAbstract
{
private:
    MariadbFilteringLazy mFilter;
    std::string  makeCondition (const Joined &j) const;

public:
    JoinMariadb();

    std::string propertiesToJoinedString(const LazyOrm &lazyOrm) const override;
    std::string joinedToString(const Joined &joined) const override;
    std::string queryString() const override;
};

}

#endif // JOINMARIADB_H
