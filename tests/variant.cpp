#include <DbVariant.h>
#include <iostream>
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <climits>

TEST_CASE( "Factorials are computed", "[Lazy_DbVariant]" ) {

#ifdef DEBUG_MODE
    LazyOrm::DbVariant privateVar1 = -30;
    REQUIRE( privateVar1.isSignedIntegerVariant() == true );
    REQUIRE( privateVar1.isUnsignedIntegerVariant() == false );
    REQUIRE( privateVar1.isSignedFloatingPointVariant() == false );
    REQUIRE( privateVar1.toSignedInteger() == -30 );

    unsigned long long ull = 30;
    LazyOrm::DbVariant privateVar1_1 = ull;
    REQUIRE( privateVar1_1.isSignedIntegerVariant() == false );
    REQUIRE( privateVar1_1.isUnsignedIntegerVariant() == true );
    REQUIRE( privateVar1_1.isSignedFloatingPointVariant() == false );
    REQUIRE( privateVar1_1.toUnsignedInteger() == ull );

    LazyOrm::DbVariant privateVar2 = std::numeric_limits<unsigned long long>::max();
    REQUIRE( privateVar2.isSignedIntegerVariant() == false );
    REQUIRE( privateVar2.isUnsignedIntegerVariant() == true );
    REQUIRE( privateVar2.isSignedFloatingPointVariant() == false );
    REQUIRE( privateVar2.toUnsignedInteger() == std::numeric_limits<unsigned long long>::max() );

    LazyOrm::DbVariant privateVar3 = std::numeric_limits<long double>::max();
    REQUIRE( privateVar3.isSignedIntegerVariant() == false );
    REQUIRE( privateVar3.isUnsignedIntegerVariant() == false );
    REQUIRE( privateVar3.isSignedFloatingPointVariant() == true );
    REQUIRE( privateVar3.toSignedFloatingPoint() == std::numeric_limits<long double>::max() );
#endif


    LazyOrm::DbVariant var1 = 40;
    REQUIRE( var1.toInt64() == 40 );
    REQUIRE( var1.toUInt64() == 40 );
    REQUIRE( var1.toLongDouble() == 40 );
    REQUIRE( var1.toString() == "40" );
    REQUIRE( var1.toBool() == true );

    LazyOrm::DbVariant var2 = "41";
    REQUIRE( var2.toInt64() == 41 );
    REQUIRE( var2.toUInt64() == 41 );
    REQUIRE( var2.toLongDouble() == 41 );
    REQUIRE( var2.toString() == "41" );
    REQUIRE( var2.toBool() == true );

    LazyOrm::DbVariant var3 = "42";
    REQUIRE( var3.toInt64() == 42 );
    REQUIRE( var3.toUInt64() == 42 );
    REQUIRE( var3.toLongDouble() == 42 );
    REQUIRE( var3.toString() == "42" );
    REQUIRE( var3.toBool() == true );
    REQUIRE( var3.convartStringToBestMatchType().toInt64() == 42 );
    REQUIRE( var3.convartStringToBestMatchType().toUInt64() == 42 );
    REQUIRE( var3.convartStringToBestMatchType().toLongDouble() == 42 );
    REQUIRE( var3.convartStringToBestMatchType().toString() == "42" );
    REQUIRE( var3.convartStringToBestMatchType().toBool() == true );
    REQUIRE( var3.toInt64() == 42 );
    REQUIRE( var3.toUInt64() == 42 );
    REQUIRE( var3.toLongDouble() == 42 );
    REQUIRE( var3.toString() == "42" );
    REQUIRE( var3.toBool() == true );

    LazyOrm::DbVariant var4 = "42.00";
    REQUIRE( var4.toInt64() == 42 );
    REQUIRE( var4.toUInt64() == 42 );
    REQUIRE( var4.toLongDouble() == 42 );
    REQUIRE( var4.toString() == "42.00" );
    REQUIRE( var4.toBool() == true );
    REQUIRE( var4.convartStringToBestMatchType().toInt64() == 42 );
    REQUIRE( var4.convartStringToBestMatchType().toUInt64() == 42 );
    REQUIRE( var4.convartStringToBestMatchType().toLongDouble() == 42 );
    REQUIRE( var4.convartStringToBestMatchType().toString() == "42" );
    REQUIRE( var4.convartStringToBestMatchType().toBool() == true );
    REQUIRE( var4.toInt64() == 42 );
    REQUIRE( var4.toUInt64() == 42 );
    REQUIRE( var4.toLongDouble() == 42 );
    REQUIRE( var4.toString() == "42" );
    REQUIRE( var4.toBool() == true );

    LazyOrm::DbVariant var5 = "42.";
    REQUIRE( var5.toInt64() == 42 );
    REQUIRE( var5.toUInt64() == 42 );
    REQUIRE( var5.toLongDouble() == 42 );
    REQUIRE( var5.toString() == "42." );
    REQUIRE( var5.toBool() == true );
    REQUIRE( var5.convartStringToBestMatchType().toInt64() == 42 );
    REQUIRE( var5.convartStringToBestMatchType().toUInt64() == 42 );
    REQUIRE( var5.convartStringToBestMatchType().toLongDouble() == 42 );
    REQUIRE( var5.convartStringToBestMatchType().toString() == "42" );
    REQUIRE( var5.convartStringToBestMatchType().toBool() == true );
    REQUIRE( var5.toInt64() == 42 );
    REQUIRE( var5.toUInt64() == 42 );
    REQUIRE( var5.toLongDouble() == 42 );
    REQUIRE( var5.toString() == "42" );
    REQUIRE( var5.toBool() == true );

    LazyOrm::DbVariant var6 = "42.561651651561651641654156165156165414265465465";
    REQUIRE( var6.toInt64() == 42 );
    REQUIRE( var6.toUInt64() == 42 );
    REQUIRE( var6.toLongDouble() == Catch::Approx(42.5617).epsilon(0.0001));
    REQUIRE( var6.toString() == "42.561651651561651641654156165156165414265465465" );
    REQUIRE( var6.toBool() == true );
    REQUIRE( var6.convartStringToBestMatchType().toInt64() == 42 );
    REQUIRE( var6.convartStringToBestMatchType().toUInt64() == 42 );
    REQUIRE( var6.convartStringToBestMatchType().toLongDouble() == Catch::Approx(42.5617).epsilon(0.0001) );
    REQUIRE( var6.convartStringToBestMatchType().toString() == "42.561652" );
    REQUIRE( var6.convartStringToBestMatchType().toBool() == true );
    REQUIRE( var6.toInt64() == 42 );
    REQUIRE( var6.toUInt64() == 42 );
    REQUIRE( var6.toLongDouble() == Catch::Approx(42.5617).epsilon(0.0001) );
    REQUIRE( var6.toString() == "42.561652" );
    REQUIRE( var6.toBool() == true );



    LazyOrm::DbVariant var7 = "42561651651561651641654156165156165414265465465";
    // REQUIRE( var7.toInt64() == 42 );
    // REQUIRE( var7.toUInt64() == 42 );
    // REQUIRE( var7.toLongDouble() == Catch::Approx(42.5617).epsilon(0.0001));
    REQUIRE( var7.toString() == "42561651651561651641654156165156165414265465465" );
    // REQUIRE( var7.toBool() == true );
    // REQUIRE( var7.convartStringToBestMatchType().toInt64() == 42 );
    // REQUIRE( var7.convartStringToBestMatchType().toUInt64() == 42 );
    // REQUIRE( var7.convartStringToBestMatchType().toLongDouble() == Catch::Approx(42.5617).epsilon(0.0001) );
    REQUIRE( var7.convartStringToBestMatchType().toString() == "42561651651561651641654156165156165414265465465" );
    // REQUIRE( var7.convartStringToBestMatchType().toBool() == true );
    // REQUIRE( var7.toInt64() == 42 );
    // REQUIRE( var7.toUInt64() == 42 );
    // REQUIRE( var7.toLongDouble() == Catch::Approx(42.5617).epsilon(0.0001) );
    // REQUIRE( var7.toString() == "42.561652" );
    // REQUIRE( var7.toBool() == true );
}
