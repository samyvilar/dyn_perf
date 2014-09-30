
#ifndef __TIMED__
#define __TIMED__

#include <time.h>

#define timed(func, args...) ({                             \
    clock_t _timed_start, _end_timed;                       \
    _timed_start = clock();                                 \
    func(args);                                             \
    _end_timed = clock();                                   \
    ((double)(_end_timed - _timed_start))/CLOCKS_PER_SEC;   \
})

#endif