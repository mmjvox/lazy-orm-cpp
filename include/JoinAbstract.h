#ifndef JOINABSTRACT_H
#define JOINABSTRACT_H

#include "LazyOrm.h"

namespace LazyOrm {

enum Primary{
    Primary
};

enum Joins{
    Joins
};

enum JoinRole{
    LeftJoin,
    RightJoin,
    InnerJoin,
    FullJoin,
    CrossJoin
};

struct Joined
{
    JoinRole joinRole;
    LazyOrm joinedOne;
    std::string joinedColumn;
    std::string primaryColumn;
    Joined(const JoinRole &joinRole, const LazyOrm &joinedOneLazyOrm, const std::string &joinedColumn, const std::string &primaryColumn)
    {
        this->joinRole = joinRole;
        this->joinedOne = joinedOneLazyOrm;
        this->joinedColumn = joinedColumn;
        this->primaryColumn = primaryColumn;
    }
};

class JoinAbstract
{
private:
    LazyOrm mPrimaryOne;
    std::vector<Joined> mJoinedOnes;

protected:
    WhereFilter mWhereFilter;
    HavingFilter mHavingFilter;
    std::string string_join(const std::string &delimiter, const std::vector<std::string> &container) const;
    std::string minQueryString() const;

public:
    JoinAbstract();

    LazyOrm & operator[](const enum Primary &primary);
    std::vector<Joined> & operator[](const enum Joins &joinType);

    const LazyOrm & primaryOne() const;

    virtual std::string propertiesToJoinedString(const LazyOrm &lazyOrm) const = 0;
    virtual std::string joinedToString(const Joined &joined) const = 0;
    virtual std::string queryString() const = 0;

    std::string query_with_trim_consecutive_spaces() const;
};

}

#endif // JOINABSTRACT_H
