#include "time.h"

unsigned long long systime() {
    struct timeval tv;

    if (gettimeofday(&tv, NULL) < 0) {
        panic("Unable to get system time!");
    }

    return tv.tv_sec;
}

unsigned long long sysutime() {
    struct timeval tv;

    if (gettimeofday(&tv, NULL) < 0) {
        panic("Unable to get system time!");
    }

    return tv.tv_sec * 1000000 + tv.tv_usec;
}
