#include <iostream>
#include "millis_overflow.h"

using namespace std;

int main()
{
    unsigned long previous = 4294917296;
    unsigned long current = 4294917296;

    while(!chk_overflow(current,previous))
    {
	cout << "Previous: " << previous << endl;
	cout << "Current: " << current << endl;
	current++;
	if(current == 4294967296)
	    current = 0;

	cout << "Difference: " << current - previous << endl;
    }

    cout << "Previous: " << previous << endl;
    cout << "Current: " << current << endl;
}
