#ifndef __PROGTEST__
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <stdint.h>
#endif /* __PROGTEST__ */

long long avg3(long long a, long long b, long long c)
{
  if (a + b + c < a && a > 0)
  {
    //printf("overflow\n");
    return a;
  }
  return (a + b + c) / 3;
}

#ifndef __PROGTEST__
int main(int argc, char *argv[])
{
  assert(avg3(1, 2, 3) == 2);
  assert(avg3(-100, 100, 30) == 10);
  assert(avg3(1, 2, 2) == 1);
  assert(avg3(-1, -2, -2) == -1);
  assert(avg3(LLONG_MAX, LLONG_MAX, LLONG_MAX) == LLONG_MAX);
  return 0;
}
#endif /* __PROGTEST__ */
