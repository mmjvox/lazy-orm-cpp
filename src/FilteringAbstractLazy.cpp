#include "FilteringAbstractLazy.h"
#include <iostream>

namespace LazyOrm {
std::string FilteringAbstractLazy::filterStr(Filters f)
{
    switch (f) {
    case AND:
        return "AND";
    case OR:
        return "OR";
    case LIMIT:
        return "LIMIT";
    }
}

FilteringAbstractLazy::FilteringAbstractLazy()
{

}

std::string FilteringAbstractLazy::toStringVal(const filterTypes &value){
  return std::visit(filterTypeToVal{}, value);
}

void FilteringAbstractLazy::setFilter(std::initializer_list<filterTypes> f)
{
    mConditions.push_back({Filters::AND, f});
}

void FilteringAbstractLazy::setFilter(const Filters &filter, std::initializer_list<filterTypes> f)
{
    mConditions.push_back({filter, f});
}

void FilteringAbstractLazy::setFilter(const Filters &filter, filterTypes &f)
{
    mConditions.push_back({filter, {f}});
}

std::string FilteringAbstractLazy::testString()
{
    std::string retStr;
    size_t conditionsSize = mConditions.size();
    for(int i=0;  i<conditionsSize; i++)
    {
        auto &item = mConditions.at(i);
        if(item.first==Filters::AND || item.first==Filters::OR)
        {
            if(i>0)
            {
                retStr.append(filterStr(item.first));
                retStr.append(" ");
            }

    //        std::vector<filterPair> nested ;//= std::visit(filterTypeToNested{}, item);
    //        if(!nested.empty())
    //        {
    //            sss.append("nested \n");
    //        }
    //        else
            if(item.second.size()==2)
            {
                retStr.append("`");
                retStr.append(toStringVal(item.second.at(0)));
                retStr.append("` = '");
                retStr.append(toStringVal(item.second.at(1)));
                retStr.append("' ");
            }
            else if(item.second.size()==3)
            {
                retStr.append("`");
                retStr.append(toStringVal(item.second.at(0)));
                retStr.append("` ");
                retStr.append(toStringVal(item.second.at(1)));
                retStr.append(" '");
                retStr.append(toStringVal(item.second.at(2)));
                retStr.append("' ");
            }
        }
        else if(item.first==Filters::LIMIT)
        {
            retStr.append("LIMIT ");
            if(item.second.size()==2)
            {
                retStr.append(toStringVal(item.second.at(0)));
                retStr.append(",");
                retStr.append(toStringVal(item.second.at(1)));
            }
            else if(item.second.size()==1)
            {
                retStr.append(toStringVal(item.second.at(0)));
            }
        }

        retStr.append("\n ");
    }
    return retStr;
}

//void FilteringAbstractLazy::test_init(std::initializer_list<filterTypes> f)
//{
//    for(const auto &item : f)
//    {
//        std::cout <<"*--> "<< std::visit(dbTypeToString{}, item) << std::endl;
//    }
//}

//filterTypes &FilteringAbstractLazy::operator[](const Filters &filter)
//{
//    return NULL;
//}
}
