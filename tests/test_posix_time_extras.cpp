#include "catch.hpp"
#include "../src/kiss_posix_time_utils.hpp"
#include "../src/kiss_posix_time_extras.hpp"
#include <stdlib.h>
#include <string.h>

TEST_CASE("iso_formatting_short"){
    char working_buffer[20];
    kiss_calendar_time working_calendar;
    kiss_time_t working_time;

    // a normal case with short values that need padding
    working_calendar = {2021, 2, 3, 2, 4, 6};
    char correct_result[] = "2021-02-03T02:04:06";
    bool result = print_iso(&working_calendar, &working_buffer[0], 20);
    REQUIRE(result);
    REQUIRE( strncmp(working_buffer, correct_result, 20) == 0 );
    char bad_result[] = "2020-02-03T12:14:56"; // just to check that we use correctly the catch2 command
    REQUIRE( ! (strncmp(working_buffer, bad_result, 20) == 0) );

    working_time = calendar_to_posix(&working_calendar);
    char second_working_buffer[20] = "2021-02-03T02:04:06";
    bool second_result = print_iso(working_time, second_working_buffer, 20);
    REQUIRE(second_result);
    REQUIRE( strncmp(second_working_buffer, correct_result, 20) == 0 );
}

TEST_CASE("iso_formatting_long"){
    char working_buffer[20];
    kiss_calendar_time working_calendar;

    // a normal case with long values that do not need padding
    working_calendar = {2021, 12, 13, 12, 14, 16};
    char correct_result[] = "2021-12-13T12:14:16";
    bool result = print_iso(&working_calendar, &working_buffer[0], 20);
    REQUIRE(result);
    REQUIRE( strncmp(working_buffer, correct_result, 20) == 0 );

}

TEST_CASE("iso_formatting_too_short_buffer"){
    kiss_calendar_time working_calendar;

    // a buffer that is too short
    char too_short_working_buffer[19];
    working_calendar = {2021, 12, 13, 12, 14, 16};
    char too_short_result[19] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    bool result = print_iso(&working_calendar, &too_short_working_buffer[0], 19);
    REQUIRE(!result);
    REQUIRE( strncmp(too_short_result, too_short_working_buffer, 19) == 0 );
}