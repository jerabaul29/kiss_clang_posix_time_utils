#include "kiss_posix_time_utils.hpp"

/*

This code was initially strongly inspired from Michael Margolis code, so we reproduce his license under;
the current version has evolved quite a bit, though :) :

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

I also include here the adapted Oryx implementation, from: https://github.com/Oryx-Embedded/Common/blob/master/date_time.c
These look like magics, but are actually not faster or slightly slower than my implementations, so I keep my implementations
as they are easier to understand... But still, the Oryx stuff looks so fun that I keep it here "just for the records".
Another thing is that the Oryx implementation actually does not need any pre-computed table of days per month and
cumulated days per month. In case flash is really limited, may be helpful.

*/

// 1 to use my implementation, 0 to use Oryx
#define USE_JR_IMPLEMENTATION 1

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// functions

bool is_leap_year(uint16_t const year_number)
{
    return (year_number % 4 == 0) && (!(year_number % 100 == 0) || (year_number % 400 == 0));
}


#if USE_JR_IMPLEMENTATION

    // my own readable (according to me :) ) implementations
    
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

        // first try to eyeball
        // this is not mandatory, can turn on or off
        // on my laptop (but, of course, this is not a MCU, and results may differ from platform to platform),
        // using this eyeballing has a large impact on performace (about twice as fast)
        // TODO: would be fun to check performance effect on a MCU :)
        #if 1
            year = static_cast<uint16_t>(year + time / days_leap_year);
            uint16_t year_minus_1 = static_cast<uint16_t>(year - 1);
            time -= static_cast<kiss_time_t>(
                year_minus_1 * 365 + year_minus_1 / 4 - year_minus_1 / 100 + year_minus_1 / 400
                - 719162  // the value we would get for the expression starting at year 0 instead of 1970
            );
        #endif

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
        uint16_t const (* cumulative_days_per_month)[13];
        if (is_leap_year(year)){
            cumulative_days_per_month = &cumulative_days_per_month_leap;
        }
        else{
            cumulative_days_per_month = &cumulative_days_per_month_normal;
        }

        // find out which month we are in, and how many days are left
        for (uint8_t month=1; month<=12; month++)
        {
            if (time < (*cumulative_days_per_month)[month]){
                time = time - (*cumulative_days_per_month)[month-1];
                calendar_out->month = month; // jan is month 1, already taken care of above [we start at 1]
                break;
            }
        }

        ////////////////////////////////////////////////////////////
        // now days left are days of the month
        calendar_out->day = static_cast<uint8_t>( time + 1 );    // day of month, starts at 1 not 0
    }

#else

    // the Oryx implementations

    // calendar to posix, with modulo magics, though relatively similar to mine...
    // about the same speed as mine, and mince is easier to understand, so keep mine.
    kiss_time_t calendar_to_posix(kiss_calendar_time const * const calendar_in) {
    int y;
    int m;
    int d;
    kiss_time_t t;

    //Year
    y = calendar_in->year;
    //Month of year
    m = calendar_in->month;
    //Day of month
    d = calendar_in->day;

    //January and February are counted as months 13 and 14 of the previous year
    if(m <= 2)
    {
        m += 12;
        y -= 1;
    }

    //Convert years to days
    t = static_cast<kiss_time_t>( (365 * y) + (y / 4) - (y / 100) + (y / 400) );
    //Convert months to days
    t = t + static_cast<kiss_time_t>( (30 * m) + (3 * (m + 1) / 5) + d );
    //Unix time starts on January 1st, 1970
    t = t - 719561;
    //Convert days to seconds
    t *= 86400;
    //Add hours, minutes and seconds
    t += static_cast<kiss_time_t>( (3600 * calendar_in->hour) + (60 * calendar_in->minute) + calendar_in->second );

    //Return Unix time
    return t;
    }

    // posix to calendar, with lots of modulo magics, this time really incomprehensible to me - good luck finding
    // these expressions in the first place ^^ :)
    // funnily, this is slightly slower than my implementation above on my computer (though not clear how relevant for a MCU),
    // so keep my implementation :) . But this passes all tests, so this seems to be correct!
    void posix_to_calendar(kiss_time_t const posix_in, kiss_calendar_time *const calendar_out){
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
    kiss_time_t t {posix_in};

    //Retrieve hours, minutes and seconds
    calendar_out->second = static_cast<uint8_t>( t % 60 );
    t /= 60;
    calendar_out->minute = static_cast<uint8_t>( t % 60 );
    t /= 60;
    calendar_out->hour = static_cast<uint8_t>( t % 24 );
    t /= 24;

    //Convert Unix time to date
    a = static_cast<uint32_t>( (4 * t + 102032) / 146097 + 15 );
    b = static_cast<uint32_t>( t + 2442113 + a - (a / 4) );
    c = (20 * b - 2442) / 7305;
    d = b - 365 * c - (c / 4);
    e = d * 1000 / 30601;
    f = d - e * 30 - e * 601 / 1000;

    //January and February are counted as months 13 and 14 of the previous year
    if(e <= 13)
    {
        c -= 4716;
        e -= 1;
    }
    else
    {
        c -= 4715;
        e -= 13;
    }

    //Retrieve year, month and day
    calendar_out->year = static_cast<uint16_t>(c);
    calendar_out->month = static_cast<uint8_t>(e);
    calendar_out->day = static_cast<uint8_t>(f);
    }

#endif
