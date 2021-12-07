#include "kiss_posix_time_extras.hpp"

bool print_iso(kiss_calendar_time const *const calendar_in, char *const buffer_out, size_t const buffer_size){
    // check that we have a buffer large enough for all uses; if not, return false and fill with null bytes
    if (buffer_size < 20){
        for (size_t i=0; i<buffer_size; i++){
            buffer_out[i] = '\0';
        }
        return false;
    }

    // start filling... this is easy, the format is completely fixed
    snprintf(&buffer_out[0], 4+1, "%04u", calendar_in->year);
    buffer_out[4] = '-';
    snprintf(&buffer_out[5], 2+1, "%02u", calendar_in->month);
    buffer_out[7] = '-';
    snprintf(&buffer_out[8], 2+1, "%02u", calendar_in->day);
    buffer_out[10] = 'T';
    snprintf(&buffer_out[11], 2+1, "%02u", calendar_in->hour);
    buffer_out[13] = ':';
    snprintf(&buffer_out[14], 2+1, "%02u", calendar_in->minute);
    buffer_out[16] = ':';
    snprintf(&buffer_out[17], 2+1, "%02u", calendar_in->second);

    // end with null byte always
    buffer_out[buffer_size-1] = '\0';

    return true;
}

bool print_iso(kiss_time_t const posix_in, char *const buffer_out, size_t const buffer_size){
    kiss_calendar_time working_calendar;
    posix_to_calendar(posix_in, &working_calendar);
    bool result = print_iso(&working_calendar, buffer_out, buffer_size);
    return result;
}
