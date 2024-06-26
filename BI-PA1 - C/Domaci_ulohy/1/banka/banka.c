#include <stdio.h>
#include <math.h>

int main(void)
{
    double zustatek = 0, tr = -1;
    int den;

    // denni urok v procentech
    double k_u, d_u;

    printf("Zadejte kreditni urok [%%]:\n");

    if (scanf(" %lf", &k_u) != 1)
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    printf("Zadejte debetni urok [%%]:\n");

    if (scanf(" %lf", &d_u) != 1)
    {
        printf("Nespravny vstup.\n");
        return 2;
    }

    printf("Zadejte transakce:\n");
    int prev = 0;
    while (tr != 0)
    {
        if (scanf(" %d, %lf", &den, &tr) != 2 || prev > den)
        {
            printf("Nespravny vstup.\n");
            return 3;
        }

        if (zustatek < 0)
        {
            for (; prev < den; prev++)
            {
                zustatek += ceil(zustatek * d_u)/100;
               // printf("Zustatek: %.2f\n", zustatek);
            }
        }
        else if (zustatek > 0)
        {
            for (; prev < den; prev++)
            {
                zustatek += floor(zustatek * k_u)/100;
               // printf("Zustatek: %.2f\n", zustatek);
            }
        }
        zustatek += tr;        
        prev = den;
    }

    printf("Zustatek: %.2f\n", zustatek);

    return 0;
}