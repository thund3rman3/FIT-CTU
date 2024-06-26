#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int rozsah;
    printf("Rozsah:\n");
    if (scanf(" %d", &rozsah) != 1 || rozsah < 1)
    {
        printf("Nespravny vstup.\n");
        return 1;
    }
    int cifry = 2, cifry2=1, r2 = rozsah * rozsah;
    while (r2 > 9)
    {
        r2 /= 10;
        cifry++;
        cifry2++;
    }
    char mezera=' ';
    int radek=0;
    for(int i = 0; i<=rozsah; i++)
    {
        if(i==0)
        {
            printf("%*c|", cifry2, mezera);
            for(int k = rozsah; k>0; k--)
                    {
                        printf("%*d",cifry,k);
                    }
            printf("\n");
            for(int i=1;i<=cifry*rozsah+1+cifry2;i++)
            {
                if(i==cifry2+1)
                {
                    printf("+");
                }else{
                    printf("-");
                }
            }
            printf("\n");


        }else{
            printf("%*d|",cifry2,i);
            for(int j=rozsah; j>radek; j--)
            {
                printf("%*d",cifry,i*j); 
            }
            printf("\n");
            radek++;
        }
    }
    return 0;
}