#include "kiss_posix_time_utils.hpp"

/*

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
// low level functional interface

kiss_time_t calendar_to_posix(kiss_calendar_time const * const calendar_in){
    int i;
    uint64_t seconds;

    // seconds from 1970 until 1 jan 00:00:00 of the given year
    seconds = calendar_in->years_since_1970 * SECS_PER_YEAR;  // no leap effect
    // get back the leaped seconds
    for (i = 0; i < calendar_in->years_since_1970; i++)
    {
        if (is_leap_year(i+1970))
        {
            seconds += SECS_PER_DAY; // add extra day
        }
    }

    // add days for this year, months start from 1
    for (i = 1; i < calendar_in->month; i++)
    {
        if ((i == 2) && is_leap_year(calendar_in->years_since_1970+1970))
        {
            seconds += SECS_PER_DAY * 29;
        }
        else
        {
            seconds += SECS_PER_DAY * days_per_month[i - 1]; // monthDay array starts from 0
        }
    }
    seconds += (calendar_in->day - 1) * SECS_PER_DAY;
    seconds += calendar_in->hour * SECS_PER_HOUR;
    seconds += calendar_in->minute * SECS_PER_MIN;
    seconds += calendar_in->second;
    return (kiss_time_t)seconds;
}

void posix_to_calendar(kiss_time_t const posix_in, kiss_calendar_time *const calendar_out)
{
    uint8_t year;
    uint8_t month, monthLength;
    uint64_t time; // time has a "changing unit"
    unsigned long days;

    time = (uint64_t)posix_in; // time in seconds
    calendar_out->second = time % 60;
    time /= 60; // now it is minutes
    calendar_out->minute = time % 60;
    time /= 60; // now it is hours
    calendar_out->hour = time % 24;
    time /= 24; // now it is days
    calendar_out->week_day_start_sunday = ((time + 4) % 7) + 1; // Sunday is day 1

    // find how many years have elapsed
    year = 0;
    days = 0;
    while ((unsigned)(days += (is_leap_year(year+1970) ? 366 : 365)) <= time)
    {
        year++;
    }
    calendar_out->years_since_1970 = year;

    // we have already counted the full number of days in the current year;
    // reduce by the number of days in the current year
    days -= is_leap_year(year+1970) ? 366 : 365;
    time -= days; // now it is days in this year, starting at 0

    days = 0;
    month = 0;
    monthLength = 0;
    for (month = 0; month < 12; month++)
    {

        // find out how many days in the current month
        if (month == 1)
        { // february
            if (is_leap_year(year+1970))
            {
                monthLength = 29;
            }
            else
            {
                monthLength = 28;
            }
        }
        else
        {
            monthLength = days_per_month[month];
        }

        // are we going over the months?
        if (time >= monthLength)
        {
            time -= monthLength;
        }
        else
        {
            break;
        }
    }
    calendar_out->month = month + 1; // jan is month 1
    calendar_out->day = time + 1;    // day of month
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// higher level functional interface

// In the Gregorian calendar, each leap year has 366 days instead of 365,
// by extending February to 29 days rather than the common 28.
// These extra days occur in each year which is an integer multiple of 4,
// except for years evenly divisible by 100, but not by 400.
bool is_leap_year(int year_number)
{
    return (year_number % 4 == 0) && (!(year_number % 100 == 0) || (year_number % 400 == 0));
}
