#include "JoinAbstract.h"
#include <algorithm>

namespace LazyOrm {

JoinAbstract::JoinAbstract() {}

LazyOrm &JoinAbstract::operator[](const enum Primary &primary)
{
    return mPrimaryOne;
}

const LazyOrm & JoinAbstract::primaryOne() const
{
    return mPrimaryOne;
}

std::vector<Joined> &JoinAbstract::operator[](const enum Joins &joinType)
{
    return mJoinedOnes;
}

std::string JoinAbstract::minQueryString() const
{
    std::string queryString = "SELECT \n";

    queryString.append(propertiesToJoinedString(mPrimaryOne));
    queryString.append(" \n");

    for(const auto &joinedItem : mJoinedOnes){
        queryString.append(propertiesToJoinedString(joinedItem.joinedOne));
        queryString.append(" \n");
    }

    queryString.append("FROM ");
    queryString.append(mPrimaryOne.tabeName());
    queryString.append(" \n");

    for(const auto &joinedItem : mJoinedOnes){
        queryString.append(joinedToString(joinedItem));
        queryString.append(" \n");
    }

    return queryString;
}

std::string JoinAbstract::query_with_trim_consecutive_spaces() const
{
    std::string str = queryString();
    auto new_end = std::unique(str.begin(), str.end(), [](char a, char b) {
        return a == ' ' && b == ' ';
    });
    str.erase(new_end, str.end());
    return str;
}

std::string JoinAbstract::string_join(const std::string &delimiter, const std::vector<std::string> &container) const
{
    size_t size = container.size();
    size_t endPos = container.size()-1;
    std::string output;
    for(size_t i = 0; i < size; ++i) {
        output.append(container[i]);
        if(i!=endPos){
            output.append(delimiter);
        }
    }
    return output;
}

}
