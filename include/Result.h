#ifndef RESULT_H
#define RESULT_H

#include <cstdint>
#include <string>
#include <vector>

namespace LazyOrm {

class ResultRow;

class Result : public std::vector<ResultRow>
{
private:
    std::string mErrorMessage;
    size_t mAffectedRows;
    unsigned long long mInsertId=0;
    std::vector<std::string> mColumnNames;

public:
    using std::vector<ResultRow>::vector;

    std::string toString() const;
    std::string toIndentedString() const;
    bool hasError() const;
    void setError(std::string errorMessage);
    size_t affectedRows() const;
    void setAffectedRows(size_t affectedRows);
    std::vector<std::string> columnNames();
    std::string columnName(size_t index);
    size_t columnsSize() const;
    unsigned long long insertId() const;
    void setInsertId(unsigned long long insertId);
    void setColumnNames(const std::vector<std::string> &columnNames);
};

}

#endif // RESULT_H
