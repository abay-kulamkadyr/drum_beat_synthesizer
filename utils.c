#include "utils.h"
#include <time.h>
#include <errno.h>
/*
Adapted from:
https://stackoverflow.com/questions/1157209/is-there-an-alternative-sleep-function-in-c-to-milliseconds
*/
void sleep_ms(unsigned int delayMs)
{
	struct timespec ts;
    int res;
	
    if (delayMs < 0)
    {
        errno = EINVAL;
        return;
    }

    ts.tv_sec = delayMs / 1000;
    ts.tv_nsec = (delayMs % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

}