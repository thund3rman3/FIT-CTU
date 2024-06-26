#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
mame pojmenovane 4-stenne kostky/domino se 4-mi policky - je to stejne
jmeno je max 64 charu
cheme zjistit jake jsou stejne a jake ne
musime uvazovat otoceni kostky
priklad: asffgrr [2,3,5,1]
*/

#define MAX 64
#define len 4

typedef struct
{
    int alone;
    char name[MAX];
    int values[len];
} T_KOSTKA;

typedef struct
{
    int cnt;
    int size;
    T_KOSTKA *kostky;
} T_KOSTKY;

typedef struct
{
    char name1[MAX];
    char name2[MAX];
} T_DUPLS;

// free
void end_my_suffer(T_KOSTKY *k)
{
    free(k->kostky);
    printf("Nespravny vstup.\n");
}

// jen tak vypis kostek
void print_smth(T_KOSTKY *k)
{
    for (int i = 0; i < k->cnt; i++)
    {
        for (int j = 0; j < len; j++)
        {
            printf(" %d ", k->kostky[i].values[j]);
        }
        printf("\n");
    }
}

int cmp(const int *a, const int *b)
{
    return (*b < *a) - (*a < *b);
}

int main(void)
{
    T_KOSTKY k;
    k.cnt = 0;
    k.size = 5;
    k.kostky = (T_KOSTKA *)malloc(k.size * sizeof(T_KOSTKA));

    int state = 0;
    char thing;

    printf("Piss\n");
    while ((state = scanf(" %63s", k.kostky[k.cnt].name)) == 1)
    {
        if (scanf(" %c", &thing) != 1 || thing != '[')
        {
            end_my_suffer(&k);
            return 1;
        }

        for (int i = 0; i < len; i++)
        {
            if (scanf(" %d", &k.kostky[k.cnt].values[i]) != 1 || k.kostky[k.cnt].values[i] < 0)
            {
                end_my_suffer(&k);
                return 1;
            }
            if (i != len - 1)
            {
                if (scanf(" %c", &thing) != 1 || thing != ',')
                {
                    end_my_suffer(&k);
                    return 1;
                }
            }
        }

        if (scanf(" %c", &thing) != 1 || thing != ']')
        {
            end_my_suffer(&k);
            return 1;
        }
        k.kostky[k.cnt].alone = 1;
        k.cnt++;

        if (k.cnt >= k.size)
        {
            k.size *= 2;
            k.kostky = (T_KOSTKA *)realloc(k.kostky, k.size * sizeof(T_KOSTKA));
        }
    }

    if (state != EOF || k.cnt < 2)
    {
        end_my_suffer(&k);
        return 1;
    }

    print_smth(&k);

    int d_id = 0;
    int d_max = 5;
    T_DUPLS *d = (T_DUPLS *)malloc(d_max * sizeof(T_DUPLS));
    int unique = 0;
    
    
    /* //implemenace pro kostky kde nazalezi na poradi v poli
        for (int i = 0; i < k.cnt; i++)
        {
            qsort(k.kostky[i].values, len, sizeof(int), (int (*)(const void *, const void *))cmp);
        }

        printf("//////////////////////\n");
        print_smth(&k);

        // kazda s kazdou kostkou
        or (int i = 0; i < k.cnt; i++)
        {
            for (int j = i + 1; j < k.cnt; j++)
            {
                // porovnat prvky pole
                for (int l = 0; l < len; l++)
                {
                    if (k.kostky[i].values[l] == k.kostky[j].values[l])
                    {
                        if (l == len - 1)
                        {
                            strcpy(d[d_id].name1, k.kostky[i].name);
                            strcpy(d[d_id].name2, k.kostky[j].name);
                            d_id++;
                            k.kostky[i].alone = 0;
                            k.kostky[j].alone = 0;

                            if (d_id >= d_max)
                            {
                                d_max *= 2;
                                d = (T_DUPLS *)realloc(d, d_max * sizeof(T_DUPLS));
                            }
                        }
                        continue;
                    }
                    else
                        break;
                }
            }
            if (k.kostky[i].alone == 1)
                unique++;
        }
        */

    //implementace pro domino, ktere se muze tocit dokola
    for (int i = 0; i < k.cnt; i++)
    {
        for (int j = i + 1; j < k.cnt; j++)
        {
            // pocet pootoceni
            for (int g = 0; g < len; g++)
            {
                if(k.kostky[i].alone == 0)
                    break;
                int first = k.kostky[i].values[0];
                // otoceni
                for (int gg = 0; gg < len-1; gg++)
                {
                    k.kostky[i].values[gg] = k.kostky[i].values[gg+1];
                }
                k.kostky[i].values[len-1] = first;

                // porovnat prvky pole
                for (int l = 0; l < len; l++)
                {
                    if (k.kostky[i].values[l] == k.kostky[j].values[l])
                    {
                        if (l == len - 1)
                        {
                            strcpy(d[d_id].name1, k.kostky[i].name);
                            strcpy(d[d_id].name2, k.kostky[j].name);
                            d_id++;
                            k.kostky[i].alone = 0;
                            k.kostky[j].alone = 0;

                            if (d_id >= d_max)
                            {
                                d_max *= 2;
                                d = (T_DUPLS *)realloc(d, d_max * sizeof(T_DUPLS));
                            }
                        }
                        continue;
                    }
                    else
                        break;
                }

            }

        }
        if (k.kostky[i].alone == 1)
            unique++;
    }

    printf("unikatni: %d\n", unique);

    for (int i = 0; i < d_id; i++)
    {
        printf("%s = %s\n", d[i].name1, d[i].name2);
    }

    free(k.kostky);
    free(d);
    return 0;
}