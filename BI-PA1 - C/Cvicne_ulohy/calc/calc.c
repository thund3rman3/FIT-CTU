#include <stdio.h>
#include <math.h>

int main(void)
{
    double x, y;
    char op, eq;

    printf("Zadejte vzorec:\n");

    if (scanf(" %lf %c %lf %c", &x, &op, &y, &eq) != 4 ||
        (op != '+' && op != '-' && op != '*' && op != '/') ||
        (op == '/' && y == 0) || eq != '=')
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    
    /*
    VYPOCET EXPONENTU Z CISLA
    int zz=z;
    int exp = floor(log10(fabs(z)));
    if(exp<5)
    {
        z=zz;
    }
    */

    switch (op)
    {
    case '+':
        printf("%g\n", x + y);
        break;
    case '-':
        printf("%g\n", x - y);
        break;
    case '*':
        printf("%g\n", x * y);
        break;
    case '/':
        printf("%g\n", trunc(x / y));
        break;

    default:
        printf("Nespravny vstup.\n");
        return 1;
    }

    return 0;
}