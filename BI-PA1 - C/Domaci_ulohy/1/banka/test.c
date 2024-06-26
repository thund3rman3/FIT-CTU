#include <stdio.h>
#include <math.h>

int main   (void)
{
    printf("%f\n", floor(5555555 * (0.01))/100);
    printf("%f\n", floor(5555555 * (0.01/100)));
    return 0;
}