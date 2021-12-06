# kiss_posix_time_utils

Keep it stupid simple primitives for performing conversions between Posix time and calendar time, in CLang (C++ that would be quite easily transposable to C if needed).

## Why writing this library?

I wrote this small library for some embedded / Arduino projects I was working on. I could not find a simple, minimalistic, non-bloated, unit tested library for doing conversions between Posix time and Gregorian calendar time. In addition, I wanted something that I could i) include easily in any Arduino / PlatformIO project, ii) that did not have any dynamic allocation.

This is a KeepItStupidSimple library: I go for a clean, easy-to-read, simple implementation rather than trying to tweak stuff for performance. While I try to keep the code KISS, I still try to use some reasonably efficient algorithms though - if you know about some much better algorithms to do the same thing, let me know :) .

The closest I found was the Timelib library at https://github.com/PaulStoffregen/Time . However, I was a bit frustrated by i) the mix of the core Posix - Gregorian primitives with quite a bit of application-specific code for timekeeping on MCUs, synchronization with time sources, etc, ii) the conventions used in the Gregorian calendar data struct, iii) the absence of unit testing and the many MCU specific macros, define etc that are not too relevant for me any longer when working on modern ARM-based MCUs.

## Goals

- Convert back and forth between Posix timestamp and Gregorian calendar dates and times. This means that we use Posix time, i.e.:
  - epoch start is 1st January 1970 at 00:00,
  - we take into account leap years,
  - we do not take into account leap seconds.
- Use a modern, intuitive representation of Gregorian calendar dates and times:
  - use full year number, not just offset from 1970,
  - use "natural" conventions for numbering of days and months (both start at 1 instead of 0),
  - provide a simple API.

We separate the "really core functions", i.e. conversions from and to Gregorian calendar / Posix timestamp, from the "extra functionalities".

## Installation

This should be usable as-is in an Arduino or PlatformIO project: just clone the repo in the library folder corresponding to your case.

For other uses, there are simply a couple of files in the **src** folder to use.

## Example

See the tests for more details, but a simple example of the core functionalities:

```cpp
#include "kiss_posix_time_utils.hpp"

// checking if a year is a leap year
bool is_2000_leap_year = is_leap_year(2000);

// converting from Gregorian calendar to Posix time
// Gregorian calendar for 14th 2:February 2021, at 16:32:04
kiss_calendar_time calendar_time {2021, 2, 14, 16, 32, 4};
kiss_time_t corresponding_posix = calendar_to_posix(&calendar_time);

// converting from Posix time to Gregorian calendar
kiss_calendar_time calendar_time_out;
posix_to_calendar(3443943, &calendar_time_out);
```

## License

Made available under the MIT license: no guarantees whatsoever, but do whatever you want with the content of this repository.

## Tests

Tests are in the **tests** folder. For simplicity, the tests are run using *Catch2*, a cpp-lang framework for unit testing. To run all tests, just run the **tests/script_compile_run_tests.sh**. Unit testing happens with quite aggressive flags, for example, unintended type conversions should be treated as an error.
