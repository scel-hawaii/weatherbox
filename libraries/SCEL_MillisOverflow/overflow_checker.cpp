#include "overflow_checker.h"

#define TRUE 1
#define FALSE 0

/**
 * chk_overflow() compares a previous value to a current value it is given 
 * and if the current value is less than the previous value, there was an overflow 
 * and the function returns it is true.
 *
 * NOTE: This function only works within reasonable periods of checking. If the current
 * value is incremented in a way that it exceeds the previous value, this function will
 * not detect the overflow.
 *
 * ie. If the previous value is 100 and the overflow happens at a value of 105, and the
 * value increments by 108, the current value would be 103. An overflow has occurred 
 * but chk_overflow() will not catch this because the current value is still greater
 * than the previous value.
 */
int chk_overflow(unsigned long current_value, unsigned long previous_value)
{
    if(current_value < previous_value)
        return TRUE;
    else
	return FALSE;
}
