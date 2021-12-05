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
// functions

bool is_leap_year(int const year_number)
{
    return (year_number % 4 == 0) && (!(year_number % 100 == 0) || (year_number % 400 == 0));
}

kiss_time_t calendar_to_posix(kiss_calendar_time const * const calendar_in){
    int i;
    uint64_t seconds;

    // start by computing seconds from 1970 until 1 jan 00:00:00 of the given year
    seconds = (calendar_in->year-1970) * SECS_PER_YEAR;  // no leap effect at this point ...
    // ... get back the leaped seconds
    for (i=1970; i < calendar_in->year; i++)
    {
        if (is_leap_year(i))
        {
            seconds += SECS_PER_DAY; // add extra day
        }
    }

    // add all the days for the months fully elapsed in this year, months start from 1
    for (i = 1; i < calendar_in->month; i++)
    {
        // do we have a leap year february?
        if ((i == 2) && is_leap_year(calendar_in->year))
        {
            seconds += SECS_PER_DAY * 29;
        }
        else  // this is a normal month
        {
            seconds += SECS_PER_DAY * days_per_month[i - 1]; // days_per_month array start index is 0
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
    uint16_t year;
    uint8_t month, monthLength;
    uint64_t time; // time has a "changing unit" in the following...
    uint32_t days;

    time = (uint64_t)posix_in; // time in seconds
    calendar_out->second = time % 60;
    time /= 60; // now it is minutes
    calendar_out->minute = time % 60;
    time /= 60; // now it is hours
    calendar_out->hour = time % 24;
    time /= 24; // now it is days
    calendar_out->week_day_start_monday = ((time + 3) % 7) + 1; // Monday is day 1

    // find how many years have elapsed
    year = 1970;
    days = 0;
    while ((unsigned)(days += (is_leap_year(year) ? 366 : 365)) <= time)
    {
        year++;
    }
    calendar_out->year= year;

    // we have already counted the full number of days in the current year;
    // reduce by the number of days in the current year
    days -= is_leap_year(year) ? 366 : 365;
    time -= days; // now it is days in this year, starting at 0

    // now we need to find out the month and day from the days in the year
    for (month=1; month<=12; month++)
    {

        // find out how many days in the current month
        if (month == 2)
        { // february
            if (is_leap_year(year))
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
            monthLength = days_per_month[month-1];
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
    calendar_out->month = month; // jan is month 1, already taken care of above
    calendar_out->day = time + 1;    // day of month, starts at 1 not 0
}
