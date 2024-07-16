#include "ResultRow.h"
#include "DbVariant.h"

std::string LazyOrm::ResultRow::toString() const
{
    // TODO remove last ,
    std::string retStr="{";
    for(const auto &[key, value] : *this){
        retStr.append(key);
        retStr.append(":");
        retStr.append(value.toString());
        retStr.append(",");
    }
    retStr.append("}");
    return retStr;
}
