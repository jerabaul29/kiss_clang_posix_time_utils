#!/bin/bash
set -e

# Just a simple script to build all tests, run them, show the results, and clean up.

# TODO
# consider if moving to some form of make or cmake may be cleaner

# warning flags
WFLAGS="-pedantic -Wall -Wextra -Werror -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused -Wconversion -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -fno-common -std=c++1z -Wfloat-conversion"

echo " "
echo "--------------------"
echo "compile all tests"

g++ $WFLAGS -o test_suite.out main.cpp test*.cpp ../src/kiss_posix_time_utils.cpp ../src/kiss_posix_time_extras.cpp

echo " "
echo "--------------------"
echo "run tests"
echo " "
./test_suite.out --durations yes --verbosity normal

echo "--------------------"
echo "cleanup tests"
rm ./test_suite.out

echo " "
