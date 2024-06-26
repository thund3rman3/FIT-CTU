#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 65
#define CNT 4

/**
 * domino kostky:
 * vstup: na kazdem radku kostka
 *  format: nazev[max 64charu] [x1,x2,x3,x4]
 *  kostky se daji otacet tzn posouvat prvky pole do L/R
 * vystup:
 *  pocet unikatu
 *  ktere domina jsou duplicitni
 */

typedef struct
{
    char name[MAX];
    int numbers[CNT];
} DOMINO;

typedef struct
{
    DOMINO *domina_arr;
    int size;
    int max;
} TW_DOMINO;

void ini_domino(TW_DOMINO *d)
{
    d->size = 0;
    d->max = 2;
    d->domina_arr = (DOMINO *)malloc(d->max * sizeof(*d->domina_arr));
}

void reallocate_domino(TW_DOMINO *d)
{
    d->max *= 2;
    d->domina_arr = (DOMINO *)realloc(d->domina_arr, d->max * sizeof(*d->domina_arr));
}

int cmp(const DOMINO *a, const DOMINO *b)
{
    if(b->numbers[0] != a->numbers[0])
        return (b->numbers[0] < a->numbers[0]) - (a->numbers[0] < b->numbers[0]);
    else if(b->numbers[1] != a->numbers[1])
        return (b->numbers[1] < a->numbers[1]) - (a->numbers[1] < b->numbers[1]);
    else if(b->numbers[2] != a->numbers[2])
        return (b->numbers[2] < a->numbers[2]) - (a->numbers[2] < b->numbers[2]);
    else
        return (b->numbers[3] < a->numbers[3]) - (a->numbers[3] < b->numbers[3]);

    
}

int main(void)
{
    TW_DOMINO d;
    ini_domino(&d);
    int state = 0;
    char leva, prava;
    char c1, c2, c3;

    printf("Vstup:\n");
    while ((state = scanf(" %64s", d.domina_arr[d.size].name)) == 1)
    {
        if ((state = scanf(" %c %d %c %d %c %d %c %d %c",
                           &leva, &d.domina_arr[d.size].numbers[0],
                           &c1, &d.domina_arr[d.size].numbers[1],
                           &c2, &d.domina_arr[d.size].numbers[2],
                           &c3, &d.domina_arr[d.size].numbers[3], &prava)) != 9 ||
            leva != '[' || prava != ']' || c1 != ',' || c2 != ',' || c3 != ',' ||
            d.domina_arr[d.size].numbers[0] < 0 || d.domina_arr[d.size].numbers[1] < 0 ||
            d.domina_arr[d.size].numbers[2] < 0 || d.domina_arr[d.size].numbers[3] < 0)
            break;
        d.size++;
        if (d.size >= d.max)
            reallocate_domino(&d);
    }
    if (state != EOF)
    {
        printf("Nespravny stup.\n");
        free(d.domina_arr);
        return 1;
    }

    for (int i = 0; i < d.size; i++)
    {
        int cnt = 0;
        int min = d.domina_arr[i].numbers[0];
        if (d.domina_arr[i].numbers[1] < min)
        {
            min = d.domina_arr[i].numbers[1];
            cnt = 3;
        }
        if (d.domina_arr[i].numbers[2] < min)
        {
            min = d.domina_arr[i].numbers[2];
            cnt = 2;
        }
        if (d.domina_arr[i].numbers[3] < min)
            cnt = 1;

        for (int j = 0; j < cnt; j++)
        {
            int tmp = d.domina_arr[i].numbers[3];
            d.domina_arr[i].numbers[3] = d.domina_arr[i].numbers[2];
            d.domina_arr[i].numbers[2] = d.domina_arr[i].numbers[1];
            d.domina_arr[i].numbers[1] = d.domina_arr[i].numbers[0];
            d.domina_arr[i].numbers[0] = tmp;
        }
    }

    qsort(d.domina_arr, d.size, sizeof(DOMINO), (int (*)(const void *, const void *))cmp);

    for (int i = 0; i < d.size; i++)
    {
        printf("%s %d %d %d %d\n", d.domina_arr[i].name, d.domina_arr[i].numbers[0],
               d.domina_arr[i].numbers[1],
               d.domina_arr[i].numbers[2],
               d.domina_arr[i].numbers[3]);
    }

    printf("Unikatni:\n");
    DOMINO prev;
    int unique = 0;
    for (int i = 0; i < d.size; i++)
    {
        if (i == 0)
        {
            unique++;
            prev = d.domina_arr[i];
        }
        else
        {
            if (prev.numbers[0] != d.domina_arr[i].numbers[0] ||
                prev.numbers[1] != d.domina_arr[i].numbers[1] ||
                prev.numbers[2] != d.domina_arr[i].numbers[2] ||
                prev.numbers[3] != d.domina_arr[i].numbers[3])
                unique++;
            prev = d.domina_arr[i];
        }
    }
    printf("%d\nDuplicity:\n", unique);

    int first = 1, ex = 0;
    for (int i = 0; i < d.size; i++)
    {
        if (i != 0)
        {
            if (prev.numbers[0] == d.domina_arr[i].numbers[0] &&
                prev.numbers[1] == d.domina_arr[i].numbers[1] &&
                prev.numbers[2] == d.domina_arr[i].numbers[2] &&
                prev.numbers[3] == d.domina_arr[i].numbers[3])
            {
                ex =1;
                if (first == 1)
                {
                    first = 0;
                    printf("%s = %s", prev.name, d.domina_arr[i].name);
                }
                else if (first == 0 )
                {
                    printf(" = %s", d.domina_arr[i].name);
                }
            }
            else if (first == 0)
                {
                    first = 1;
                    printf("\n");
                }
            
            prev = d.domina_arr[i];
        }
        else
            prev = d.domina_arr[i];
    }
    if(ex ==1)
        printf("\n");

    free(d.domina_arr);
    return 0;
}