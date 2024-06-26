#include <stdio.h>
#include <math.h>

int scan(double *x, double *y)
{
    if (scanf(" %lf %lf", x, y) != 2)
    {
        printf("Nespravny vstup.\n");
        return 1;
    }
    return 0;
}

int main(void)
{

    double ax, ay, bx, by, cx, cy;

    printf("Bod A:\n");
    if(scan(&ax, &ay)==1)
    {
        return 1;
    }
   // printf("%f,%f\n", ax, ay);

    printf("Bod B:\n");
    if(scan(&bx, &by)==1)
    {
        return 1;
    };
   // printf("%f,%f\n", bx, by);

    printf("Bod C:\n");
    if(scan(&cx, &cy)==1)
    {
        return 1;
    };
    //printf("%f,%f\n", cx, cy);

    double v_AaB = sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by));
    double v_BaC = sqrt((bx - cx) * (bx - cx) + (by - cy) * (by - cy));
    double v_AaC = sqrt((ax - cx) * (ax - cx) + (ay - cy) * (ay - cy));
    double max = v_AaB, mid, min;
   // printf("%f, %f, %f\n", v_AaB, v_AaC, v_BaC);
    char bod;

    // if A=B=C
    if ((ax == bx && ay == by) || 
        (ax == cx && ay == cy) || 
        (bx==cx && by==cy))
    {
        printf("Nektere body splyvaji.\n");
        return 0;
    }

    if (v_AaC - max > max * __DBL_EPSILON__ &&  v_AaC - v_BaC > max*__DBL_EPSILON__)
    {
        max = v_AaC;
        if (v_BaC - v_AaB > max * __DBL_EPSILON__)
        {
            mid = v_BaC;
            min = v_AaB;
        }
        else
        {
            mid = v_AaB;
            min = v_BaC;
        }
        bod='B';
    }
    else if (v_BaC - max > max * __DBL_EPSILON__ && v_BaC - v_AaC > max*__DBL_EPSILON__)
    {
        max = v_BaC;
        if (v_AaC - v_AaB > max * __DBL_EPSILON__)
        {
            mid = v_AaC;
            min = v_AaB;
        }
        else
        {
            mid = v_AaB;
            min = v_AaC;
        }
        bod='A';
    }
    else
    {
        bod='C';
        if (v_AaC - v_BaC > max * __DBL_EPSILON__)
        {
            mid = v_AaC;
            min = v_BaC;
        }
        else
        {
            mid = v_BaC;
            min = v_AaC;
        }
    }
/*
    printf("min=%f\n", min);
    printf("mid=%f\n", mid);
    printf("max=%f\n", max);
    printf("stred %c\n", bod);
*/
    if(fabs(max-mid-min)<=max*__DBL_EPSILON__)
    {
        printf("Body lezi na jedne primce.\n");
        printf("Prostredni je bod %c.\n",bod);
    }else{
        printf("Body nelezi na jedne primce.\n");
    }

    return 0;
}
