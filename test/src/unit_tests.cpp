#include <catch2/catch_all.hpp>

#include <vector>
#include <string>

#include <iostream>

#include "k4MarlinWrapper/util/EDM4hep2LcioUtils.h"

TEST_CASE( "TestToolInputParameters", "[EDM4hep2Lcio]" ) {

    const std::vector<std::string> test1_original = {
        "SimTrackerHit", "hits_VXD_barrel", "hits_VXD_barrel_LCIO"};
    std::vector<std::string> test1 (test1_original);
    REQUIRE( test1_original.size() == test1.size() );

    k4MW::util::optimizeOrderParams(test1);

    for (int i=0; i < test1_original.size(); ++i) {
        REQUIRE( test1_original[i] == test1[i]);
    }


    const std::vector<std::string> test2_original = {
        "SimTrackerHit", "hits_VXD_barrel", "hits_VXD_barrel_LCIO",
        "TrackerHit", "th", "th_LCIO"};
    std::vector<std::string> test2 (test2_original);
    REQUIRE( test2_original.size() == test2.size() );

    k4MW::util::optimizeOrderParams(test2);

    for (int i=0; i < test2_original.size(); ++i) {
        REQUIRE( test2_original[i] == test2[i] );
    }


    const std::vector<std::string> test3_original = {
        "SimTrackerHit", "hits_VXD_barrel", "hits_VXD_barrel_LCIO",
        "MCParticle", "mcp", "mcp_LCIO"};
    std::vector<std::string> test3 (test3_original);
    REQUIRE( test3_original.size() == test3.size() );

    k4MW::util::optimizeOrderParams(test3);

    REQUIRE( test3_original[0] == test3[3] );
    REQUIRE( test3_original[1] == test3[4] );
    REQUIRE( test3_original[2] == test3[5] );
    REQUIRE( test3_original[3] == test3[0] );
    REQUIRE( test3_original[4] == test3[1] );
    REQUIRE( test3_original[5] == test3[2] );


    const std::vector<std::string> test4_original = {
        "Vertex", "v", "v_LCIO",
        "ReconstructedParticle", "rp", "rp_LCIO",
        "Track", "tr", "tr_LCIO",
        "TrackerHit", "trh", "trh_LCIO"};
    std::vector<std::string> test4 (test4_original);
    REQUIRE( test4_original.size() == test4.size() );

    k4MW::util::optimizeOrderParams(test4);

    REQUIRE( test4_original[0]  == test4[0]  );
    REQUIRE( test4_original[1]  == test4[1]  );
    REQUIRE( test4_original[2]  == test4[2]  );
    REQUIRE( test4_original[3]  == test4[6]  );
    REQUIRE( test4_original[4]  == test4[7]  );
    REQUIRE( test4_original[5]  == test4[8]  );
    REQUIRE( test4_original[6]  == test4[9]  );
    REQUIRE( test4_original[7]  == test4[10] );
    REQUIRE( test4_original[8]  == test4[11] );
    REQUIRE( test4_original[9]  == test4[3]  );
    REQUIRE( test4_original[10] == test4[4]  );
    REQUIRE( test4_original[11] == test4[5]  );
}