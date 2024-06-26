#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 11
/**
 * parkoviste eviduje dobu parkovani
 * vstup:
 *  hodina:minuta * RZ
 *  0:00 -> vypis aut, *=parkuje, bez*=neparkuje uz,
 *      doba parkovani a pak serazena sestupne podle doby parkovani
 */

typedef struct
{
    int h_in;
    int min_in;
    int time;
    int parks;
    char RZ[MAX];
} PARK;

typedef struct
{
    int size;
    int max;
    PARK *park;
} TWPARK;

void ini_park(TWPARK *a)
{
    a->max = 2;
    a->size = 0;
    a->park = (PARK *)malloc(a->max * sizeof(*a->park));
}

void re_park(TWPARK *a)
{
    a->max *= 2;
    a->park = (PARK *)realloc(a->park, a->max * sizeof(*a->park));
}

int cmp(const PARK* a, const PARK* b)
{
    if(a->time == b->time)
        return strcmp(a->RZ, b->RZ);
    return (a->time < b->time) - (b->time < a->time);
}

int main(void)
{
    TWPARK t;
    ini_park(&t);

    int state = 0;
    int prev_hour_record = 0;
    char c;
    int hour = 0, min = 0;
    printf("-Log:\n");
    while ((state = scanf(" %d : %d %c", &hour, &min, &c)) == 3)
    {
        if (hour > 23 || hour < 0 || min < 0 || min > 59 || (c != '*' && c != '='))
        {
            printf("Nespravny vstup.\n");
            free(t.park);
            return 1;
        }
        if (c == '=')
        {
            prev_hour_record = 0;
            for (int i = 0; i < t.size; i++)
            {
                if(t.park[i].parks == 1)
                {
                    t.park[i].time += 24*60 - t.park[i].h_in*60 - t.park[i].min_in;
                    t.park[i].h_in = 0;
                    t.park[i].min_in = 0;
                }
            }
            qsort(t.park,t.size,sizeof(t.park[0]),(int(*)(const void*,const void*))cmp);
            for (int i = 0; i < t.size; i++)
            {
                if(t.park[i].parks == 0)
                    printf("- %s %d:%02d\n", t.park[i].RZ,t.park[i].time/60, t.park[i].time%60);
                else
                    printf("-* %s %d:%02d\n",t.park[i].RZ, t.park[i].time/60, t.park[i].time%60);
            }
            
            
        }
        else if (c == '*')
        {
            int tmp = hour * 60 + min;
            if (prev_hour_record > tmp)
            {
                printf("Nespravny vstup.\n");
                free(t.park);
                return 1;
            }
            prev_hour_record = tmp;
            char RZ[MAX];
            if (scanf(" %10s", RZ) != 1)
            {
                printf("Nespravny vstup.\n");
                free(t.park);
                return 1;
            }
            int found = 0;
            for (int i = 0; i < t.size; i++)
            {
                if(strcmp(RZ,t.park[i].RZ)==0)
                {
                    found =1;
                    if(t.park[i].parks ==0)
                    {
                        t.park[i].parks=1;
                        t.park[i].h_in = hour;
                        t.park[i].min_in = min;
                        printf("Vjezd\n");
                    }
                    else if(t.park[i].parks == 1)
                    {
                        t.park[i].parks=0;
                        t.park[i].time += hour*60+min - t.park[i].h_in*60 - t.park[i].min_in;
                        printf("Odjezd, pakovani celkem: %d:%02d\n", t.park[i].time/60, t.park[i].time%60);
                    }
                    break;
                }
            }
            if( found==0)
            {
                strcpy(t.park[t.size].RZ, RZ);
                t.park[t.size].h_in = hour;
                t.park[t.size].min_in = min;
                t.park[t.size].time = 0;
                t.park[t.size].parks = 1;
                t.size++;
                if(t.size>=t.max)
                    re_park(&t);
                printf("Vjezd\n");
            }
            
        }
    }
    if (state != EOF)
    {
        printf("Nespravny vstup.\n");
        free(t.park);
        return 1;
    }

    free(t.park);
    return 0;
}