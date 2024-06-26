#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#endif /* __PROGTEST__ */
#define number_of_months 12

// implementation of Zeller algorithm
int is_sunday(int day, int month, int year)
{
       if (month == 1 || month == 2)
       {
              month += 12;
              year--;
       }
       int x = year / 100; // first 2 digits of the year
       int y = year % 100; // last 2 digits of the year

       return (day + ((13 * (month + 1)) / 5) + y + (y / 4) + (x / 4) + (5 * x)) % 7;
}

/*Checks if year is leap
 *returns 1 if year is leap, 0 if it is not
 */
int is_leap(int year)
{
       return (year % 4 == 0 && year % 100 != 0 && year % 4000 != 0) ||
              (year % 400 == 0 && year % 4000 != 0);
}
// mezi roky
int get_year_rings(int y1, int y2, int rings_in_day, int rings_in_year)
{
       long long int rings = 0;
       for (y1 += 1; y1 < y2; y1++)
       {
              int i = 1;
              for (; i <= 7; i++)
              {
                     if (1 == is_sunday(i, 1, y1))
                            break;
              }
              int leap = is_leap(y1);
              rings += leap == 1 ? rings_in_day : 0;

              int sundays = 50;
              if ((i == 2 && leap == 1) || i == 1)
                     sundays += 3;
              else
                     sundays += 2;

              rings += rings_in_year - sundays * rings_in_day;
       }
       return rings;
}
// mezi hodiny
int get_rings_in_day(int h1, int h2, int i1)
{
       long long int rings = 0;
       if(i1 != 0 )
              h1++;
       for (; h1 <= h2; h1++)
       {
              if (h1 == 0 || h1 == 12)
                     rings += 12;
              else
                     rings += h1 % 12;
       }
       return rings;
}
// konec dne, mezi dny, konec dne
int get_rings_in_month(int d1, int d2, int h1, int h2, int i1, int i2, int rings_in_day, int m1, int m2, int y1, int y2)
{
       long long int rings = 0;
       if (is_sunday(d1, m1, y1) == 1)
              ;
       else
              rings += get_rings_in_day(h1, 23, i1);

       if (d2 - d1 >= 2)
       {
              for (d1 += 1; d1 < d2; d1++)
              {
                     if (is_sunday(d1, m1, y1) == 1)
                            ;
                     else
                     {
                            rings += rings_in_day;
                     }
              }
       }
       //  rings += (d2 - d1 - 1) * rings_in_day;

       if (is_sunday(d2, m2, y2) == 1)
              ;
       else
              rings += get_rings_in_day(0, h2, 0);

       return rings;
}

int rings_for_month_interval(int m1, int m2, int y, int month_lenght[], int rings_in_day)
{
       int sundays = 4;
       long long int rings = 0;
       int leap = is_leap(y);
       if (leap == 1)
              month_lenght[1]++;
       for (m1 += 1; m1 < m2; m1++)
       {
              int i = 1;
              for (; i <= month_lenght[m1 - 1]; i++)
              {
                     if (is_sunday(i, m1, y) == 1)
                            break;
              }
              rings += (month_lenght[m1 - 1] - sundays) * rings_in_day;
              if ((i == 1 && month_lenght[m1 - 1] >= 29) ||
                  (i == 2 && month_lenght[m1 - 1] >= 30) ||
                  (i == 3 && month_lenght[m1 - 1] == 31))
                     rings -= rings_in_day; // odectu 1 nedeli
       }
       if (leap == 1)
              month_lenght[1]--;
       return rings;
}

