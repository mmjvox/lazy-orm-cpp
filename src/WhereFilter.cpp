#include "WhereFilter.h"
#include <iostream>

namespace LazyOrm {

WhereFilter::~WhereFilter()
{
}

WhereFilter::WhereFilter(std::string str)
{
    if(str.empty()){
        return;
    }
    mNestedDbVariant.push_back({str});
    mNestedWhereFilters.push_back(*this);
}

WhereFilter::WhereFilter(std::initializer_list<WhereFilter> wfs) : mNestedWhereFilters(wfs.begin(), wfs.end()) {}

LazyOrm::WhereFilter::WhereFilter()
{
}

LazyOrm::WhereFilter::WhereFilter(DbVariant variant){
    if(variant.empty()){
        return;
    }
    mNestedDbVariant.push_back({variant});
    mNestedWhereFilters.push_back(*this);
}

LazyOrm::WhereFilter::WhereFilter(std::initializer_list<std::variant<std::vector<DbVariant>,DbVariant>> variants)
{
    std::vector<DbVariant> tempVector;
    for(const auto& filter : variants)
    {
        std::visit([this, &tempVector](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<LazyOrm::DbVariant>>) {
                mNestedDbVariant.push_back(arg);
            }
            if constexpr (std::is_same_v<T, LazyOrm::DbVariant>) {
                tempVector.push_back(arg);
            }
        }, filter);
    }
    if(!tempVector.empty())
    {
        if(tempVector.size()==1)
        {
            const auto & var = tempVector.at(0).toUpperString();
            if(var=="AND" || var=="OR"  || var=="NOT" || var=="AND NOT" || var=="OR NOT")
            {
                mNestedDbVariant.push_back({{var}});
                mNestedWhereFilters.push_back(*this);
                return;
            }
        }
        if(tempVector.size()==2)
        {
            const auto & var1 = tempVector.at(0).toUpperString();
            const auto & var2 = tempVector.at(1).toUpperString();
            if(var1=="AND" || var1=="OR"  || var1=="NOT" || var2=="NOT")
            {
                mNestedDbVariant.push_back({{var1+" "+var2}});
                mNestedWhereFilters.push_back(*this);
                return;
            }
        }
        mNestedDbVariant.push_back(tempVector);
    }
}

LazyOrm::WhereFilter::WhereFilter(std::vector<DbVariant> variants)
{
    mNestedDbVariant.push_back(variants);
}

LazyOrm::WhereFilter::WhereFilter(std::vector<WhereFilter> wfs)
{
    if(wfs.empty()){
        return;
    }
    mNestedWhereFilters.push_back(wfs);
}

void WhereFilter::operator=(const WhereFilter &wf)
{
    if(wf.empty()){
        return;
    }
    mNestedWhereFilters.push_back(wf);
}

bool WhereFilter::empty() const
{
    return (mNestedWhereFilters.empty() && mNestedDbVariant.empty());
}

bool WhereFilter::isWhereFilter() const{
    return (mNestedWhereFilters.size()>0);
}

bool WhereFilter::isVariantVaector() const{
    return (mNestedDbVariant.size()>0);
}


std::string WhereFilter::toString() const
{
    std::string retStr;

    if(empty()){
        return retStr;
    }

    retStr = " WHERE ";

    if(isWhereFilter()){
        for (const auto& whereFilter : mNestedWhereFilters) {
            retStr.append(toString(whereFilter));
        }
    }
    else if(isVariantVaector()){
        for (const auto& nestedVectorVariant : mNestedDbVariant) {
            retStr.append(string_join(" ", nestedVectorVariant));
        }
    }

    return retStr.append(" ");
}

void WhereFilter::append(std::vector<DbVariant> variants)
{
    mNestedDbVariant.push_back(variants);
}

std::string WhereFilter::toString(WhereFilter wf) const
{
    std::string retStr;
    bool and_or=false;
    for (const auto& whereFilter : wf.mNestedWhereFilters) {
        if(whereFilter.isWhereFilter())
        {
            retStr.append( toString(whereFilter) );
        }
        else if(whereFilter.isVariantVaector())
        {
            for (const auto& nestedVectorVariant : whereFilter.mNestedDbVariant) {

                if(nestedVectorVariant.size()==1)
                {
                    and_or=true;
                    retStr.append(" " + nestedVectorVariant.at(0).toString() + " ");
                    return retStr;
                }

                retStr.append("(");
                retStr.append(string_join(" ", nestedVectorVariant));
                retStr.append(")");
            }
        }
    }

    if(wf.mNestedWhereFilters.size()>1){
        return "("+retStr+")";
    }
    return retStr;
}

std::string WhereFilter::string_join(const std::string &delimiter, const std::vector<DbVariant> &container) const
{
    if(container.size()==1){
        return container[0].toString();
    }

    if(container.size()==2){
        return container[0].setBackTick() + " = " +container[1].setQuote();
    }

    if(container.size()==3){
        return container[0].setBackTick() + " " + container[1].toString() + " " +container[2].setQuote();
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
