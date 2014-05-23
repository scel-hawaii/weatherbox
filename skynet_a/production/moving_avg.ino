// averager used for averaging samples
double averager[15];

/***************************************************
 *  Name:        addToAverager
 *  Returns:     nothing
 *  Parameters:  add
 *  Description: Removes the oldest value and puts in
                 a new value to the averager to be averaged
		 with the rest of the previous values.
 ***************************************************/

void addToAverager(double add)
{
    int count = 0;

    for(count = 0; count < 14; count++)
    {
        averager[count] = averager[count+1];
    }

    averager[count] = add;
}

/***************************************************
 *  Name:        takeAverage
 *  Returns:     average of the averager
 *  Parameters:  none
 *  Description: Takes the average of the averager.
 ***************************************************/

double takeAverage(void)
{
    int count = 0;
    double sum = 0;

    for(count = 0; count < 15; count++)
    {
        sum += averager[count];
    }

    return sum/15;
}

/***************************************************
 *  Name:        initMovingAvg
 *  Returns:     none
 *  Parameters:  none
 *  Description: Initializes the averager to have no values.
 ***************************************************/

void initMovingAvg()
{
    int count = 0;

    for(count = 0; count < 15; count++)
    {
        averager[count] = 0;
    }
}