// konec dne, konec mesice, zacatek mesice, zacaek dne
int get_rings_in_year(int month_lenght[], int y1, int m1, int m2, int d1, int d2, int h1, int h2, int i1, int rings_in_day)
{
       long long int rings = 0;
       int leap = is_leap(y1);
       if (leap == 1)
              month_lenght[1]++;

       if (is_sunday(d1, m1, y1) == 1)
              ;
       else
              rings += get_rings_in_day(h1, 23, i1);

       for (d1 += 1; d1 <= month_lenght[m1 - 1]; d1++)
       {
              if (is_sunday(d1, m1, y1) == 1)
                     ;
              else
                     rings += rings_in_day;
       }

       for (int i = 1; i < d2; i++)
       {
              if (is_sunday(i, m2, y1) == 1)
                     ;
              else
                     rings += rings_in_day;
       }
       // rings += (d2 - 1) * rings_in_day;
       if (is_sunday(d2, m2, y1) == 1)
              ;
       else
              rings += get_rings_in_day(0, h2, 0);

       if (leap == 1)
              month_lenght[1]--;

       return rings;
}

// z==0 -dny do konce mesice, jinak od zacatku
int rings_between_years(int month_lenght[], int rings_in_day, int y, int m1, int m2, int d1, int d2, int h1, int h2, int i1, int z)
{
       long long int rings = 0;
       int sunday = 0;
       int leap = is_leap(y);
       if (leap == 1)
              month_lenght[1]++;
       if (z == 0)
       {
              sunday = is_sunday(d1, m1, y);
              for (d1 += 1; d1 <= month_lenght[m1 - 1]; d1++)
              {
                     if (is_sunday(d1, m1, y) == 1)
                            ;
                     else
                            rings += rings_in_day;
              }
              if (leap == 1)
                     month_lenght[1]--;
              rings += rings_for_month_interval(m1, m2 + 1, y, month_lenght, rings_in_day);
       }
       else
       {
              sunday = is_sunday(d2, m2, y);
              for (int i = 1; i < d2; i++)
              {
                     if (is_sunday(i, m2, y) == 1)
                            ;
                     else
                            rings += rings_in_day;
              }
              if (leap == 1)
                     month_lenght[1]--;
              rings += rings_for_month_interval(m1 - 1, m2, y, month_lenght, rings_in_day);
       }

       if (sunday == 1)
              ;
       else
              rings += get_rings_in_day(h1, h2, i1);

       return rings;
}

int get_bell2_rings(int y1, int m1, int d1, int h1, int i1,
                    int y2, int m2, int d2, int h2, int i2, int month_lenght[])
{
       long long int rings = 0;
       int rings_in_day = 156;
       int rings_in_year = 56940;

       if (y1 == y2 && m1 == m2 && d1 == d2)
              if (is_sunday(d1, m1, y1) == 1)
                     ;
              else
              {
                     rings += get_rings_in_day(h1, h2, i1);
              }

       else if (y1 == y2 && m1 == m2 && d1 != d2)
              rings += get_rings_in_month(d1, d2, h1, h2, i1, i2, rings_in_day, m1, m2, y1, y2);
       else if (y1 == y2 && m1 != m2)
              rings += get_rings_in_year(month_lenght, y1, m1, m2, d1, d2, h1, h2, i1, rings_in_day) +
                       rings_for_month_interval(m1, m2, y1, month_lenght, rings_in_day); // mezi mesice
       else
       {
              rings += rings_between_years(month_lenght, rings_in_day, y1, m1, number_of_months, d1, d2, h1, 23, i1, 0);
              if (y2 - y1 >= 2)
                     rings += get_year_rings(y1, y2, rings_in_day, rings_in_year);
              rings += rings_between_years(month_lenght, rings_in_day, y2, 1, m2, d1, d2, 0, h2, 0, 1);
       }

       return rings;
}

// z==0 -dny do konce mesice, jinak od zacatku
int get_bell1_days(int month_lenght[], int rings_in_day, int m, int d, int leap, int z, int y)
{
       long long int rings = 0;
       if (leap == 1)
              month_lenght[1]++;

       if (z == 0)
       {
              for (d += 1; d <= month_lenght[m - 1]; d++)
              {
                     if (is_sunday(d, m, y) == 1)
                            ;
                     else
                            rings += rings_in_day;
              }
       }
       else
       {
              for (int i = 1; i < d; i++)
              {
                     if (is_sunday(i, m, y) == 1)
                            ;
                     else
                            rings += rings_in_day;
              }
       }
       if (leap == 1)
              month_lenght[1]--;
       return rings;
}

