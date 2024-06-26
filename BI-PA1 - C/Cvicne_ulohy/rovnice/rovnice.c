#include <stdio.h>
#include <math.h>

int main(void)
{
    double x,y,z;
    char c;
    printf("Zadejte rovnici:\n");

    if(scanf(" %lf %c %lf = %lf", &x,&c,&y,&z)!=4 || (c=='/' && y==0))
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    switch (c)
    {
    case '+':
        {
            if(fabs(x+y-z)<=fabs(z)*__DBL_EPSILON__)
            {
                printf("Rovnice je spravne.\n");
            }else{
                printf("%g != %g\n",x+y,z);
            }
        }
        break;
    
    case '-':
        {
            if(fabs(x-y-z)<=fabs(z)*__DBL_EPSILON__)
            {
                printf("Rovnice je spravne.\n");
            }else{
                printf("%g != %g\n",x-y,z);
            }
        }
        break;
    
    case '*':
        {
            if(fabs(x*y-z)<=fabs(z)*__DBL_EPSILON__)
            {
                printf("Rovnice je spravne.\n");
            }else{
                printf("%g != %g\n",x*y,z);
            }
        }
        break;
    
    case '/':
        {
            if(fabs(trunc(x/y)-z)<=fabs(z)*__DBL_EPSILON__)
            {
                printf("Rovnice je spravne.\n");
            }
            else{
                printf("%g != %g\n",trunc(x/y),z);
            }
        }
        break;

    default:
        {
            printf("Nespravny vstup.\n");
            return 1;
        }
    }
    return 0;
}