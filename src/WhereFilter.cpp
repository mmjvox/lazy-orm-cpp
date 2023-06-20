#include "WhereFilter.h"
#include <iostream>

namespace LazyOrm {

WhereFilter::~WhereFilter()
{
//    std::cerr << "~WhereFilter: "<< MDaaaaa.toString() << std::endl;
}

WhereFilter::WhereFilter(std::initializer_list<WhereFilter> input) : nestedClasses_(input)
{
    std::cerr << "initializer_list<WhereFilter>" << this << std::endl;

    for(const auto& filter : input)
    {
        std::cerr << "\t initializer_list<WhereFilter>" << &filter << std::endl;
    }
}

LazyOrm::WhereFilter::WhereFilter(){
    std::cerr << "() " << this << std::endl;
}
LazyOrm::WhereFilter::WhereFilter(Filters filter):filter{filter}{
//    std::cerr << "(Filters filter): "<< filter << this << std::endl;
}

LazyOrm::WhereFilter::WhereFilter(std::initializer_list<std::variant<std::vector<DbVariant>,DbVariant>> whereFilters)
//    : mData{{whereFilters}}
{
    std::cerr << " func 1 "<< this << std::endl;
    std::vector<DbVariant> tempVector;
    for(const auto& filter : whereFilters)
    {
        std::visit([this, &tempVector](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<LazyOrm::DbVariant>>) {
                mNestedDbVariant.push_back(arg);
            }
            if constexpr (std::is_same_v<T, LazyOrm::DbVariant>) {
                std::cerr << "DbVariant: " << arg.toString() <<" " <<this << std::endl;
                tempVector.push_back(arg);
            }
        }, filter);
    }
    if(!tempVector.empty())
    {
        mNestedDbVariant.push_back(tempVector);
    }
    //printNestedVectors();
}

//LazyOrm::WhereFilter::WhereFilter(Filters filter, std::vector<WhereFilter> whereFilters)
//    : vector{whereFilters}, filter{filter}
//{
//    std::cerr << "2" << std::endl;
//}

//LazyOrm::WhereFilter::WhereFilter(Filters filter, std::vector<DbVariant> whereFilters)
//    : mData{{whereFilters}}, filter{filter}
//{
//    std::cerr << "3" << std::endl;
//}

//LazyOrm::WhereFilter::WhereFilter(Filters filter, std::vector<std::vector<DbVariant>> whereFilters)
//    : mData{whereFilters}, filter{filter}
//{
//    std::cerr << "4" << std::endl;
//}

//LazyOrm::WhereFilter::WhereFilter(Filters filter, std::vector<std::vector<WhereFilter> > whereFilters)
////    : vector{whereFilters}, filter{filter}
//{
//}

LazyOrm::WhereFilter::WhereFilter(std::vector<DbVariant> whereFilters)
//    : mData{{whereFilters}}, filter{Filters::AND}
{
    std::cerr << "5" << std::endl;
}

LazyOrm::WhereFilter::WhereFilter(std::vector<WhereFilter> whereFilters)
//    : vector{whereFilters}, filter{Filters::AND}
{
    std::cerr << "6" << std::endl;
}

void WhereFilter::printNestedVectors() const {
    std::cout << this << std::endl;

    if(nestedClasses_.size()>0)
    {
        for (const auto& whereFilter : nestedClasses_) {
            printNestedVectors(whereFilter);
        }
    }
    else if(mNestedDbVariant.size()>0)
    {
        for (const auto& nestedVectorVariant : mNestedDbVariant) {

            for (const auto& dbVariant : nestedVectorVariant) {
                std::cout << dbVariant.toString() << " ";
            }
            std::cout <<"_"<< std::endl;
        }
    }
}

void WhereFilter::printNestedVectors(WhereFilter wf) const
{
    for (const auto& whereFilter : wf.nestedClasses_) {
        if(whereFilter.nestedClasses_.size()>0)
        {
            printNestedVectors(whereFilter);
        }
        else if(whereFilter.mNestedDbVariant.size()>0)
        {
            for (const auto& nestedVectorVariant : whereFilter.mNestedDbVariant) {

                for (const auto& dbVariant : nestedVectorVariant) {
                    std::cout << dbVariant.toString() << " ";
                }
                std::cout <<"_"<< std::endl;
            }
        }
    }
}


}
