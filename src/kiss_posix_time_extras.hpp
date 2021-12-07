#ifndef KISS_POSIX_TIME_EXTRAS
#define KISS_POSIX_TIME_EXTRAS

#include "kiss_posix_time_utils.hpp"

#ifndef ARDUINO
  #include <cstdio>
#endif

// print ISO8601 with second precision to buffer
// ie prints: 2020-03-20T14:28:23 to the buffer
// note that the buffer size must be at least 20 for having space for the terminating null byte
// return true if success, false if no success (for example, buffer too small)
bool print_iso(kiss_time_t const posix_in, char *const buffer_out, size_t const buffer_size);
bool print_iso(kiss_calendar_time const *const calendar_in, char *const buffer_out, size_t const buffer_size);

// TODO: implement all under (if there is some demand for it!)

// what is the current week day number associated with a calendar entry?
// 1 is monday, 2 is tuesday, ..., 7 is sunday
uint8_t day_of_week(kiss_time_t const posix_in);
uint8_t day_of_week(kiss_calendar_time const *const calendar_in);

// which week number are we within the current year?
// i.e. 1 is first week, 2 is second week, ..., 52 is 52nd
uint8_t week_of_year(kiss_time_t const posix_in);
uint8_t week_of_year(kiss_calendar_time const *const calendar_in);

// print full date in English convention to the buffer
// i.e. print: Wednesday 23 September 2021, 14:32:05
// need a buffer that is large enough, typically at least 38
// return true if success, false if no success (for example, buffer too small)
bool print_plaintext(kiss_time_t const posix_in, char *const buffer_out, size_t const buffer_size);
bool print_plaintext(kiss_calendar_time const *const calendar_in, char *const buffer_out, size_t const buffer_size);


#endif