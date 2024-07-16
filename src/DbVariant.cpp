#include "DbVariant.h"
#include <cfloat>
#include <cstdint>
#include <climits>
#include <limits>


std::string LazyOrm::DbVariant::toString() const
{
    return std::visit([=, this](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        }
        else if constexpr (std::is_same_v<T, UnsignedIntegerVariant>) {
            return toFixedString(toUnsignedInteger());
        }
        else if constexpr (std::is_same_v<T, SignedIntegerVariant>) {
                return  toFixedString(toSignedInteger());
        }
        else if constexpr (std::is_same_v<T, SignedFloatingPointVariant>) {
                return  toFixedString(toSignedFloatingPoint());
        }
        else if constexpr (std::is_same_v<T, bool>) {
                return arg?"true":"false";
        }
        return "";
    }, *this);
}

std::string LazyOrm::DbVariant::typeName() const
{
    return std::visit([=](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "string";
        }
        else if constexpr (std::is_same_v<T, UnsignedIntegerVariant>) {
            return "uint";
        }
        else if constexpr (std::is_same_v<T, SignedIntegerVariant>) {
            return  "int";
        }
        else if constexpr (std::is_same_v<T, SignedFloatingPointVariant>) {
            return  "float";
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return "boolean";
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
        throw "error toUnsignedIntegerVariant";
    }, *this);
}

bool LazyOrm::DbVariant::isUnsignedIntegerVariant() const
{
    return std::visit([=](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, UnsignedIntegerVariant>) {
            return  true;
        }
        return  false;
    }, *this);
}

unsigned long long LazyOrm::DbVariant::toUnsignedInteger() const
{
    return std::visit([=](auto&& arg) -> unsigned long long {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, unsigned long long>) {
            return  static_cast<unsigned long long>(arg);
        }
        if constexpr (std::is_same_v<T, unsigned long>) {
            return  static_cast<unsigned long>(arg);
        }
        if constexpr (std::is_same_v<T, unsigned int>) {
            return  static_cast<unsigned int>(arg);
        }
        if constexpr (std::is_same_v<T, unsigned short>) {
            return  static_cast<unsigned short>(arg);
        }
        return 0;
    }, toUnsignedIntegerVariant());
}

bool LazyOrm::DbVariant::can_UInt_Fit_Int(unsigned long long value) const
{
    if (value <= LLONG_MAX) {
        return true;
    }
    return false;
}

bool LazyOrm::DbVariant::can_UInt_Fit_Float(unsigned long long value) const
{
    if (value <= LDBL_MAX) {
        return true;
    }
    return false;
}

unsigned long long LazyOrm::DbVariant::toUInt64() const
{
    if(isUnsignedIntegerVariant()){
        return toUnsignedInteger();
    }
    if(isSignedFloatingPointVariant()){
        auto value = toSignedFloatingPoint();
        if(can_Float_Fit_UInt(value)){
            return value;
        }
        return -1;
    }
    if(isSignedIntegerVariant()){
        auto value = toSignedInteger();
        if(can_Int_Fit_UInt(value)){
            return value;
        }
        return -1;
    }

    return std::visit([=, this](auto&& arg) -> unsigned long long {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::string>) {
            try {
                auto converted = std::stold(arg);
                if(can_Float_Fit_UInt(converted)){
                    return converted;
                }
                return -1;
            } catch (const std::exception&) {
                return -1;
            }
        }
        if constexpr (std::is_same_v<T, bool>) {
            return arg?1:0;
        }

        return -1;
    }, *this);

    return -1;
}


LazyOrm::SignedFloatingPointVariant LazyOrm::DbVariant::toSignedFloatingPointVariant() const
{
    return std::visit([=](auto&& arg) -> SignedFloatingPointVariant {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, SignedFloatingPointVariant>) {
            return  arg;
        }
        throw "error toSignedFloatingPointVariant";
    }, *this);
}

bool LazyOrm::DbVariant::isSignedFloatingPointVariant() const
{
    return std::visit([=](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, SignedFloatingPointVariant>) {
            return  true;
        }
        return  false;
    }, *this);
}

long double LazyOrm::DbVariant::toSignedFloatingPoint() const
{
    return std::visit([=](auto&& arg) -> long double {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, long double>) {
            return  static_cast<long double>(arg);
        }
        if constexpr (std::is_same_v<T, double>) {
            return  static_cast<double>(arg);
        }
        if constexpr (std::is_same_v<T, float>) {
            return  static_cast<float>(arg);
        }
        return 0;
    }, toSignedFloatingPointVariant());
}

bool LazyOrm::DbVariant::can_Float_Fit_Int(long double value) const
{
    if (value >= LLONG_MIN && value <= LLONG_MAX) {
        return true;
    }
    return false;
}

bool LazyOrm::DbVariant::can_Float_Fit_UInt(long double value) const
{
    if (value >= 0 && value <= ULLONG_MAX) {
        return true;
    }
    return false;
}

