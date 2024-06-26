#include <stdio.h>
#include <stdlib.h>

void print_line(int n)
{
    for (int i = 0; i < n; i++)
    {
        if (i == 0)
        {
            printf("+");
        }
        else if (i == n - 1)
        {
            printf("+\n");
        }
        else
        {
            printf("-");
        }
    }
}

int main(void)
{
    int pocet_poli, velikost_pole;
    printf("Zadejte pocet poli:\n");

    if (scanf(" %d", &pocet_poli) != 1 || pocet_poli < 1)
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    printf("Zadejte velikost pole:\n");

    if (scanf(" %d", &velikost_pole) != 1 || velikost_pole < 1)
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    int n = pocet_poli * velikost_pole + 2;

    print_line(n);

    for (int i = 0; i < pocet_poli; i++)
    {
        if(i%2==0)
        {
            for(int l=0;l<velikost_pole;l++)
            {
                for (int j = 0; j < pocet_poli+2; j++)
                {
                    if (j == 0)
                    {
                        printf("|");
                    }
                    else if (j == pocet_poli+1)
                    {
                        printf("|\n");
                    }
                    else if (j % 2 == 1)
                    {
                        for (int k = 0; k < velikost_pole; k++)
                        {
                            printf(" ");
                        }
                    }
                    else
                    {
                        for (int k = 0; k < velikost_pole; k++)
                        {
                            printf("X");
                        }
                    }
                }
            }

        }else{
            for(int l=0;l<velikost_pole;l++)
            {
                for (int j = 0; j < pocet_poli+2; j++)
                {
                    if (j == 0)
                    {
                        printf("|");
                    }
                    else if (j == pocet_poli+1)
                    {
                        printf("|\n");
                    }
                    else if (j % 2 == 1)
                    {
                        for (int k = 0; k < velikost_pole; k++)
                        {
                            printf("X");
                        }
                    }
                    else
                    {
                        for (int k = 0; k < velikost_pole; k++)
                        {
                            printf(" ");
                        }
                    }
                }
            }
        }

    }

    print_line(n);

    return 0;
}