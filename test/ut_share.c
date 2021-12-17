#include <stdio.h>
#include <time.h>
#include <stdint.h>
 
void test_time_to_string(void)
{
    time_t epoch = 0;
    printf("%jd seconds since the epoch began\n", (intmax_t)epoch);
    printf("%s", asctime(gmtime(&epoch)));
}

/*
See also
time
 
returns the current calendar time of the system as time since epoch
(function)
localtime
localtime_r
localtime_s
  
(C23)
(C11)
 
converts time since epoch to calendar time expressed as local time
(function)
gmtime
gmtime_r
gmtime_s
  
(C23)
(C11)
 
converts time since epoch to calendar time expressed as Coordinated Universal Time (UTC)
(functi

https://en.cppreference.com/w/c/chrono/time_t
*/
