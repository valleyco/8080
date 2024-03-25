#include "misc.h"
#include <time.h>

int64_t get_hr_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ts.tv_nsec * 1e-3 + ts.tv_sec;
}