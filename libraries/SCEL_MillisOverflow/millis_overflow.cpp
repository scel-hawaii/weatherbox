#include "millis_overflow.h"

#define TRUE 1
#define FALSE 0

int chk_overflow(unsigned long uptime, unsigned long previous_time)
{
    if(uptime < previous_time)
        return TRUE;
    else
	return FALSE;
}
