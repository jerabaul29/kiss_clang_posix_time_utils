#include "catch.hpp"
#include "../src/kiss_posix_time_utils.hpp"
#include <stdlib.h>

TEST_CASE("is_leap_year"){
    // check that lap years work fine

    // a few normal leap years
    REQUIRE( is_leap_year(2004) );
    REQUIRE( is_leap_year(2020) );
    REQUIRE( is_leap_year(2024) );

    // a few normal non leap years
    REQUIRE( !is_leap_year(2001) );
    REQUIRE( !is_leap_year(2022) );
    REQUIRE( !is_leap_year(2023) );
    REQUIRE( !is_leap_year(2025) );

    // a few non normal non leap years
    REQUIRE( !is_leap_year(1800) );
    REQUIRE( !is_leap_year(1900) );
    REQUIRE( !is_leap_year(2100) );
    REQUIRE( !is_leap_year(2200) );

    // a few non normal leap years
    REQUIRE( is_leap_year(2000) );
    REQUIRE( is_leap_year(2400) );
}

TEST_CASE("calendar_to_posix"){
    // a number of specific test cases that we can go from calendar to posix
    // since we check below that calendar to posix and posix to calendar are inverse operation of
    // each other, that should be enough to fully validate both functions.

    kiss_calendar_time working_calendar;
    kiss_time_t working_time;

    // at around my time
    working_time = 1638795207;
    working_calendar = {2021, 12, 6, 12, 53, 27};
    REQUIRE( calendar_to_posix(&working_calendar) == working_time );

    // back to the 70s! :)
    working_time = 40003264;
    working_calendar = {1971, 4, 9, 0, 1, 4};
    REQUIRE( calendar_to_posix(&working_calendar) == working_time );

    // around year 2000
    working_time = 951782464;
    working_calendar = {2000, 2, 29, 0, 1, 4};
    REQUIRE( calendar_to_posix(&working_calendar) == working_time );

    // by 2190, I will likely be long dead
    working_time = 6952953588;
    working_calendar = {2190, 4, 30, 23, 59, 48};
    REQUIRE( calendar_to_posix(&working_calendar) == working_time );

    // but who knows in 2090?
    working_time = 3801738768;
    working_calendar = {2090, 6, 21, 14, 32, 48};
    REQUIRE( calendar_to_posix(&working_calendar) == working_time );

    // second second of a year
    working_time = 1609459201;
    working_calendar = {2021, 1, 1, 0, 0, 1};
    REQUIRE( calendar_to_posix(&working_calendar) == working_time );

    // first second of a year
    working_time = 1609459200;
    working_calendar = {2021, 1, 1, 0, 0, 0};
    REQUIRE( calendar_to_posix(&working_calendar) == working_time );

    // last second of a year
    working_time = 1640995199;
    working_calendar = {2021, 12, 31, 23, 59, 59};
    REQUIRE( calendar_to_posix(&working_calendar) == working_time );

    // last second of a month
    working_time = 1614556799;
    working_calendar = {2021, 2, 28, 23, 59, 59};
    REQUIRE( calendar_to_posix(&working_calendar) == working_time );

    // first second of a month
    working_time = 1614556800;
    working_calendar = {2021, 3, 1, 0, 0, 0};
    REQUIRE( calendar_to_posix(&working_calendar) == working_time );
}

// if we go posix_time -> calendar -> posix_time, do we get back to the same time?
// TODO: check in the middle that valid calendar!!
bool back_and_forth_is_equal(kiss_time_t const posix_time){
    kiss_calendar_time working_calendar;
    kiss_time_t result_time;

    posix_to_calendar(posix_time, &working_calendar);
    result_time = calendar_to_posix(&working_calendar);

    return result_time == posix_time;
}

TEST_CASE("back_and_forth_posix_calendar_posix"){
    // a few tests that we can go back and forth

    REQUIRE( back_and_forth_is_equal(100) );
    REQUIRE( back_and_forth_is_equal(1) );
    REQUIRE( back_and_forth_is_equal(123456) );
    REQUIRE( back_and_forth_is_equal(1638794479) );
    REQUIRE( back_and_forth_is_equal(3549848964) );
    REQUIRE( back_and_forth_is_equal(6952953588) );
}

// this is only going up to RAND_MAX, but should be fine enough...
// TODO: use propoer random C++11 to go to higher values!
kiss_time_t get_random_posix(void){
    return static_cast<kiss_time_t>(rand());
}

TEST_CASE("random_back_and_forth_posix_calendar_posix"){
    // a number of random tests that we can go back and forth

    size_t n_cases = 16777216;
    
    for (size_t i=0; i<n_cases; i++){
        REQUIRE( back_and_forth_is_equal(get_random_posix()) );
    }
}
