#include "WhereFilterPostgre.h"
#include <iostream>

namespace LazyOrm {

WhereFilterPostgre::WhereFilterPostgre() {
}

WhereFilterPostgre::WhereFilterPostgre(const WhereFilter &wf)
{
    WhereFilter::mNestedDbVariant = wf.nestedDbVariant();
    WhereFilter::mNestedWhereFilters = wf.nestedWhereFilters();
}


std::string WhereFilterPostgre::string_join(const std::string &delimiter, const std::vector<DbVariant> &container) const
{
    if(container.size()==1){
        return container[0].toString();
    }

    if(container.size()==2){
        return container[0].setDoubleQuote() + " = " +container[1].setQuote();
    }

    if(container.size()==3){
        return container[0].setDoubleQuote() + " " + container[1].toString() + " " +container[2].setQuote();
    }

    if(container.size()==5 && container[1].toLowerString()=="between"){
        return container[0].setDoubleQuote() + " " + container[1].toString() + " " + container[2].setQuote() + " " + container[3].toString() + " " + container[4].setQuote();
    }

    size_t size = container.size();
    size_t endPos = container.size()-1;
    std::string output;
    for(size_t i = 0; i < size; ++i) {
        output.append(container[i].toString());
        if(i!=endPos){
            output.append(delimiter);
        }
    }
    return output;
}

}