std::string LazyOrm::DbVariant::toFixedString(long double value) const
{
    std::string str = std::to_string(value);
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    if (str.back() == '.') {
        str.pop_back();
    }
    return str;
}

long double LazyOrm::DbVariant::toLongDouble() const
{
    if(isSignedFloatingPointVariant()){
        return toSignedFloatingPoint();
    }
    if(isUnsignedIntegerVariant()){
        auto value = toUnsignedInteger();
        if(can_UInt_Fit_Float(value)){
            return value;
        }
        return -1;
    }
    if(isSignedIntegerVariant()){
        auto value = toSignedInteger();
        if(can_Int_Fit_Float(value)){
            return value;
        }
        return -1;
    }

    return std::visit([=, this](auto&& arg) -> long double {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::string>) {
            try {
                auto converted = std::stold(arg);
                return converted;
            } catch (const std::exception&) {
                return -1;
            }
        }
        if constexpr (std::is_same_v<T, bool>) {
            return arg?1:0;
        }

        return -1;
    }, *this);

    return -1;
}


LazyOrm::SignedIntegerVariant LazyOrm::DbVariant::toSignedIntegerVariant() const
{
    return std::visit([=](auto&& arg) -> SignedIntegerVariant {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, SignedIntegerVariant>) {
            return  arg;
        }
        throw "error toSignedIntegerVariant";
    }, *this);
}

bool LazyOrm::DbVariant::isSignedIntegerVariant() const
{
    return std::visit([=](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, SignedIntegerVariant>) {
            return  true;
        }
        return  false;
    }, *this);
}

long long LazyOrm::DbVariant::toSignedInteger() const
{
    return std::visit([=](auto&& arg) -> long long {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, long long>) {
            return  static_cast<long long>(arg);
        }
        if constexpr (std::is_same_v<T, long>) {
            return  static_cast<long>(arg);
        }
        if constexpr (std::is_same_v<T, int>) {
            return  static_cast<int>(arg);
        }
        if constexpr (std::is_same_v<T, short>) {
            return  static_cast<short>(arg);
        }
        return 0;
    }, toSignedIntegerVariant());
}

bool LazyOrm::DbVariant::can_Int_Fit_UInt(long long value) const
{
    if (value >= 0 && (unsigned long long)value <= ULLONG_MAX) {
        return true;
    }
    return false;
}

bool LazyOrm::DbVariant::can_Int_Fit_Float(long long value) const
{
    if (value >= -LDBL_MAX && value <= LDBL_MAX) {
        return true;
    }
    return false;
}

long long LazyOrm::DbVariant::toInt64() const
{
    if(isSignedIntegerVariant()){
        return toSignedInteger();
    }
    if(isSignedFloatingPointVariant()){
        auto value = toSignedFloatingPoint();
        if(can_Float_Fit_Int(value)){
            return value;
        }
        return -1;
    }
    if(isUnsignedIntegerVariant()){
        auto value = toUnsignedInteger();
        if(can_UInt_Fit_Int(value)){
            return value;
        }
        return -1;
    }

    return std::visit([=, this](auto&& arg) -> long long {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::string>) {
            try {
                auto converted = std::stold(arg);
                if(can_Float_Fit_Int(converted)){
                    return converted;
                }
                return -1;
            } catch (const std::exception&) {
                return -1;
            }
        }
        if constexpr (std::is_same_v<T, bool>) {
            return arg?1:0;
        }

        return -1;
    }, *this);

    return -1;
}


bool LazyOrm::DbVariant::toBool() const
{
    return std::visit([=, this](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            return  arg;
        }
        if constexpr (std::is_same_v<T, std::string>) {
            return !arg.empty();
        }
        else if constexpr (std::is_same_v<T, SignedFloatingPointVariant>) {
            return  toSignedFloatingPoint()>0;
        }
        else if constexpr (std::is_same_v<T, UnsignedIntegerVariant>) {
            return toUnsignedInteger()>0;
        }
        else if constexpr (std::is_same_v<T, SignedIntegerVariant>) {
            return  toSignedInteger()>0;
        }
        return false;
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

std::string LazyOrm::DbVariant::toUpperString() const
{
    std::string upper = toString();
    for(auto& ch : upper)
    {
        ch=std::toupper(ch);
    }
    return upper;
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

LazyOrm::DbVariant LazyOrm::DbVariant::convartStringToBestMatchType()
{
    std::visit([this](auto&& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::string>) {
            try {
                auto converted = std::stold(value);
                // TODO: fix
                // std::string convertedSTR = toFixedString(converted);
                // if (convertedSTR!=value) {
                //     throw "converted is same as value";
                // }
                *this = converted;
            }
            catch (const std::exception&) {}
            catch (...) {}
        }
    }, *this);

    return *this;
}