// od zacatku hodiny
int get_bell1_min2(int i)
{
       if (i < 15)
              return 4;
       else if (i < 30)
              return 5;
       else if (i < 45)
              return 7;
       else
              return 10;
}

// do konce hodiny
int get_bell1_min1(int i)
{
       if (i == 0)
              return 10;
       else if (i <= 15)
              return 6;
       else if (i <= 30)
              return 5;
       else if (i <= 45)
              return 3;
       else
              return 0;
}

// hodiny h2=24 do konce dne/ h1=-1 od zacatku
int get_bell1_hours(int h1, int h2)
{
       long long int rings = 0;
       int rings_in_hour = 10;

       for (h1 += 1; h1 < h2; h1++)
       {
              rings += rings_in_hour;
       }
       return rings;
}

int get_bell1_mins(int i1, int i2)
{
       long long int rings = 0;
       for (; i1 <= i2; i1++)
       {
              switch (i1)
              {
              case 0:
                     rings += 4;
                     break;
              case 15:
                     rings += 1;
                     break;
              case 30:
                     rings += 2;
                     break;
              case 45:
                     rings += 3;
                     break;
              default:
                     break;
              }
       }
       return rings;
}

int get_bell1_rings(int y1, int m1, int d1, int h1, int i1,
                    int y2, int m2, int d2, int h2, int i2, int month_lenght[])
{
       long long int rings = 0;
       int leap1 = is_leap(y1);
       int leap2 = is_leap(y2);
       int rings_in_day = 240;
       int rings_in_year = 87600;

       if (m1 == m2 && y1 == y2 && d1 == d2 && h1 == h2)
       {
              if (is_sunday(d1, m1, y1) == 1)
                     ;
              else
                     rings += get_bell1_mins(i1, i2);
       }
       else if (m1 == m2 && y1 == y2 && d1 == d2 && h1 != h2)
       {
              if (is_sunday(d1, m1, y1) == 1)
                     ;
              else
              {
                     if (h2 - h1 >= 2)
                            rings += get_bell1_hours(h1, h2);
                     rings += get_bell1_min1(i1);
                     rings += get_bell1_min2(i2);
              }
       }
       else if (m1 == m2 && y1 == y2 && d1 != d2)
       {
              if (d2 - d1 >= 2)
              {
                     for (int i = d1 + 1; i < d2; i++)
                     {
                            if (is_sunday(i, m1, y1) == 1)
                                   ;
                            else
                                   rings += rings_in_day;
                     }
              }

              if (is_sunday(d1, m1, y1) == 1)
                     ;
              else
                     rings += get_bell1_hours(h1, 24) + get_bell1_min1(i1);
              if (is_sunday(d2, m2, y2) == 1)
                     ;
              else
                     rings += get_bell1_hours(-1, h2) + get_bell1_min2(i2);
       }
       else if (m2 != m1 && y1 == y2)
       {
              rings += m2 - m1 >= 2 ? rings_for_month_interval(m1, m2, y1, month_lenght, rings_in_day) : 0;
              rings += get_bell1_days(month_lenght, rings_in_day, m1, d1, leap1, 0, y1) + get_bell1_days(month_lenght, rings_in_day, m2, d2, leap1, 1, y1);
              if (is_sunday(d1, m1, y1) == 1)
                     ;
              else
                     rings += get_bell1_hours(h1, 24) + get_bell1_min1(i1);
              if (is_sunday(d2, m2, y2) == 1)
                     ;
              else
                     rings += get_bell1_hours(-1, h2) + get_bell1_min2(i2);
       }
       else if (y1 != y2)
       {
              if (y2 - y1 >= 2)
                     rings += get_year_rings(y1, y2, rings_in_day, rings_in_year);
              rings += rings_for_month_interval(m1, number_of_months + 1, y1, month_lenght, rings_in_day);                                                  // mesice do konce roka
              rings += rings_for_month_interval(0, m2, y2, month_lenght, rings_in_day);                                                                     // mesice od zacatku roka
              rings += get_bell1_days(month_lenght, rings_in_day, m1, d1, leap1, 0, y1) + get_bell1_days(month_lenght, rings_in_day, m2, d2, leap2, 1, y2); // dny do konce a zacatku mesice
              if (is_sunday(d1, m1, y1) == 1)
                     ;
              else
                     rings += get_bell1_hours(h1, 24) + get_bell1_min1(i1);
              if (is_sunday(d2, m2, y2) == 1)
                     ;
              else
                     rings += get_bell1_hours(-1, h2) + get_bell1_min2(i2);
       }

       return rings;
}

