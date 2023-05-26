#include "DbVariant.h"

std::string LazyOrm::DbVariant::DbVariantToString::operator()(const std::string &value){return value;}

std::string LazyOrm::DbVariant::DbVariantToString::operator()(const uint64_t &value){return std::to_string(value);}

std::string LazyOrm::DbVariant::DbVariantToString::operator()(const int &value){return std::to_string(value);}

std::string LazyOrm::DbVariant::DbVariantToString::operator()(const double &value){return std::to_string(value);}

std::string LazyOrm::DbVariant::DbVariantToString::operator()(const float &value){return std::to_string(value);}

std::string LazyOrm::DbVariant::DbVariantToString::operator()(const bool &value){return value?"true":"false";}

std::string LazyOrm::DbVariant::toString() const
{
    return std::visit(DbVariantToString{}, *this);
}

std::string LazyOrm::DbVariant::toLowerString() const
{
    std::string lower = toString();
    for(auto& ch : lower)
    {
        ch=std::tolower(ch);
    }
    return lower;
}

bool LazyOrm::DbVariant::empty()
{
    return std::visit([=](auto&& arg) -> bool {

        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::string>) {
            if(!arg.empty())
            {
                return false;
            }
        }

        return true;

    }, *this);
}

std::string LazyOrm::DbVariant::setQuote() const
{
    std::string strVal = toString();
    if(toLowerString()=="true")
    {
        return strVal;
    }
    if(toLowerString()=="false")
    {
        return strVal;
    }
    if(toLowerString()=="null")
    {
        return "NULL";
    }
    if(strVal.substr(0,5)=="[no']")
    {
        return strVal.substr(5);
    }
    return "'"+strVal+"'";
}
