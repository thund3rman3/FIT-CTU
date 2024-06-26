#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 11

/**
 * vlozit data:
 * + region[max 10 charu] plat
 * dotaz: pokud je ?region obsazen v +region tak najdi necastejsi vyskyt
 *  platu v techto regionech, nebo pri ?* vypocte nejcastejsi plat ze vsech
 *  pokud je vice nejcastejsich, ukaz vsechny
 *  format vystupu: MODUS: 
 *      1: plat [vyskyt]
 *      2+: plat, plat, ... [vyskyt]
 *      nic: N/A
 * ? region
 * osetreni:
 * plat neni zaporny, region neni cislo
*/

typedef struct
{
    char region[MAX];
    int plat;
} TREG;

typedef struct
{
    TREG *record;
    int size;
    int max;
} TAR;

typedef struct
{
    int plat;
    int occur;
} MODUS;

typedef struct
{
    MODUS *platy;
    int size;
    int max;
} TMOD;

void ini_tmod(TMOD *res)
{
    res->max = 2;
    res->size = 0;
    res->platy = (MODUS *)malloc(res->max * sizeof(*res->platy));
}

void reallocate_tmod(TMOD *res)
{
    res->max *= 2;
    res->platy = (MODUS *)realloc(res->platy, res->max * sizeof(*res->platy));
}

void ini_tar(TAR *record_wrap)
{
    record_wrap->max = 2;
    record_wrap->size = 0;
    record_wrap->record = (TREG *)malloc(record_wrap->max * sizeof(*record_wrap->record));
}

void reallocate_treg(TAR *record_wrap)
{
    record_wrap->max *= 2;
    record_wrap->record = (TREG *)realloc(record_wrap->record, record_wrap->max * sizeof(*record_wrap->record));
}

void end_quote()
{
    printf("Nespravny vstup.\n");
}

int cmp(const MODUS* a, const MODUS* b)
{
    if(a->occur == b->occur)
        return (b->plat < a->plat) - (a->plat < b->plat);
    return (a->occur < b->occur) - (b->occur < a->occur);
}

int main(void)
{
    TAR record_wrap;
    ini_tar(&record_wrap);

    int state = 0;
    char c, buffer[MAX];
    int flag = 0;

    while ((state = scanf(" %c", &c)) == 1)
    {
        switch (c)
        {
        case '+':
            if (flag == 1)
            {
                end_quote();
                free(record_wrap.record);
                return 1;
            }
            if ((state = scanf(" %10s %d", record_wrap.record[record_wrap.size].region, &record_wrap.record[record_wrap.size].plat)) != 2 || record_wrap.record[record_wrap.size].plat < 0)
            {
                end_quote();
                free(record_wrap.record);
                return 1;
            }

            for (int i = 0; record_wrap.record[record_wrap.size].region[i] != '\0'; i++)
            {
                if (isdigit(record_wrap.record[record_wrap.size].region[i]) == 0)
                {
                    end_quote();
                    free(record_wrap.record);
                    return 1;
                }
            }
            record_wrap.size++;
            if (record_wrap.size >= record_wrap.max)
                reallocate_treg(&record_wrap);
            break;
        case '?':
            flag = 1;
            if ((state = scanf(" %10s", buffer)) != 1)
            {
                end_quote();
                free(record_wrap.record);
                return 1;
            }

            TMOD res;
            ini_tmod(&res);

            if (strncmp(buffer, "*", strlen("*")) == 0)
            {
                for (int i = 0; i < record_wrap.size; i++)
                {
                    int found = 0;
                    for (int j = 0; j < res.size; j++)
                    {
                        if (res.platy[j].plat == record_wrap.record[i].plat)
                        {
                            found = 1;
                            res.platy[j].occur++;
                            break;
                        }
                    }
                    if (found == 0)
                    {
                        res.platy[res.size].plat = record_wrap.record[i].plat;
                        res.platy[res.size].occur = 1;
                        res.size++;
                        if (res.size >= res.max)
                            reallocate_tmod(&res);
                    }
                }
                if (res.size == 0)
                    printf("MODUS: N/A\n");
                else
                {
                    qsort(res.platy, res.size, sizeof(MODUS), (int (*)(const void *, const void *))cmp);
                    /*for (int i = 0; i < res.size; i++)
                    {
                        printf(" %d,", res.platy[i].plat);
                    }
                    printf("\n");*/

                    printf("MODUS: %d", res.platy[0].plat);
                    int prev = res.platy[0].plat;
                    for (int i = 1; i < res.size; i++)
                    {
                        if (res.platy[0].occur == res.platy[i].occur &&
                            res.platy[i].plat != prev)
                        {
                            printf(", %d", res.platy[i].plat);
                            prev = res.platy[i].plat;
                        }
                    }
                    printf(" [%d]\n", res.platy[0].occur);
                }
                free(res.platy);
            }
            else
            {
                for (int i = 0; buffer[i] != '\0'; i++)
                {
                    if (isdigit(buffer[i]) == 0)
                    {
                        end_quote();
                        free(res.platy);
                        free(record_wrap.record);
                        return 1;
                    }
                }
                for (int i = 0; i < record_wrap.size; i++)
                {
                    if (strncmp(buffer, record_wrap.record[i].region, strlen(buffer)) == 0)
                    {
                        int found = 0;
                        for (int j = 0; j < res.size; j++)
                        {
                            if (res.platy[j].plat == record_wrap.record[i].plat)
                            {
                                found = 1;
                                res.platy[j].occur++;
                                break;
                            }
                        }
                        if (found == 0)
                        {
                            res.platy[res.size].plat = record_wrap.record[i].plat;
                            res.platy[res.size].occur = 1;
                            res.size++;
                            if (res.size >= res.max)
                                reallocate_tmod(&res);
                        }
                    }
                }
                if (res.size == 0)
                    printf("MODUS: N/A\n");
                else
                {
                    qsort(res.platy, res.size, sizeof(MODUS), (int (*)(const void *, const void *))cmp);
                    /*for (int i = 0; i < res.size; i++)
                    {
                        printf(" %d,", res.platy[i].plat);
                    }
                    printf("\n");*/

                    printf("MODUS: %d", res.platy[0].plat);
                    int prev = res.platy[0].plat;
                    for (int i = 1; i < res.size; i++)
                    {
                        if (res.platy[0].occur == res.platy[i].occur &&
                            res.platy[i].plat != prev)
                        {
                            printf(", %d", res.platy[i].plat);
                            prev = res.platy[i].plat;
                        }
                    }
                    printf(" [%d]\n", res.platy[0].occur);
                }
                free(res.platy);
            }
            break;

        default:
            end_quote();
            free(record_wrap.record);
            return 1;
        }
    }
    if (state != EOF)
    {
        end_quote();
        free(record_wrap.record);
        return 1;
    }

    free(record_wrap.record);
    return 0;
}