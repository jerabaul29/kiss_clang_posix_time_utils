#ifndef KISS_POSIX_TIME_UTILS
#define KISS_POSIX_TIME_UTILS

// TODO: 
// a few possible improvements:
// - make "pure C compatible"
// - simplify the interface: use ony "natural" kiss_calendar_time values
// - functions to get the day month weekday as a c-style string
// - for tests, use 1st 1970, some random days in 2000, 2020, 2021, 2036, 2040, 2060
// - for tests, run N random tests against the default c / cpp implementation

#include <cstdint>

/*

The aim of this library is to provide a self contained, minimalistic way of performing conversions
back and from posix time and calendar time. In the following, we only consider dates that come
after the unix epoch, ie date that are after the start of 1970. We use the Gregorian calendar,
counting leap seconds. If you need dates before 1st Jan 1970, use some other library! However, we
do use 64-bits posix timestamps, so this will continue to work after the Y2k38 overflow problem.

This code is very strongly inspired from Michael Margolis code, so we reproduce his license under:

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

*/

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// low level data structures

// the usual posix epoch time since 1970
// we use 64 bits to avoid any problem with the Y2k38 32-bits overflow
using kiss_time_t = uint64_t;

// struct representing calendar date and time
// hour, minute, second is as expected: starting at 0, going up to 23, 59, 59
// week_day_starting_monday is 0 for monday, 1 for tuesday, etc...
// day is day of the month, in "natural" convention, i.e. 1 is the first day of the month
// month is in "natural" convention, i.e. 1 is january, 2 february, etc...
// year is the "full" year, i.e. 2021, 2022, etc...
// full example: second:12, minute:5, hour:14, day:12th, month:february:2, year:2023, week_day_start_monday:sunday:7
struct kiss_calendar_time
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t week_day_start_monday; // day of week, sunday is day 1
    uint8_t day;
    uint8_t month;
    uint16_t year;
};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// constants

#define SECS_PER_MIN  ((kiss_time_t)(60UL))
#define SECS_PER_HOUR ((kiss_time_t)(3600UL))
#define SECS_PER_DAY  ((kiss_time_t)(86400UL))
#define SECS_PER_YEAR ((kiss_time_t)(31536000UL))

// how many days per month, for non leap years (leap years would have a number of days of
// 29 in february); leap year, and february leap years, will be taken care of separately.
static const uint8_t days_per_month[] =
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// functions

// Is the current year a leap year, in the Gregorian calendar?
// In the Gregorian calendar, each leap year has 366 days instead of 365,
// by extending February to 29 days rather than the common 28.
// These extra days occur in each year which is an integer multiple of 4,
// except for years evenly divisible by 100, but not by 400.
bool is_leap_year(int const year);

// given a calendar_in, compute the corresponding posix time
// note that the calendar_in must follow the conventions used for the kiss_calendar_time struct,
// see above
kiss_time_t calendar_to_posix(kiss_calendar_time const *const calendar_in);

// given a posix time, compute the corresponding calendar_time
// note that kiss_calendar_time struct follows a few specific conventions,
// see above
void posix_to_calendar(kiss_time_t const posix_in, kiss_calendar_time *const calendar_out);

#endif