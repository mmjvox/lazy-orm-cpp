#include "Result.h"
#include "ResultRow.h"

namespace LazyOrm {

std::string Result::errorMessage() const
{
    return mErrorMessage;
}

std::string Result::toString() const
{
    // TODO remove last ,
    std::string retStr="[";
    for(const auto &value : *this){
        retStr.append(value.toString());
        retStr.append(",");
    }
    retStr.append("]");
    return retStr;
}

std::string Result::toIndentedString() const
{
    // TODO remove last ,
    std::string retStr="[";
    for(const auto &value : *this){
        retStr.append(value.toIndentedString());
        retStr.append(",");
    }
    retStr.append("]");
    return retStr;
}

bool Result::hasError() const {
    return !mErrorMessage.empty();
}

void Result::setError(std::string errorMessage){
    mErrorMessage = errorMessage;
}

size_t Result::affectedRows() const {
    return mAffectedRows;
}

void Result::setAffectedRows(size_t affectedRows)
{
    mAffectedRows = affectedRows;
}

void Result::setColumnNames(const std::vector<std::string> &columnNames)
{
    mColumnNames = columnNames;
}

std::vector<std::string> Result::columnNames(){
    return mColumnNames;
}

std::string Result::columnName(size_t index){
    if(mColumnNames.size()>index)
        return mColumnNames[index];
    return "";
}

size_t Result::columnsSize() const{
    return mColumnNames.size();
}

unsigned long long Result::insertId() const {
    return mInsertId;
}

void Result::setInsertId(unsigned long long insertId) {
    mInsertId = insertId;
}

ResultRow Result::value(unsigned long long index)
{
    if(index < std::vector<ResultRow>::size()){
        return std::vector<ResultRow>::at(index);
    }
    return {};
}

}
