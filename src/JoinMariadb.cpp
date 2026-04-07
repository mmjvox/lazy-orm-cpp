#include "JoinMariadb.h"
#include <functional>

namespace LazyOrm {

JoinMariadb::JoinMariadb() {}

std::string JoinMariadb::propertiesToJoinedString(const LazyOrm &lazyOrm) const
{
    const auto &tableName = DbVariant{lazyOrm.tabeName()}.setBackTick() + ".";
    std::vector<std::string> keys;
    for(const auto &[key, value] : lazyOrm.properties())
    {
        if(key.toString()!="*")
        {
            keys.push_back(tableName + key.setBackTick());
        }
    }
    return string_join(", \n",keys);
}

std::string JoinMariadb::joinedToString(const Joined &joined) const
{
    switch (joined.joinRole) {
    case LeftJoin:
        return "LEFT JOIN " + makeCondition(joined);
    case RightJoin:
        return "RIGHT JOIN " + makeCondition(joined);
    case InnerJoin:
        return "INNER JOIN " + makeCondition(joined);
    case FullJoin:
        return "FULL JOIN " + makeCondition(joined);
    case CrossJoin:
        return "CROSS JOIN " + DbVariant{joined.joinedOne.tabeName()}.setBackTick();
    }

    return "JOIN " + makeCondition(joined);;
}

std::string JoinMariadb::makeCondition(const Joined &j) const
{
    return  j.joinedOne.tabeName() +
            " ON " +
            DbVariant{primaryOne().tabeName()}.setBackTick() + "." +  DbVariant{j.primaryColumn}.setBackTick() +
            " = " +
            DbVariant{j.joinedOne.tabeName()}.setBackTick() + "." +  DbVariant{j.joinedColumn}.setBackTick();
}

}
