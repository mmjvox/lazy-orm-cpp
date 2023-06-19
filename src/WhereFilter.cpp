#include "WhereFilter.h"
#include <iostream>

namespace LazyOrm {

WhereFilter::~WhereFilter()
{
//    std::cerr << "~WhereFilter: "<< MDaaaaa.toString() << std::endl;
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
    for(const auto& filter : whereFilters)
    {
        std::visit([=](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::vector<LazyOrm::WhereFilter>>) {
                std::cerr << "std::vector<LazyOrm::WhereFilter>" << std::endl;
            }
            if constexpr (std::is_same_v<T, WhereFilter>) {
                std::cerr << "WhereFilter" << std::endl;
            }
            if constexpr (std::is_same_v<T, std::vector<LazyOrm::DbVariant>>) {
                size_t size = arg.size();
                size_t endPos = arg.size()-1;
                std::string output;
                for(size_t i = 0; i < size; ++i) {
                    output.append(arg[i].toString());
                    if(i!=endPos){
                        output.append(",");
                    }
                }
                std::cerr << "std::vector<LazyOrm::DbVariant>: " << output << " depth:"<<mDepth << std::endl;
                ++mDepth;
            }
            if constexpr (std::is_same_v<T, LazyOrm::DbVariant>) {
                //MDaaaaa=arg;
                std::cerr << "LazyOrm::DbVariant: " << arg.toString() << " depth:"<<mDepth << std::endl;
                ++mDepth;
            }
//            if constexpr (std::is_same_v<T, std::vector<std::string>>) {
//                size_t size = arg.size();
//                size_t endPos = arg.size()-1;
//                std::string output;
//                for(size_t i = 0; i < size; ++i) {
//                    output.append(arg[i]);
//                    if(i!=endPos){
//                        output.append(",");
//                    }
//                }
//                std::cerr << "std::vector<std::string>: " << output << std::endl;
//            }
//            if constexpr (std::is_same_v<T, std::string>) {
//                std::cerr << "std::string "<< arg << std::endl;
//            }
            if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::cerr << "std::vector<int>: "<< std::endl;
            }
            if constexpr (std::is_same_v<T, int>) {
                std::cerr << "int: "<< arg << std::endl;
            }
        }, filter);

    }
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


}
