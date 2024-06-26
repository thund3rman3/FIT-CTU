#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int t1_h, t1_m, t1_s, t1_ms=0;
    int t2_h, t2_m, t2_s, t2_ms=0;
    int h, m, s, ms;
    char ch1,ch2;

    printf("Zadejte cas t1:\n");
    if (scanf(" %d : %d : %d %c", &t1_h, &t1_m, &t1_s, &ch1) != 4 ||
        t1_h > 23 || t1_h < 0 ||
        t1_m > 59 || t1_m < 0 ||
        t1_s > 59 || t1_s < 0 || ch1!=',')
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    int t, r;
    while (42)
    {
        r = getchar();
        if ((char)r != ' ')
        {
            break;
        }
    }
    for (int i = 100; i >= 1; i /= 10)
    {
        t = 0;
        if (i == 100)
        {
            t = r - 48;
        }
        else
        {
            t = (int)getchar() - 48;
        }
        if (t > 0 && t < 10)
        {
            t1_ms += t * i;
        }
        else if (t == -38)
        {
            break;
        }
        else if(t!=0){
            printf("Nespravny vstup.\n");
            return 2;
        }
    }

    if(t1_ms > 999 || t1_ms < 0 )
    {
        printf("Nespravny vstup.\n");
        return 2;
    }

    printf("Zadejte cas t2:\n");
    if (scanf(" %d : %d : %d %c", &t2_h, &t2_m, &t2_s, &ch2) != 4 ||
        t2_h > 23 || t2_h < 0 ||
        t2_m > 59 || t2_m < 0 ||
        t2_s > 59 || t2_s < 0 || ch2!=',')
    {
        printf("Nespravny vstup.\n");
        return 2;
    }

    r = 0;
    while (42)
    {
        r = getchar();
        if ((char)r != ' ')
        {
            break;
        }
    }
    for (int i = 100; i >= 1; i /= 10)
    {
        t = 0;
        if (i == 100)
        {
            t = r - 48;
        }
        else
        {
            t = (int)getchar() - 48;
        }
        if (t > 0 && t < 10)
        {
            t2_ms += t * i;
        }
        else if (t == -38)
        {
            break;
        }
        else if(t!=0){
            printf("Nespravny vstup.\n");
            return 2;
        }
    }

    if ( t2_ms > 999 || t2_ms < 0 ||
        t1_h > t2_h || (t1_h == t2_h && t1_m > t2_m) ||
        (t1_h == t2_h && t1_m == t2_m && t1_s > t2_s) ||
        (t1_h == t2_h && t1_m == t2_m && t1_s == t2_s && t1_ms > t2_ms))
    {
        printf("Nespravny vstup.\n");
        return 3;
    }

    int sum1 = t1_h * 60 * 60 * 1000 + t1_m * 60 * 1000 + t1_s * 1000 + t1_ms;
    int sum2 = t2_h * 60 * 60 * 1000 + t2_m * 60 * 1000 + t2_s * 1000 + t2_ms;
    int res = sum2 - sum1;
    ms = res % 1000;
    res = (res - ms) / 1000;
    s = res % 60;
    res = (res - s) / 60;
    m = res % 60;
    h = (res - m) / 60;

    printf("Doba:%3d:%02d:%02d,%03d\n", h, m, s, ms);
    return 0;
}