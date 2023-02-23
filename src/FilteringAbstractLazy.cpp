#include "FilteringAbstractLazy.h"

namespace LazyOrm {
std::string FilteringAbstractLazy::filterStr(Filters f)
{
    switch (f) {
    case AND:
        return "AND";
    case OR:
        return "OR";
    }
}

FilteringAbstractLazy::FilteringAbstractLazy()
{

}

void FilteringAbstractLazy::setFilter(const Filters &filter, std::initializer_list<std::string> f)
{
    mConditions.push_back({filter, f});
}

std::string FilteringAbstractLazy::testString()
{
    std::string sss;
    for(const auto &item : mConditions)
    {
        sss.append(filterStr(item.first));
        sss.append(" ");
        if(item.second.size()==2)
        {
            sss.append(item.second.at(0)+"="+item.second.at(1));
        }
        else if(item.second.size()==3)
        {
            sss.append(item.second.at(0)+item.second.at(1)+item.second.at(2));
        }
        sss.append("\n ");
    }
    return sss;
}

//filterTypes &FilteringAbstractLazy::operator[](const Filters &filter)
//{
//    return NULL;
//}
}
