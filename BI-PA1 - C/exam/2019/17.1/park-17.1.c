#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 11
/**
 *
 */

typedef struct
{
    int free;
    char RZ[MAX];
    int time;
} PLACE;

typedef struct
{
    PLACE *place;
} FLOOR;

typedef struct
{
    FLOOR *floor;
    int cap;
    int cnt;
    char flrs;
    char plcs;
} PARKHOUSE;

void ini_park(PARKHOUSE *p)
{
    p->cap = (p->plcs + 1) * (p->flrs + 1);
    p->cnt = 0;
    p->floor = (FLOOR *)malloc((p->flrs + 1) * sizeof(*p->floor));
    for (int i = 0; i < p->flrs + 1; i++)
    {
        p->floor[i].place = (PLACE *)malloc((p->plcs + 1) * sizeof(PLACE));
        for (int j = 0; j < p->plcs + 1; j++)
        {
            p->floor[i].place[j].free = 1;
            p->floor[i].place[j].time = 0;
        }
    }
}

void frrree(PARKHOUSE *p)
{
    for (int i = 0; i < p->flrs+1; i++)
    {
        free(p->floor[i].place);
    }
    free(p->floor);
}

int main(void)
{
    PARKHOUSE p;
    printf("Velikost:\n");
    if (scanf(" %c %c", &p.flrs, &p.plcs) != 2 || p.flrs - 'A' < 0 ||
        p.flrs - 'Z' > 0 || p.plcs - 'a' < 0 || p.plcs - 'z' > 0)
    {
        printf("N\n");
        return 1;
    }
    p.flrs -= 'A';
    p.plcs -= 'a';

    ini_park(&p);
    int state = 0;
    int first = 1;
    char c, tmp[MAX];
    int flidx = 0, plidx = 0;
    int time = 0;
    while ((state = scanf(" %c %10s", &c, tmp)) == 2)
    {
        if (c != '+' && c != '-')
        {
            printf("N\n");
            frrree(&p);
            return 1;
        }
        if (c == '+')
        {
            if (p.cap - p.cnt == 0)
            {
                printf("plne obsazeno\n");
                first = 0;
            }
            else if (first == 1)
            {
                time += 1;
                p.floor[flidx].place[plidx].time = time;
                p.floor[flidx].place[plidx].free = 0;
                strcpy(p.floor[flidx].place[plidx].RZ, tmp);
                p.cnt++;
                printf("Pozice: %c%c, zbyva: %d\n",flidx+'A',plidx+'a',p.cap-p.cnt);
                plidx++;

                if (plidx >= p.plcs+1)
                {
                    plidx = 0;
                    flidx++;
                }
                
            }
            else
            {
                int found = __INT_MAX__;
                for (int i = 0; i < p.flrs + 1; i++)
                {
                    for (int j = 0; j < p.plcs + 1; j++)
                    {
                        if (p.floor[i].place[j].time < found && p.floor[i].place[j].free == 1)
                        {
                            found = p.floor[i].place[j].time;
                        }
                    }
                }
                for (int i = 0; i < p.flrs + 1; i++)
                {
                    for (int j = 0; j < p.plcs + 1; j++)
                    {
                        if (p.floor[i].place[j].time == found && p.floor[i].place[j].free == 1)
                        {
                            time += 1;
                            p.floor[i].place[j].time = time;
                            p.floor[i].place[j].free = 0;
                            strcpy(p.floor[i].place[j].RZ, tmp);
                            p.cnt++;
                            printf("Pozice: %c%c, zbyva: %d\n",i+'A',j+'a',p.cap-p.cnt);
                            break;
                        }
                    }
                }
            }
        }
        else if (c == '-')
        {
            int found = 0;
            for (int i = 0; i < p.flrs + 1; i++)
            {
                for (int j = 0; j < p.plcs + 1; j++)
                {
                    if (p.floor[i].place[j].free == 0)
                    {
                        if (strncmp(p.floor[i].place[j].RZ, tmp, MAX) == 0)
                        {
                            p.floor[i].place[j].free = 1;
                            time++;
                            p.floor[i].place[j].time = time;
                            p.cnt--;
                            found = 1;
                            printf("Pozice: %c%c,zbyva: %d\n", i + 'A', j + 'a', p.cap - p.cnt);
                            break;
                        }
                    }
                }
                if (found == 1)
                    break;
            }
            if (found == 0)
                printf("Nenalezeno\n");
        }
    }
    if (state != EOF)
    {
        printf("N\n");
        frrree(&p);
        return 1;
    }
    frrree(&p);
    return 0;
}