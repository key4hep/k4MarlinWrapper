#include <catch2/catch_all.hpp>

#include "k4MarlinWrapper/converters/EDM4hep2Lcio.h"

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}

TEST_CASE( "Parameter sorting", "[parameters]") {
    std::vector<std::string> fake_data1 = {"SimTrackerHit", "hits_VXD_barrel", "hits_VXD_barrel_LCIO"};

    REQUIRE(  )
}