#ifndef KISS_POSIX_TIME_UTILS
#define KISS_POSIX_TIME_UTILS

// TODO: 
// a few possible improvements:
// - put in a namespace
// - implement the additional stuff under

// Arduino is a very common use case for this, so let us make ready to import for it!
#ifdef ARDUINO
  #include "Arduino.h"
#else
  #include <cstdint>
  #include <stddef.h>
#endif

/*

The aim of this library is to provide a self contained, minimalistic way of performing conversions
to and from posix time and gregorian calendar time. In the following, we only consider dates that come
after the unix epoch, ie date that are after the start of 1970. We use the Gregorian calendar,
counting leap days, not counting leap seconds. If you need dates before 1st Jan 1970, use some other library!
However, we do use 64-bits posix timestamps, so this will continue to work after the Y2k38 overflow problem.

- This code has been taking inspiration from:

-- Michael Margolis code, so we reproduce his license under:
https://github.com/michaelmargolis/arduino_time

  time.c - low level time and date functions
  Copyright (c) Michael Margolis 2009-2014
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

-- Paul Stoffregen code:
https://github.com/PaulStoffregen/Time
*/

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// low level data structures

// the usual posix epoch: seconds elapsed since 1970, counting leap years, not counting leap seconds
// we use 64 bits to avoid any problem with the Y2k38 32-bits overflow for many, many years...
using kiss_time_t = uint64_t;

// struct representing calendar date and time in Gregorian calendar, using "natural" conventions,
// i.e. hour, minute, second is as expected: starting at 0, going up to 23, 59, 59,
// day is day of the month, in "natural" convention, i.e. 1 is the first day of the month, 2 second day, etc...,
// month is in "natural" convention, i.e. 1 is january, 2 february, etc...,
// year is the "full" year, i.e. 2021, 2022, etc... .
// example: second:12, minute:5, hour:14, day:12, month:february:2, year:2023 is: 2023-02-12T14:05:12
// this is different from the usual c standard library, but makes for easier use by humans
struct kiss_calendar_time
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// constants

static constexpr kiss_time_t SECS_PER_MIN  = 60;
static constexpr kiss_time_t SECS_PER_HOUR = 3600;
static constexpr kiss_time_t SECS_PER_DAY  = 86400;
static constexpr kiss_time_t SECS_PER_YEAR = 31536000;

static constexpr uint16_t EPOCH_START = 1970;

static constexpr uint32_t days_normal_year = 365;
static constexpr uint32_t days_leap_year   = 366;

static constexpr uint8_t days_per_month_normal[] =
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static constexpr uint8_t days_per_month_leap[] =
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static constexpr uint16_t cumulative_days_per_month_normal[] =
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
static constexpr uint16_t cumulative_days_per_month_leap[] =
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

// TODO: move to additional functionalities
// longest is 9 chars
static char const *const day_names[] = {
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday",
  "Sunday"
};

// longest is 9 chars
static char const *const month_names[] = {
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// functions

// Is the current year a leap year, in the Gregorian calendar?
// In the Gregorian calendar, each leap year has 366 days instead of 365,
// by extending February to 29 days rather than the common 28.
// These extra days occur in each year which is an integer multiple of 4,
// except for years evenly divisible by 100, but not by 400.
bool is_leap_year(uint16_t const year);

// given a calendar_in, compute the corresponding posix time
// note that the calendar_in must follow the conventions used for the kiss_calendar_time struct,
// see above
kiss_time_t calendar_to_posix(kiss_calendar_time const *const calendar_in);

// given a posix time, compute the corresponding calendar_time
// note that kiss_calendar_time struct follows a few specific conventions,
// see above
void posix_to_calendar(kiss_time_t const posix_in, kiss_calendar_time *const calendar_out);

// TODO: implement and test all below

// is the current calendar a valid calendar entry?
bool calendar_is_valid(kiss_calendar_time const *const calendar_in);

// TODO: put it in an "extra functionalities" file

// what is the current week day number associated with a calendar entry?
// 1 is monday, 2 is tuesday, ..., 7 is sunday
uint8_t day_of_week(kiss_time_t const posix_in);
uint8_t day_of_week(kiss_calendar_time const *const calendar_in);

// which week number are we within the current year?
// i.e. 1 is first week, 2 is second week, ..., 52 is 52nd
uint8_t week_of_year(kiss_time_t const posix_in);
uint8_t week_of_year(kiss_calendar_time const *const calendar_in);

// print ISO8601 with second precision to buffer
// ie prints: 2020-03-20T14:28:23 to the buffer
// note that the buffer size must be at least 20 for having space for the terminating null byte
// return true if success, false if no success (for example, buffer too small)
bool print_iso(kiss_time_t const posix_in, char *const buffer_out, size_t const buffer_size);
bool print_iso(kiss_calendar_time const *const calendar_in, char *const buffer_out, size_t const buffer_size);

// print full date in English convention to the buffer
// i.e. print: Wednesday 23 September 2021, 14:32:05
// need a buffer that is large enough, typically at least 38
// return true if success, false if no success (for example, buffer too small)
bool print_plaintext(kiss_time_t const posix_in, char *const buffer_out, size_t const buffer_size);
bool print_plaintext(kiss_calendar_time const *const calendar_in, char *const buffer_out, size_t const buffer_size);


#endif
