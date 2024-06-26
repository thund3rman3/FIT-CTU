#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX 101

/**
 * na vstupu:
 *  ean kody 5-100 charu
 * vystup:
 *  10 nejcastejsich eanu
 */

typedef struct
{
    char ean[MAX];
    int pos;
} T_EAN;

typedef struct
{
    T_EAN *eans;
    int size;
    int max;
} TW_EAN;

typedef struct
{
    char ean[MAX];
    int occ;
    int pos;
} T_OUT;

typedef struct
{
    T_OUT *res;
    int size;
    int max;
} TW_OUT;

void ini_ean(TW_EAN *a)
{
    a->max = 2;
    a->size = 0;
    a->eans = (T_EAN *)malloc(a->max * sizeof(*a->eans));
}

void reallocate_ean(TW_EAN *a)
{
    a->max *= 2;
    a->eans = (T_EAN *)realloc(a->eans, a->max * sizeof(*a->eans));
}

void ini_out(TW_OUT *a)
{
    a->max = 2;
    a->size = 0;
    a->res = (T_OUT *)malloc(a->max * sizeof(*a->res));
}

void reallocate_out(TW_OUT *a)
{
    a->max *= 2;
    a->res = (T_OUT *)realloc(a->res, a->max * sizeof(*a->res));
}

int cmp_EAN(const T_EAN *a, const T_EAN *b)
{
    return strcmp(a->ean, b->ean);
}

int cmp_OUT(const T_OUT *a, const T_OUT *b)
{
    if (a->occ == b->occ)
        return (b->pos < a->pos) - (a->pos < b->pos);
    return (a->occ < b->occ) - (b->occ < a->occ);
}

int main(void)
{
    TW_EAN twean;
    ini_ean(&twean);
    int state = 0;

    while ((state = scanf(" %100s", twean.eans[twean.size].ean)) == 1)
    {
        int len = 0;
        if ((len = strlen(twean.eans[twean.size].ean)) < 5)
        {
            state = 0;
            break;
        }
        int id = 0;
        while (id < len)
        {
            if (isdigit(twean.eans[twean.size].ean[id]) == 0)
            {
                printf("Nespravny vstup.\n");
                free(twean.eans);
                return 1;
            }
            id++;
        }

        twean.eans[twean.size].pos = twean.size;
        twean.size++;
        if (twean.size >= twean.max)
            reallocate_ean(&twean);
    }
    if (state != EOF)
    {
        printf("Nespravny vstup.\n");
        free(twean.eans);
        return 1;
    }

    qsort(twean.eans, twean.size, sizeof(twean.eans[0]), (int (*)(const void *, const void *))cmp_EAN);
    ;
    printf(",,,,,,,,,,,,,,,,,,,,,,,\n");
    for (int i = 0; i < twean.size; i++)
    {
        printf("%s ", twean.eans[i].ean);
    }
    printf("\n,,,,,,,,,,,,,,,,,,,,,,,\n");

    TW_OUT out;
    ini_out(&out);
    for (int i = 0; i < twean.size; i++)
    {
        int found = 0;
        for (int k = 0; k < out.size; k++)
        {
            if (strcmp(twean.eans[i].ean, out.res[k].ean) == 0)
            {
                found = 1;
                out.res[k].occ++;
                break;
            }
        }
        if (found == 0)
        {
            strcpy(out.res[out.size].ean, twean.eans[i].ean);
            out.res[out.size].occ = 1;
            out.res[out.size].pos = twean.eans[i].pos;
            out.size++;
            if (out.size >= out.max)
                reallocate_out(&out);
        }
    }

    qsort(out.res, out.size, sizeof(out.res[0]), (int (*)(const void *, const void *))cmp_OUT);
    if (out.size > 10)
        out.size = 10;
    for (int i = 0; i < out.size; i++)
    {
        printf("%s %dx\n", out.res[i].ean, out.res[i].occ);
    }

    free(out.res);
    free(twean.eans);
    return 0;
}