#include "kiss_posix_time_utils.hpp"

/*

This code was initially strongly inspired from Michael Margolis code, so we reproduce his license under:

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

bool is_leap_year(uint16_t const year_number)
{
    return (year_number % 4 == 0) && (!(year_number % 100 == 0) || (year_number % 400 == 0));
}

kiss_time_t calendar_to_posix(kiss_calendar_time const * const calendar_in){
    kiss_time_t seconds;

    ////////////////////////////////////////////////////////////
    // start by computing seconds from EPOCH_START until 1 jan 00:00:00 of the given year
    // this is the number of days times seconds per day for the years until the previous year, included
    uint16_t year_minus_1 = static_cast<uint16_t>(calendar_in->year - 1);
    seconds = SECS_PER_DAY * static_cast<kiss_time_t>(
        year_minus_1 * 365 + year_minus_1 / 4 - year_minus_1 / 100 + year_minus_1 / 400
        - 719162  // the value we would get for the expression starting at year 0 instead of 1970
    );
    
    ////////////////////////////////////////////////////////////
    // add all the days for the months fully elapsed in this year, months start from 1
    if (is_leap_year(calendar_in->year)){
        seconds += cumulative_days_per_month_leap[calendar_in->month - 1] * SECS_PER_DAY;
    }
    else{
        seconds += cumulative_days_per_month_normal[calendar_in->month - 1] * SECS_PER_DAY;
    }

    ////////////////////////////////////////////////////////////
    // the easy part :) seconds due to day, hour, minute, second
    seconds += static_cast<kiss_time_t>(calendar_in->day - 1) * SECS_PER_DAY;
    seconds += calendar_in->hour * SECS_PER_HOUR;
    seconds += calendar_in->minute * SECS_PER_MIN;
    seconds += calendar_in->second;

    return seconds;
}

void posix_to_calendar(kiss_time_t const posix_in, kiss_calendar_time *const calendar_out)
{
    kiss_time_t time; // time has a "changing unit" in the following, secs -> mins -> hrs -> days...

    ////////////////////////////////////////////////////////////
    // the easy part: minutes, seconds, hours
    time = posix_in; // time now is in seconds
    calendar_out->second = static_cast<uint8_t>( time % 60 );
    time /= 60; // now it is minutes
    calendar_out->minute = static_cast<uint8_t>( time % 60 );
    time /= 60; // now it is hours
    calendar_out->hour = static_cast<uint8_t>( time % 24 );
    time /= 24; // now it is days
    // calendar_out->week_day_start_monday = static_cast<uint8_t>( ((time + 3) % 7) + 1 ); // Monday is day 1

    ////////////////////////////////////////////////////////////
    // find which year we are in
    uint16_t year {EPOCH_START};
    uint32_t nbr_days_in_current_year {0};
    // count cumulative number of days per year until we overshoot the number of days we look for
    while (true){
        nbr_days_in_current_year = is_leap_year(year) ? days_leap_year : days_normal_year;
        if (nbr_days_in_current_year <= time){
            year = static_cast<uint16_t>(year + 1);
            time -= nbr_days_in_current_year;
        }
        else{
            break;
        }
    }
    calendar_out->year = year;

    ////////////////////////////////////////////////////////////
    // now we need to find out the month we are in

    // are we currently in a leap or non leap year, and how many days per month for us?
    uint8_t const (* days_per_month)[12];
    if (is_leap_year(year)){
        days_per_month = &days_per_month_leap;
    }
    else{
        days_per_month = &days_per_month_normal;
    }

    // crrt nbr of days in month
    uint8_t crrt_nbr_days {0};

    // find out which month we are in, and how many days are left
    for (uint8_t month=1; month<=12; month++)
    {
        crrt_nbr_days = (*days_per_month)[month-1];
        if (time >= crrt_nbr_days){
            time -= crrt_nbr_days;
        }
        else{  // going one month further overshoots: this is the month we are in
            calendar_out->month = month; // jan is month 1, already taken care of above [we start at 1]
            break;
        }
    }

    ////////////////////////////////////////////////////////////
    // now days left are days of the month
    calendar_out->day = static_cast<uint8_t>( time + 1 );    // day of month, starts at 1 not 0
}