int check_day_input(int year, int month_lenght[], int day, int month)
{
       if (is_leap(year) == 0 && day > month_lenght[month - 1])
              return 0;
       else if (is_leap(year) == 1)
       {
              month_lenght[1]++;
              if (day > month_lenght[month - 1])
                     return 0;
              month_lenght[1]--;
              return 1;
       }
       else
              return 1;
}

int bells(int y1, int m1, int d1, int h1, int i1,
          int y2, int m2, int d2, int h2, int i2,
          long long int *b1, long long int *b2)
{

       if (y1 < 1600 || y2 < 1600 || m1 < 1 || m2 < 1 || m1 > 12 || m2 > 12 ||
           d1 < 1 || d2 < 1 || h1 < 0 || h2 < 0 || h1 > 23 || h2 > 23 ||
           i1 < 0 || i2 < 0 || i1 > 59 || i2 > 59 ||
           y1 > y2 || (y1 == y2 && m1 > m2) ||
           (y1 == y2 && m1 == m2 && d1 == d2 && h1 > h2) ||
           (y1 == y2 && m1 == m2 && d1 > d2) ||
           (y1 == y2 && m1 == m2 && d1 == d2 && h1 == h2 && i1 > i2))
              return 0;

       int month_lenght[number_of_months] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

       if (check_day_input(y1, month_lenght, d1, m1) == 0 ||
           check_day_input(y2, month_lenght, d2, m2) == 0)
              return 0;

       *b1 = get_bell1_rings(y1, m1, d1, h1, i1, y2, m2, d2, h2, i2, month_lenght);
       *b2 = get_bell2_rings(y1, m1, d1, h1, i1, y2, m2, d2, h2, i2, month_lenght);

       return 1;
}

