#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * naplanuj cyklotrasu
 * vstup:
 *  nm.v. na zacatku >= nm.v. na konci
 *  1.vstup = nm.v zacaktku
 *  dalsi = + vzd.od_startu nm.v.
 * vystup:
 *  cesta s nejvyssim klesanim
 *  je-li vice moznosti se stejnym klesanim, zvolime tu delsi
 *  format: *delka_useku km, klesani: klesani m, varianty: pocet*
 *           *varianty*
 *  pokud nelze najit: *nenalezeno*
 */

typedef struct
{
    int range;
    int elevation;
} CYK;

typedef struct
{
    int size;
    int max;
    CYK *points;
} TW_CYK;

typedef struct
{
    int range1;
    int el1;
    int range2;
    int el2;
    int range_in;
} PATH;

typedef struct
{
    int size;
    int max;
    PATH *path;
} TW_PATH;

void ini_cyk(TW_CYK *a)
{
    a->max = 2;
    a->size = 0;
    a->points = (CYK *)malloc(a->max * sizeof(*a->points));
}

void re_cyk(TW_CYK *a)
{
    a->max *= 2;
    a->points = (CYK *)realloc(a->points, a->max * sizeof(*a->points));
}

void ini_path(TW_PATH *a)
{
    a->max = 2;
    a->size = 0;
    a->path = (PATH *)malloc(a->max * sizeof(*a->path));
}

void re_path(TW_PATH *a)
{
    a->max *= 2;
    a->path = (PATH *)realloc(a->path, a->max * sizeof(*a->path));
}

int cmp(const PATH *a, const PATH *b)
{
    return (a->range_in < b->range_in) - (b->range_in < a->range_in);
}

int main(void)
{
    int state = 0;
    char c;
    TW_CYK tr;
    ini_cyk(&tr);
    printf("*Cyklotrasa:*\n");

    if ((state = scanf(" %d", &tr.points[tr.size].elevation)) != 1)
    {
        printf("Nespravny vstup.\n");
        free(tr.points);
        return 1;
    }
    tr.points[tr.size].range = 0;
    tr.size++;

    while ((state = scanf(" %c %d %d", &c, &tr.points[tr.size].range,
                          &tr.points[tr.size].elevation)) == 3 &&
           c == '+')
    {
        tr.size++;
        if (tr.size >= tr.max)
            re_cyk(&tr);
    }
    if (state != EOF)
    {
        printf("Nespravny vstup.\n");
        free(tr.points);
        return 1;
    }

    int min = 0;
    for (int i = 0; i < tr.size; i++)
    {
        for (int j = i + 1; j < tr.size; j++)
        {
            int tmp = tr.points[i].elevation - tr.points[j].elevation;
            if (tmp < 0)
                continue;
            if (tmp > min)
                min = tmp;
        }
    }
    printf("max klesani:%d\n", min);

    int variants = 0;
    TW_PATH p;
    ini_path(&p);

    for (int i = 0; i < tr.size; i++)
    {
        for (int j = i + 1; j < tr.size; j++)
        {
            if (tr.points[i].elevation - tr.points[j].elevation == min)
            {
                p.path[p.size].el1 = tr.points[i].elevation;
                p.path[p.size].range1 = tr.points[i].range;
                p.path[p.size].el2 = tr.points[j].elevation;
                p.path[p.size].range2 = tr.points[j].range;
                p.path[p.size].range_in = tr.points[j].range - tr.points[i].range;
                p.size++;
                if (p.size >= p.max)
                    re_path(&p);
            }
        }
    }

    qsort(p.path, p.size, sizeof(p.path[0]), (int (*)(const void *, const void *))cmp);
    int max = p.path[0].range_in;
    for (int i = 0; i < p.size; i++)
    {
        if (p.path[i].range_in == max)
            variants++;
    }
    if (variants == 0)
        printf("*Nenalezeno*\n");
    else
    {

        printf("*%d km, klesani: %d m, varianty: %d*\n", max, min, variants);

        for (int i = 0; i < p.size; i++)
        {
            if (p.path[i].range_in == max)
                printf("+ %d (%d) -> + %d (%d)\n", p.path[i].range1, p.path[i].el1,
                       p.path[i].range2, p.path[i].el2);
        }
    }
    free(tr.points);
    free(p.path);
    return 0;
}