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

int cmp(const T_KOSTKA *a, const T_KOSTKA *b)
{
    if(b->values[0] != a->values[0])
        return (b->values[0] < a->values[0]) 	- (a->values[0] < b->values[0]);
    else if(b->values[1] != a->values[1])
        return (b->values[1] < a->values[1]) - (a->values[1] < b->values[1]);
    else if(b->values[2] != a->values[2])
        return (b->values[2] < a->values[2]) - (a->values[2] < b->values[2]);
    else 
        return (b->values[3] < a->values[3]) - (a->values[3] < b->values[3]);
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

    // kazda kostka
    for (int i = 0; i < k.cnt; i++)
    {
        // najit min
        int min = __INT_MAX__;
        int min_id = 0;
        for (int j = 0; j < len; j++)
        {
            if(k.kostky[i].values[j] < min )
            {
                min = k.kostky[i].values[j];
                min_id = j;
            }
        }

        for (int l = 0; l < min_id; l++)
        {
            int d = k.kostky[i].values[0];
            printf("%d\n",d);
            k.kostky[i].values[0] = k.kostky[i].values[1];
            printf("%d\n",k.kostky[i].values[0]);
            k.kostky[i].values[1] = k.kostky[i].values[2];
            printf("%d\n",k.kostky[i].values[1]);
            k.kostky[i].values[2] = k.kostky[i].values[3];
            printf("%d\n",k.kostky[i].values[2]);
            k.kostky[i].values[3] = d;
            printf("%d\n",k.kostky[i].values[3]);
        }
    }
    printf("/////////////////\n");
    print_smth(&k);
    qsort(k.kostky,k.cnt,sizeof(T_KOSTKA),(int(*)(const void*, const void*))cmp);
    printf("/////////////////\n");
    print_smth(&k);

    int uq =0;
    for(int i = 0; i<k.cnt;i++)
    {
        if(i==0 && (k.kostky[i].values[0] != k.kostky[i+1].values[0] ||
            k.kostky[i].values[1] != k.kostky[i+1].values[1] ||
            k.kostky[i].values[2] != k.kostky[i+1].values[2] ||
            k.kostky[i].values[3] != k.kostky[i+1].values[3]))
            uq++;
        else if(i == k.cnt-1 && (k.kostky[i].values[0] != k.kostky[i-1].values[0] ||
            k.kostky[i].values[1] != k.kostky[i-1].values[1] ||
            k.kostky[i].values[2] != k.kostky[i-1].values[2] ||
            k.kostky[i].values[3] != k.kostky[i-1].values[3]))
            uq++;
        else if((k.kostky[i].values[0] != k.kostky[i-1].values[0] ||
            k.kostky[i].values[1] != k.kostky[i-1].values[1] ||
            k.kostky[i].values[2] != k.kostky[i-1].values[2] ||
            k.kostky[i].values[3] != k.kostky[i-1].values[3]) &&
            (k.kostky[i].values[0] != k.kostky[i+1].values[0] ||
            k.kostky[i].values[1] != k.kostky[i+1].values[1] ||
            k.kostky[i].values[2] != k.kostky[i+1].values[2] ||
            k.kostky[i].values[3] != k.kostky[i+1].values[3]))
            uq++;
    }
    printf("unique %d\nDuplicity:\n",uq);

    for (int i = 0; i < k.cnt; i++)
    {
        for (int j = i+1; j < k.cnt; j++)
        {
            if((k.kostky[i].values[0] == k.kostky[j].values[0] &&
                k.kostky[i].values[1] == k.kostky[j].values[1] &&
                k.kostky[i].values[2] == k.kostky[j].values[2] &&
                k.kostky[i].values[3] == k.kostky[j].values[3]))
                printf("%s = %s\n",k.kostky[i].name, k.kostky[j].name);
        }
        
    }
    

    free(k.kostky);
    return 0;
}
