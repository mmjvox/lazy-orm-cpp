#include "FilteringAbstractLazy.h"
#include <iostream>

namespace LazyOrm {


std::string FilteringAbstractLazy::filterStr(Filters f)
{
    switch (f) {
    case OR:
        return "OR";
    case AND:
        return "AND";
    case LIMIT:
        return "LIMIT";
    case HAVING:
        return "HAVING";
    case ORDERBY:
        return "ORDER BY";
    case GROUPBY:
        return "GROUP BY";
    }
    return {};
}

FilteringAbstractLazy::FilteringAbstractLazy()
{

}

std::string FilteringAbstractLazy::toStringVal(const WhereTypes &value){
  return std::visit(WhereTypeToVal{}, value);
}

void FilteringAbstractLazy::setFilter(std::initializer_list<WhereTypes> f)
{
    mWhereConditions.push_back({Filters::AND, f});
}

void FilteringAbstractLazy::setFilter(const Filters &filter, std::initializer_list<WhereTypes> f)
{
    switch (filter) {
    case OR:
    case AND:
        mWhereConditions.push_back({filter, f});
        break;
    case ORDERBY:
        break;
    case LIMIT:
        break;
    case HAVING:
        break;
    case GROUPBY:
        break;
    }
}

void FilteringAbstractLazy::setFilter(const Filters &filter, FilterTypes f)
{
    switch (filter) {
    case LIMIT:
        mLimitConditions.push_back(f);
        break;
    case ORDERBY:
        mOrderConditions.push_back(f);
        break;
    case HAVING:
        mHavingConditions.push_back(f);
        break;
    case GROUPBY:
        mGroupConditions.push_back(f);
        break;
    default:
        break;
    }
}

void FilteringAbstractLazy::setFilter(const Filters &filter, WhereTypes &f)
{
    mWhereConditions.push_back({filter, {f}});
}


std::string FilteringAbstractLazy::testString()
{
    std::string retStr;
    size_t conditionsSize = mWhereConditions.size();
    for(int i=0;  i<conditionsSize; i++)
    {
        auto &item = mWhereConditions.at(i);
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
