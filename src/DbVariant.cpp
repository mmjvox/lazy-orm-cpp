#include "DbVariant.h"

std::string LazyOrm::DbVariant::toString() const
{
    return std::visit([=](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        }
        else if constexpr (std::is_same_v<T, UnsignedIntegerVariant>) {
            return std::to_string(toUInt64());
        }
        else if constexpr (std::is_same_v<T, SignedIntegerVariant>) {
                return  std::to_string(toInt64());
        }
        else if constexpr (std::is_same_v<T, SignedFloatingPointVariant>) {
                return  std::to_string(toLongDouble());
        }
        else if constexpr (std::is_same_v<T, bool>) {
                return arg?"true":"false";
        }
        return "";
    }, *this);
}

LazyOrm::UnsignedIntegerVariant LazyOrm::DbVariant::toUnsignedIntegerVariant() const
{
    return std::visit([=](auto&& arg) -> UnsignedIntegerVariant {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, UnsignedIntegerVariant>) {
            return  arg;
        }
        return {};
    }, *this);
}
unsigned long long LazyOrm::DbVariant::toUInt64() const
{
    return std::visit([=](auto&& arg) -> unsigned long long {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, unsigned long long>) {
            return  arg;
        }
        if constexpr (std::is_same_v<T, unsigned long>) {
            return  arg;
        }
        if constexpr (std::is_same_v<T, unsigned int>) {
            return  arg;
        }
        if constexpr (std::is_same_v<T, unsigned short>) {
            return  arg;
        }
        return 0;
    }, toUnsignedIntegerVariant());
}


LazyOrm::SignedFloatingPointVariant LazyOrm::DbVariant::toSignedFloatingPointVariant() const
{
    return std::visit([=](auto&& arg) -> SignedFloatingPointVariant {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, SignedFloatingPointVariant>) {
            return  arg;
        }
        return 0.0;
    }, *this);
}
long double LazyOrm::DbVariant::toLongDouble() const
{
    return std::visit([=](auto&& arg) -> long double {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, long double>) {
            return  arg;
        }
        if constexpr (std::is_same_v<T, double>) {
            return  arg;
        }
        if constexpr (std::is_same_v<T, float>) {
            return  arg;
        }
        return 0;
    }, toSignedFloatingPointVariant());
}


LazyOrm::SignedIntegerVariant LazyOrm::DbVariant::toSignedIntegerVariant() const
{
    return std::visit([=](auto&& arg) -> SignedIntegerVariant {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, SignedIntegerVariant>) {
            return  arg;
        }
        return 0;
    }, *this);
}
long long LazyOrm::DbVariant::toInt64() const
{
    return std::visit([=](auto&& arg) -> long long {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, long long>) {
            return  arg;
        }
        if constexpr (std::is_same_v<T, long>) {
            return  arg;
        }
        if constexpr (std::is_same_v<T, int>) {
            return  arg;
        }
        if constexpr (std::is_same_v<T, short>) {
            return  arg;
        }
        return 0;
    }, toSignedIntegerVariant());
}


bool LazyOrm::DbVariant::toBool() const
{
    return std::visit([=](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            return  arg;
        }
        return 0;
    }, *this);
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
    auto substr05=strVal.substr(0,5);
    if(substr05=="[no']" or substr05=="[no`]")
    {
        return strVal.substr(5);
    }
    return "'"+strVal+"'";
}

std::string LazyOrm::DbVariant::setBackTick() const
{
    std::string strVal = toString();
    auto substr05=strVal.substr(0,5);
    if(substr05=="[no']" or substr05=="[no`]")
    {
        return strVal.substr(5);
    }
    if(strVal.substr(0,6)=="[func]")
    {
        return strVal.substr(6);
    }
    if(strVal.substr(0,8)=="[update]")
    {
        return "`"+strVal.substr(8)+"`";
    }
    return "`"+strVal+"`";
}

std::string LazyOrm::DbVariant::toCleanString() const
{
    std::string strVal = toString();
    auto substr05=strVal.substr(0,5);
    if(substr05=="[no']" or substr05=="[no`]")
    {
        return strVal.substr(5);
    }
    if(strVal.substr(0,6)=="[func]")
    {
        return strVal.substr(6);
    }
    if(strVal.substr(0,8)=="[update]")
    {
        return strVal.substr(8);
    }
    return strVal;
}

bool LazyOrm::DbVariant::isUpdate() const
{
    return toString().substr(0,8)=="[update]";
}
