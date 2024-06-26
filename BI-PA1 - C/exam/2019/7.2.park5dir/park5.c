#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 11
/**
 * parkovaci dum
 * vstup:
 *  1.:velikost domu
 *      format: max_patro[A-Z] mista_v_patre
 *  dalsi:
 *      prijezd: + SPZ[max 10char bez mezer]
 *          potom co se odpspodu obsadi vsechna mista, se obsazuji
 *          nejdele prazdna mista
 *      odjezd: - SPZ
 *
 * vystup:
 *  prijedzd: zobrazeni prirazeneho mista
 *      pokud je park. plne, odtahne se nejdele stojici auto
 *  odjezd: zobrazeni opusteneho mista
 *      pokud SPZ neex. pak odmitne pozadavek
 *
 * pocet park mist >0
 *
 */

typedef struct
{
    int full;
    char spz[MAX];
    int spend;
} PLACE;

typedef struct
{
    PLACE *floor_places;
} FLOOR;

typedef struct
{
    FLOOR *parking_floor;
    int full;
    int cnt;
    int cap;
    int fl_idx;
    int pl_idx;
    int spend;
} PARKHOUSE;

int main(void)
{
    printf("Velikost:\n");

    int places = 0;
    char max_floors;
    if (scanf(" %c %d", &max_floors, &places) != 2 || places < 1 ||
        max_floors - 'A' < 0 || max_floors - 'Z' > 0)
    {
        printf("Nespravny vstup.\n");
        return 1;
    }
    int floors = max_floors - 'A' + 1;

    PARKHOUSE p;
    p.cnt = 0;
    p.full = 0;
    p.fl_idx = 0;
    p.pl_idx = 0;
    p.spend = 1;
    p.cap = floors * places;
    int first = 1;
    p.parking_floor = (FLOOR *)malloc(floors * sizeof(*p.parking_floor));
    for (int i = 0; i < floors; i++)
    {
        p.parking_floor[i].floor_places = (PLACE *)malloc(places * sizeof(PLACE));
        for (int j = 0; j < places; j++)
        {
            p.parking_floor[i].floor_places[j].full = 0;
            printf("%d ", p.parking_floor[i].floor_places[j].full);
        }
    }
    int state = 0;
    char tmp[MAX], c;
    printf("Pozadavky:\n");

    while ((state = scanf(" %c %10s", &c, tmp)) == 2)
    {
        if (p.full == 1)
            first = 0;

        switch (c)
        {
        case '+':
        {
            if (p.full == 1)
            {
                int idxi = 0, idxj = 0;
                int spend = __INT_MAX__;
                for (int i = 0; i < floors; i++)
                {
                    for (int j = 0; j < places; j++)
                    {
                        if (p.parking_floor[i].floor_places[j].spend < spend)
                        {
                            spend = p.parking_floor[i].floor_places[j].spend;
                            idxi = i;
                            idxj = j;
                        }
                    }
                }
                printf("Odtah vozu %s z pozice %c%d\n", p.parking_floor[idxi].floor_places[idxj].spz, idxi + 'A', idxj + 1);
                p.parking_floor[idxi].floor_places[idxj].spend = p.spend;
                p.spend++;
                strcpy(p.parking_floor[idxi].floor_places[idxj].spz, tmp);
                printf("Pozice %c%d\n", idxi + 'A', idxj + 1);
            }
            else if (first == 1)
            {
                p.parking_floor[p.fl_idx].floor_places[p.pl_idx].full = 1;
                strcpy(p.parking_floor[p.fl_idx].floor_places[p.pl_idx].spz, tmp);
                p.parking_floor[p.fl_idx].floor_places[p.pl_idx].spend = p.spend;
                p.spend++;
                printf("Prijezd Pozice: %c%d\n", p.fl_idx + 'A', p.pl_idx + 1);
                p.pl_idx++;
                p.cnt++;
                if (p.pl_idx >= places)
                {
                    p.pl_idx = 0;
                    p.fl_idx++;
                    if (p.fl_idx >= floors)
                    {
                        p.fl_idx = 0;
                        if (p.cnt >= p.cap)
                            p.full = 1;
                    }
                }
            }
            else
            {
                int flag =0;
                for (int i = 0; i < floors; i++)
                {
                    for (int j = 0; j < places; j++)
                    {
                        if (p.parking_floor[i].floor_places[j].full ==0)
                        {
                            p.parking_floor[i].floor_places[j].full =1;
                            p.parking_floor[i].floor_places[j].spend = p.spend;
                            p.spend++;
                            strcpy(p.parking_floor[i].floor_places[j].spz, tmp);
                            flag=1;
                            break;
                        }
                    }
                    if(flag==1)
                        break;
                }
                p.cnt++;
                if (p.cnt >= p.cap)
                    p.full = 1;
            }
            break;
        }
        case '-':
        {
            int found = 0;
            for (int i = 0; i < floors; i++)
            {
                for (int j = 0; j < places; j++)
                {
                    if (p.parking_floor[i].floor_places[j].full == 1)
                    {
                        if (strcmp(p.parking_floor[i].floor_places[j].spz, tmp) == 0)
                        {
                            printf("Odjezd Pozice: %c%d\n", i + 'A', j + 1);
                            p.parking_floor[i].floor_places[j].full = 0;
                            strncpy(p.parking_floor[i].floor_places[j].spz, "\0", 1);
                            if (p.cnt == p.cap)
                                p.full = 0;
                            p.cnt--;
                            found = 1;
                        }
                    }
                    if (found == 1)
                        break;
                }
                if (found == 1)
                    break;
            }
            if (found == 0)
            {
                printf("Nenalezeno, odmitnuti pozadavku.\n");
            }

            break;
        }
        default:
            printf("Nespravny vstup.\n");
            for (int i = 0; i < floors; i++)
            {
                free(p.parking_floor[i].floor_places);
            }
            free(p.parking_floor);
            return 1;
        }
    }
    if (state != EOF)
    {
        printf("Nespravny vstup.\n");
        for (int i = 0; i < floors; i++)
        {
            free(p.parking_floor[i].floor_places);
        }
        free(p.parking_floor);
        return 1;
    }
    for (int i = 0; i < floors; i++)
    {
        free(p.parking_floor[i].floor_places);
    }
    free(p.parking_floor);
    return 0;
}