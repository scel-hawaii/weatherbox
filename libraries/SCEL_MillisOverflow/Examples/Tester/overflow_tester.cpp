#include <iostream>
#include "overflow_checker.h"

using namespace std;

/**
 * This tester checks that the chk_overflow() function properly works,
 * given that the checks between the previous and current times are
 * done at reasonable intervals. In the case of this tester code, the 
 * increment is determined by how much is added to the current variable.
 *
 * To understand what a reasonable increment is, check 'overflow_checker.cpp'.
 */
int main()
{
    int previous = 21;
    int current = previous;

    while(!chk_overflow(current,previous))
    {
	current = current + 22;
    }

    cout << "OVERFLOW\n" << endl;

    cout << "Previous: " << previous << endl;
    cout << "Current: " << current << endl;
}
