#include "ResultRow.h"

namespace LazyOrm {

std::string ResultRow::toString() const
{
    std::string retStr="{";
    bool first = true;
    for(auto [key, value] : *this){

        if (!first) {
            retStr.append(",");
        }
        first = false;

        retStr.append("\""+key+"\"");
        retStr.append(":");
        // value.alterStringToBestMatchType();
        if(value.typeName()=="string"){
            retStr.append("\""+value.toString()+"\"");
        } else {
            retStr.append(value.toString());
        }
    }
    retStr.append("}");
    return retStr;
}

std::string ResultRow::toIndentedString() const
{
    std::string retStr="\n  {";
    bool first = true;
    for(auto [key, value] : *this){

        if (!first) {
            retStr.append(",");
        }
        first = false;

        retStr.append("\n   \""+key+"\"");
        retStr.append(":");
        // value.alterStringToBestMatchType();
        if(value.typeName()=="string"){
            retStr.append("\""+value.toString()+"\"");
        } else {
            retStr.append(value.toString());
        }
    }
    retStr.append("\n  }");
    return retStr;
}

LazyOrm::DbVariant ResultRow::value(const std::string key, const DbVariant dbVariant) const
{
    auto value = find(key);
    if(value==std::unordered_map<std::string,DbVariant>::end()){
        return {};
    }
    return value->second;
}

void ResultRow::insert(const std::string key, const DbVariant dbVariant)
{
    std::unordered_map<std::string,DbVariant>::insert_or_assign(key, dbVariant);
}

LazyOrm::DbVariant ResultRow::operator[](const std::string key) const
{
    return this->value(key);
}

LazyOrm::DbVariant ResultRow::at(unsigned long long columnIndex) const
{
    unsigned long long column = -1;
    DbVariant dbVariant;
    for(const auto [key, value] : *this){
        column++;
        if(column==columnIndex){
            dbVariant = value;
            break;
        }
    }
    return dbVariant;
}

LazyOrm::DbVariant ResultRow::value(unsigned long long columnIndex) const
{
    return at(columnIndex);
}

}
