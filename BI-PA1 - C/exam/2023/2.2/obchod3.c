#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 101
/**
 * sklad
 * vstup:
 *  format: +/- pocet_kusu EAN[5-100charu]
 * vystup:
 *  pocet kusu na sklade
 *  pokud se ma vyskladnit vic nez je na sklade, na sklad se nic nepricita
 *
 * ean musi byt cislo
 */

typedef struct
{
    char ean[MAX];
    int cnt;
} EAN;

typedef struct
{
    int size;
    int max;
    EAN *sklad;
} TWEAN;

void ini_ean(TWEAN *a)
{
    a->max = 2;
    a->size = 0;
    a->sklad = (EAN *)malloc(a->max * sizeof(*a->sklad));
}

void re_ean(TWEAN *a)
{
    a->max *= 2;
    a->sklad = (EAN *)realloc(a->sklad, a->max * sizeof(*a->sklad));
}

int main(void)
{
    TWEAN t;
    ini_ean(&t);

    EAN tmp;
    int state = 0;
    char c;

    while ((state = scanf(" %c %dx %100s", &c, &tmp.cnt, tmp.ean)) == 3)
    {
        int len = strlen(tmp.ean);
        if ((c != '+' && c != '-') || tmp.cnt < 0 || len < 5)
        {
            printf("Nespravny vstup.\n");
            free(t.sklad);
            return 1;
        }
        for (int i = 0; i < len; i++)
        {
            if (isdigit(tmp.ean[i]) == 0)
            {
                printf("Nespravny vstup.\n");
                free(t.sklad);
                return 1;
            }
        }
        int found = 0;
        for (int i = 0; i < t.size; i++)
        {
            if (strcmp(t.sklad[i].ean, tmp.ean) == 0)
            {
                found = 1;
                switch (c)
                {
                case '+':
                    t.sklad[i].cnt += tmp.cnt;
                    printf("> %dx\n", t.sklad[i].cnt);
                    break;
                case '-':
                    if (tmp.cnt > t.sklad[i].cnt)
                        printf("Nedostatek\n");
                    else
                    {
                        t.sklad[i].cnt -= tmp.cnt;
                        printf("> %dx\n", t.sklad[i].cnt);
                    }
                    break;
                }
            }
            if (found == 1)
                break;
        }
        if (found == 0)
        {
            switch (c)
            {
            case '+':
                t.sklad[t.size].cnt = tmp.cnt;
                printf("> %dx\n",t.sklad[t.size].cnt);
                strcpy(t.sklad[t.size].ean, tmp.ean);
                t.size++;
                if (t.size >= t.max)
                    re_ean(&t);
                break;
            case '-':
                printf("Nedostatek\n");
                break;
            }
        }
    }
    if (state != EOF)
    {
        printf("Nespravny vstup.\n");
        free(t.sklad);
        return 1;
    }

    free(t.sklad);
    return 0;
}