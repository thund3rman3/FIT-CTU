#ifndef __PROGTEST__
#include <stdio.h>
#include <assert.h>
#endif /* __PROGTEST__ */

int isLeap(int year)
{
    return (year % 4 == 0 && year % 100 != 0 && year % 4000 != 0) ||
           (year % 400 == 0 && year % 4000 != 0);
}

int dateToIndex(int day, int month, int year, int *idx)
{

    int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (isLeap(year))
        daysInMonth[1]++;

    if (month < 1 || month > 12 || year < 2000 || day < 1 || day > daysInMonth[month - 1])
        return 0;

    *idx = day;

    if (month > 1)
    {
        for (int i = 1; i < month; i++)
        {
            *idx += daysInMonth[i - 1];
        }
    }
    return 1;
}

#ifndef __PROGTEST__
int main(int argc, char *argv[])
{
    int idx;

    assert(dateToIndex(1, 1, 2000, &idx) == 1 && idx == 1);
    assert(dateToIndex(1, 2, 2000, &idx) == 1 && idx == 32);
    assert(dateToIndex(29, 2, 2000, &idx) == 1 && idx == 60);
    assert(dateToIndex(29, 2, 2001, &idx) == 0);
    assert(dateToIndex(1, 12, 2000, &idx) == 1 && idx == 336);
    assert(dateToIndex(31, 12, 2000, &idx) == 1 && idx == 366);
    assert(dateToIndex(1, 1, 1999, &idx) == 0);
    assert(dateToIndex(6, 7, 3600, &idx) == 1 && idx == 188);
    assert(dateToIndex(29, 2, 3600, &idx) == 1 && idx == 60);
    assert(dateToIndex(29, 2, 4000, &idx) == 0);

    return 0;
}
#endif /* __PROGTEST__ */
