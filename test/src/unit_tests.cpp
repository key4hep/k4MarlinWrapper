#include <catch2/catch_all.hpp>

#include <vector>
#include <string>

#include <iostream>

#include "k4MarlinWrapper/util/EDM4hep2LcioUtils.h"

TEST_CASE( "TestToolInputParameters", "[EDM4hep2Lcio]" ) {

    auto checkNext3 = [](
        const std::vector<std::string>& original,
        const std::vector<std::string>& modified,
        const int orig_idx,
        const int mod_idx)
    {
        REQUIRE( original[orig_idx]   == modified[mod_idx]);
        REQUIRE( original[orig_idx+1] == modified[mod_idx+1]);
        REQUIRE( original[orig_idx+2] == modified[mod_idx+2]);
    };

    const std::vector<std::string> test1_original = {
        "SimTrackerHit", "hits_VXD_barrel", "hits_VXD_barrel_LCIO"};
    std::vector<std::string> test1 (test1_original);
    REQUIRE( test1_original.size() == test1.size() );

    k4MW::util::optimizeOrderParams(test1);

    checkNext3(test1_original, test1,  0,  0);


    const std::vector<std::string> test2_original = {
        "SimTrackerHit", "hits_VXD_barrel", "hits_VXD_barrel_LCIO",
        "TrackerHit", "th", "th_LCIO"};
    std::vector<std::string> test2 (test2_original);
    REQUIRE( test2_original.size() == test2.size() );

    k4MW::util::optimizeOrderParams(test2);

    checkNext3(test2_original, test2,  0,  0);
    checkNext3(test2_original, test2,  3,  3);


    const std::vector<std::string> test3_original = {
        "SimTrackerHit", "hits_VXD_barrel", "hits_VXD_barrel_LCIO",
        "MCParticle", "mcp", "mcp_LCIO"};
    std::vector<std::string> test3 (test3_original);
    REQUIRE( test3_original.size() == test3.size() );

    k4MW::util::optimizeOrderParams(test3);

    checkNext3(test3_original, test3,  0,  3);
    checkNext3(test3_original, test3,  3,  0);


    const std::vector<std::string> test4_original = {
        "Vertex", "v", "v_LCIO",
        "ReconstructedParticle", "rp", "rp_LCIO",
        "Track", "tr", "tr_LCIO",
        "TrackerHit", "trh", "trh_LCIO"};
    std::vector<std::string> test4 (test4_original);
    REQUIRE( test4_original.size() == test4.size() );

    k4MW::util::optimizeOrderParams(test4);

    checkNext3(test4_original, test4,  0,  0);
    checkNext3(test4_original, test4,  3,  9);
    checkNext3(test4_original, test4,  6,  6);
    checkNext3(test4_original, test4,  9,  3);


    const std::vector<std::string> test5_original = {
        "Vertex", "v", "v_LCIO",
        "ReconstructedParticle", "rp", "rp_LCIO",
        "Track", "tr", "tr_LCIO",
        "Track", "tr1", "tr1_LCIO",
        "SimTrackerHit", "sth", "sth_LCIO",
        "MCParticle", "mc", "mc_LCIO",
        "TrackerHit", "trh", "trh_LCIO",
        "MCParticle", "mc1", "mc1_LCIO",
        "TrackerHit", "trh1", "trh1_LCIO"};
    std::vector<std::string> test5 (test5_original);
    REQUIRE( test5_original.size() == test5.size() );

    k4MW::util::optimizeOrderParams(test5);

    checkNext3(test5_original, test5,  0,  0);
    checkNext3(test5_original, test5,  3, 24);
    checkNext3(test5_original, test5,  6,  9);
    checkNext3(test5_original, test5,  9, 18);
    checkNext3(test5_original, test5, 12, 21);
    checkNext3(test5_original, test5, 15, 12);
    checkNext3(test5_original, test5, 18,  3);
    checkNext3(test5_original, test5, 21, 15);
    checkNext3(test5_original, test5, 24,  6);

}