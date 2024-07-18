#include "ResultRow.h"
#include "DbVariant.h"

std::string LazyOrm::ResultRow::toString() const
{
    // TODO remove last ,
    std::string retStr="{";
    for(auto [key, value] : *this){
        retStr.append("\""+key+"\"");
        retStr.append(":");
        // value.alterStringToBestMatchType();
        if(value.typeName()=="string"){
            retStr.append("\""+value.toString()+"\"");
        } else {
            retStr.append(value.toString());
        }
        retStr.append(",");
    }
    retStr.append("}");
    return retStr;
}

std::string LazyOrm::ResultRow::toIndentedString() const
{
    // TODO remove last ,
    std::string retStr="\n  {";
    for(auto [key, value] : *this){
        retStr.append("\n   \""+key+"\"");
        retStr.append(":");
        // value.alterStringToBestMatchType();
        if(value.typeName()=="string"){
            retStr.append("\""+value.toString()+"\"");
        } else {
            retStr.append(value.toString());
        }
        retStr.append(",");
    }
    retStr.append("\n  }");
    return retStr;
}
