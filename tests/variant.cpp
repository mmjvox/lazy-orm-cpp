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
    REQUIRE( var1.toLongLong() == 40 );
    REQUIRE( var1.toULongLong() == 40 );
    REQUIRE( var1.toLongDouble() == 40 );
    REQUIRE( var1.toString() == "40" );
    REQUIRE( var1.toBool() == true );

    LazyOrm::DbVariant var2 = "41";
    REQUIRE( var2.toLongLong() == 41 );
    REQUIRE( var2.toULongLong() == 41 );
    REQUIRE( var2.toLongDouble() == 41 );
    REQUIRE( var2.toString() == "41" );
    REQUIRE( var2.toBool() == true );

    LazyOrm::DbVariant var3 = "42";
    REQUIRE( var3.toLongLong() == 42 );
    REQUIRE( var3.toULongLong() == 42 );
    REQUIRE( var3.toLongDouble() == 42 );
    REQUIRE( var3.toString() == "42" );
    REQUIRE( var3.toBool() == true );
    REQUIRE( var3.alterStringToBestMatchType().toLongLong() == 42 );
    REQUIRE( var3.alterStringToBestMatchType().toULongLong() == 42 );
    REQUIRE( var3.alterStringToBestMatchType().toLongDouble() == 42 );
    REQUIRE( var3.alterStringToBestMatchType().toString() == "42" );
    REQUIRE( var3.alterStringToBestMatchType().toBool() == true );
    REQUIRE( var3.toLongLong() == 42 );
    REQUIRE( var3.toULongLong() == 42 );
    REQUIRE( var3.toLongDouble() == 42 );
    REQUIRE( var3.toString() == "42" );
    REQUIRE( var3.toBool() == true );

    LazyOrm::DbVariant var4 = "42.00";
    REQUIRE( var4.toLongLong() == 42 );
    REQUIRE( var4.toULongLong() == 42 );
    REQUIRE( var4.toLongDouble() == 42 );
    REQUIRE( var4.toString() == "42.00" );
    REQUIRE( var4.toBool() == true );
    REQUIRE( var4.alterStringToBestMatchType().toLongLong() == 42 );
    REQUIRE( var4.alterStringToBestMatchType().toULongLong() == 42 );
    REQUIRE( var4.alterStringToBestMatchType().toLongDouble() == 42 );
    REQUIRE( var4.alterStringToBestMatchType().toString() == "42" );
    REQUIRE( var4.alterStringToBestMatchType().toBool() == true );
    REQUIRE( var4.toLongLong() == 42 );
    REQUIRE( var4.toULongLong() == 42 );
    REQUIRE( var4.toLongDouble() == 42 );
    REQUIRE( var4.toString() == "42" );
    REQUIRE( var4.toBool() == true );

    LazyOrm::DbVariant var5 = "42.";
    REQUIRE( var5.toLongLong() == 42 );
    REQUIRE( var5.toULongLong() == 42 );
    REQUIRE( var5.toLongDouble() == 42 );
    REQUIRE( var5.toString() == "42." );
    REQUIRE( var5.toBool() == true );
    REQUIRE( var5.alterStringToBestMatchType().toLongLong() == 42 );
    REQUIRE( var5.alterStringToBestMatchType().toULongLong() == 42 );
    REQUIRE( var5.alterStringToBestMatchType().toLongDouble() == 42 );
    REQUIRE( var5.alterStringToBestMatchType().toString() == "42" );
    REQUIRE( var5.alterStringToBestMatchType().toBool() == true );
    REQUIRE( var5.toLongLong() == 42 );
    REQUIRE( var5.toULongLong() == 42 );
    REQUIRE( var5.toLongDouble() == 42 );
    REQUIRE( var5.toString() == "42" );
    REQUIRE( var5.toBool() == true );

    LazyOrm::DbVariant var6 = "42.561651651561651641654156165156165414265465465";
    REQUIRE( var6.toLongLong() == 0 );
    REQUIRE( var6.toULongLong() == 0 );
    REQUIRE( var6.toLongDouble() == 0);
    REQUIRE( var6.toString() == "42.561651651561651641654156165156165414265465465" );
    REQUIRE( var6.toBool() == true );
    REQUIRE( var6.alterStringToBestMatchType().toLongLong() == 0 );
    REQUIRE( var6.alterStringToBestMatchType().toULongLong() == 0 );
    REQUIRE( var6.alterStringToBestMatchType().toLongDouble() == 0 );
    REQUIRE( var6.alterStringToBestMatchType().toString() == "42.561651651561651641654156165156165414265465465" );
    REQUIRE( var6.alterStringToBestMatchType().toBool() == true );
    REQUIRE( var6.toLongLong() == 0 );
    REQUIRE( var6.toULongLong() == 0 );
    REQUIRE( var6.toLongDouble() == 0 );
    REQUIRE( var6.toString() == "42.561651651561651641654156165156165414265465465" );
    REQUIRE( var6.toBool() == true );



    LazyOrm::DbVariant var7 = "42561651651561651641654156165156165414265465465";
    REQUIRE( var7.toLongLong() == 0 );
    REQUIRE( var7.toULongLong() == 0 );
    REQUIRE( var7.toLongDouble() == 0);
    REQUIRE( var7.toString() == "42561651651561651641654156165156165414265465465" );
    REQUIRE( var7.toBool() == true );
    REQUIRE( var7.alterStringToBestMatchType().toLongLong() == 0 );
    REQUIRE( var7.alterStringToBestMatchType().toULongLong() == 0 );
    REQUIRE( var7.alterStringToBestMatchType().toLongDouble() == 0 );
    REQUIRE( var7.alterStringToBestMatchType().toString() == "42561651651561651641654156165156165414265465465" );
    REQUIRE( var7.alterStringToBestMatchType().toBool() == true );
    REQUIRE( var7.toLongLong() == 0 );
    REQUIRE( var7.toULongLong() == 0 );
    REQUIRE( var7.toLongDouble() == 0 );
    REQUIRE( var7.toString() == "42561651651561651641654156165156165414265465465" );
    REQUIRE( var7.toBool() == true );


    LazyOrm::DbVariant var8 = "42.561651651561651641654156165156165414265465465";
    var8.alterStringToNumber();
    REQUIRE( var8.toLongLong() == 42 );
    REQUIRE( var8.toULongLong() == 42 );
    REQUIRE( var8.toLongDouble() == Catch::Approx(42.5617).epsilon(0.0001));
    REQUIRE( var8.toString() ==  "42.561652" );
    REQUIRE( var8.toBool() == true );
    REQUIRE( var8.alterStringToBestMatchType().toLongLong() == 42 );
    REQUIRE( var8.alterStringToBestMatchType().toULongLong() == 42 );
    REQUIRE( var8.alterStringToBestMatchType().toLongDouble() == Catch::Approx(42.5617).epsilon(0.0001) );
    REQUIRE( var8.alterStringToBestMatchType().toString() ==  "42.561652" );
    REQUIRE( var8.alterStringToBestMatchType().toBool() == true );
    REQUIRE( var8.toLongLong() == 42 );
    REQUIRE( var8.toULongLong() == 42 );
    REQUIRE( var8.toLongDouble() == Catch::Approx(42.5617).epsilon(0.0001) );
    REQUIRE( var8.toString() ==  "42.561652" );
    REQUIRE( var8.toBool() == true );

    LazyOrm::DbVariant var9 = "-42.561651651561651641654156165156165414265465465";
    var9.alterStringToNumber();
    REQUIRE( var9.toLongLong() == -42 );
    REQUIRE( var9.toULongLong() == 0 );
    REQUIRE( var9.toLongDouble() == Catch::Approx(-42.5617).epsilon(0.0001));
    REQUIRE( var9.toString() ==  "-42.561652" );
    REQUIRE( var9.toBool() == false );
    REQUIRE( var9.alterStringToBestMatchType().toLongLong() == -42 );
    REQUIRE( var9.alterStringToBestMatchType().toULongLong() == 0 );
    REQUIRE( var9.alterStringToBestMatchType().toLongDouble() == Catch::Approx(-42.5617).epsilon(0.0001) );
    REQUIRE( var9.alterStringToBestMatchType().toString() ==  "-42.561652" );
    REQUIRE( var9.alterStringToBestMatchType().toBool() == false );
    REQUIRE( var9.toLongLong() == -42 );
    REQUIRE( var9.toULongLong() == 0 );
    REQUIRE( var9.toLongDouble() == Catch::Approx(-42.5617).epsilon(0.0001) );
    REQUIRE( var9.toString() ==  "-42.561652" );
    REQUIRE( var9.toBool() == false );

    LazyOrm::DbVariant var10 = "-42561651651561651641654156165156165414265465465";
    var10.alterStringToNumber();
    REQUIRE( var10.toLongLong() == 0 );
    REQUIRE( var10.toULongLong() == 0 );
    REQUIRE( var10.toLongDouble() == Catch::Approx(-4.25617e+46).epsilon(0.0001));
    REQUIRE( var10.toString() == "-42561651651561651641963300908244742187757076480" );
    REQUIRE( var10.toBool() == false );
    REQUIRE( var10.alterStringToBestMatchType().toLongLong() == 0 );
    REQUIRE( var10.alterStringToBestMatchType().toULongLong() == 0 );
    REQUIRE( var10.alterStringToBestMatchType().toLongDouble() == Catch::Approx(-4.25617e+46).epsilon(0.0001) );
    REQUIRE( var10.alterStringToBestMatchType().toString() == "-42561651651561651641963300908244742187757076480" );
    REQUIRE( var10.alterStringToBestMatchType().toBool() == false );
    REQUIRE( var10.toLongLong() == 0 );
    REQUIRE( var10.toULongLong() == 0 );
    REQUIRE( var10.toLongDouble() == Catch::Approx(-4.25617e+46).epsilon(0.0001) );
    REQUIRE( var10.toString() == "-42561651651561651641963300908244742187757076480" );
    REQUIRE( var10.toBool() == false );
}