#ifndef __PROGTEST__
int main(int argc, char *argv[])
{
       long long int b1, b2;
       // assert(bells(2000, 1, 1, 2, 2, 2000, 1, 1, 16, 30, &b1, &b2) == 1 && b2==85);
       /*assert(is_leap(2000) == 1);
       assert(is_leap(2024) == 1);
       assert(is_leap(2023) == 0);
       assert(bells(2023, 10, 1, 13, 15,
                    2022, 10, 1, 18, 45, &b1, &b2) == 0);
       assert(bells(2022, 11, 1, 13, 15,
                    2022, 10, 1, 18, 45, &b1, &b2) == 0);
       assert(bells(2022, 10, 2, 13, 15,
                    2022, 10, 1, 18, 45, &b1, &b2) == 0);
       assert(bells(2022, 10, 1, 21, 15,
                    2022, 10, 1, 18, 45, &b1, &b2) == 0);
       assert(bells(2022, 10, 1, 18, 59,
                    2022, 10, 1, 18, 45, &b1, &b2) == 0);
       assert(bells(2022, 2, 29, 13, 15,
                    2022, 10, 1, 18, 45, &b1, &b2) == 0);
       assert(bells(2022, 10, 0, 13, 15,
                    2022, 10, 1, 18, 45, &b1, &b2) == 0);

       assert(bells(2022, 10, 1, 13, 15,
                    2022, 10, 1, 18, 45, &b1, &b2) == 1 &&
              b1 == 56 && b2 == 20);
       assert(bells(2022, 10, 3, 13, 15,
                    2022, 10, 4, 11, 20, &b1, &b2) == 1 &&
              b1 == 221 && b2 == 143);
       assert(bells(2022, 10, 1, 13, 15,
                    2022, 10, 2, 11, 20, &b1, &b2) == 1 &&
              b1 == 106 && b2 == 65);
       assert(bells(2022, 10, 2, 13, 15,
                    2022, 10, 3, 11, 20, &b1, &b2) == 1 &&
              b1 == 115 && b2 == 78);
       assert(bells(2022, 10, 1, 13, 15,
                    2022, 10, 3, 11, 20, &b1, &b2) == 1 &&
              b1 == 221 && b2 == 143);
       assert(is_sunday(2, 1, 2022) == 1);
       assert(is_sunday(1, 1, 2022) == 0);
       assert(is_sunday(3, 1, 2022) == 2);
       assert(is_sunday(4, 1, 2022) == 3);
       assert(is_sunday(5, 1, 2022) == 4);
       assert(is_sunday(6, 2, 2022) == 1);
       assert(bells(2022, 1, 1, 13, 15,
                    2022, 10, 5, 11, 20, &b1, &b2) == 1 &&
              b1 == 56861 && b2 == 36959);
       assert(bells(2019, 1, 1, 13, 15,
                    2019, 10, 5, 11, 20, &b1, &b2) == 1 &&
              b1 == 57101 && b2 == 37115);
       assert(bells(2024, 1, 1, 13, 15,
                    2024, 10, 5, 11, 20, &b1, &b2) == 1 &&
              b1 == 57341 && b2 == 37271);
       assert(bells(1900, 1, 1, 13, 15,
                    1900, 10, 5, 11, 20, &b1, &b2) == 1 &&
              b1 == 57101 && b2 == 37115);
       assert(bells(2022, 10, 1, 0, 0,
                    2022, 10, 1, 12, 0, &b1, &b2) == 1 &&
              b1 == 124 && b2 == 90);
       assert(bells(2022, 10, 1, 0, 15,
                    2022, 10, 1, 0, 25, &b1, &b2) == 1 &&
              b1 == 1 && b2 == 0);
       assert(bells(2022, 10, 1, 12, 0,
                    2022, 10, 1, 12, 0, &b1, &b2) == 1 &&
              b1 == 4 && b2 == 12);
       assert(bells(2022, 11, 1, 12, 0,
                    2022, 10, 1, 12, 0, &b1, &b2) == 0 &&
              b1 == 4 && b2 == 12);
       assert(bells(2022, 10, 32, 12, 0,
                    2022, 11, 10, 12, 0, &b1, &b2) == 0 &&
              b1 == 4 && b2 == 12);
       assert(bells(2100, 2, 29, 12, 0,
                    2100, 2, 29, 12, 0, &b1, &b2) == 0 &&
              b1 == 4 && b2 == 12);
       assert(bells(2000, 2, 29, 12, 0,
                    2000, 2, 29, 12, 0, &b1, &b2) == 1 &&
              b1 == 4 && b2 == 12);
       assert(bells(2004, 2, 29, 12, 0,
                    2004, 2, 29, 12, 0, &b1, &b2) == 1 &&
              b1 == 0 && b2 == 0);*/
       assert(bells(1952, 6, 12, 14, 11,
                    2000, 9, 20, 17, 56, &b1, &b2) == 1 &&
              b1 == 3627156 && b2 == 2357640);

       return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
