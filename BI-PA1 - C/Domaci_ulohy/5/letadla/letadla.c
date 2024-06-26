#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NAME_LENGHT 200

typedef struct airplane
{
    double x;
    double y;
    char name[NAME_LENGHT];
} AP;

typedef struct arr
{
    size_t airplanes_size;
    size_t airplanes_count;
    AP *airplanes;
} HANGAR;

typedef struct dist
{
    char name1[NAME_LENGHT];
    char name2[NAME_LENGHT];
} DIST;

/**
 * Print each airplane pair with the shortest distance
 * @param[in] ap_combinations array of airplanes with shortest distance
 * @param[in] comb_index number of airplane pairs
 */
void print_pairs(DIST *ap_combinations, size_t comb_index)
{
    for (size_t index = 0; index < comb_index; index++)
    {
        printf("%s - %s\n", ap_combinations[index].name1, ap_combinations[index].name2);
    }
}

/**
 * Find airplane pairs by the shortest distance
 * @param[in] hangar struct with array of airplanes
 * @param[out] ap_combinations array of airplanes with shortest distance
 * @param[in] min_dist shortest distance
 * @param[out] comb_index number of airplane pairs
 */
void find_pairs(HANGAR *hangar, DIST **ap_combinations, double min_dist, size_t *comb_index)
{
    size_t cnt = *comb_index;
    size_t ap_comb_size = hangar->airplanes_size;
    for (size_t i = 0; i < hangar->airplanes_count; i++)
    {
        for (size_t j = i + 1; j < hangar->airplanes_count; j++)
        {
            double distance = sqrt((hangar->airplanes[i].x - hangar->airplanes[j].x) *
                                       (hangar->airplanes[i].x - hangar->airplanes[j].x) +
                                   (hangar->airplanes[i].y - hangar->airplanes[j].y) *
                                       (hangar->airplanes[i].y - hangar->airplanes[j].y));

            if (fabs(distance) - fabs(min_dist) <= distance * __DBL_EPSILON__ * 1e6)
            {
                if (ap_comb_size == cnt)
                {
                    ap_comb_size *= 2;
                    *ap_combinations = (DIST *)realloc(*ap_combinations, ap_comb_size * sizeof(DIST));
                }
                strncpy((*ap_combinations)[cnt].name1, hangar->airplanes[i].name, sizeof((*ap_combinations)[cnt].name1));
                strncpy((*ap_combinations)[cnt].name2, hangar->airplanes[j].name, sizeof((*ap_combinations)[cnt].name2));
                cnt++;
            }
        }
    }
    *comb_index = cnt;
}

/**
 * Find the shortest distance
 * @param[in] hangar struct with array of airplanes
 * @return shortest distance
 */
double find_distance(HANGAR *hangar)
{
    double min_dist = 0;
    for (size_t i = 0; i < hangar->airplanes_count; i++)
    {
        for (size_t j = i + 1; j < hangar->airplanes_count; j++)
        {
            double distance = sqrt((hangar->airplanes[i].x - hangar->airplanes[j].x) *
                                       (hangar->airplanes[i].x - hangar->airplanes[j].x) +
                                   (hangar->airplanes[i].y - hangar->airplanes[j].y) *
                                       (hangar->airplanes[i].y - hangar->airplanes[j].y));

            if (i == 0 && j == 1)
            {
                min_dist = distance;
            }
            else if (fabs(distance) - fabs(min_dist) <= distance * __DBL_EPSILON__ * 1e6)
            {
                min_dist = distance;
            }
        }
    }
    return min_dist;
}

/**
 * Load collection of airplanes into hangar parameter
 * @param[in] hangar struct with array of airplanes
 * @return 1 = valid input, 0 = error
 */
int read_airplane(HANGAR *hangar)
{
    int state;
    while ((state = scanf(" %lf , %lf : %199s", &(*hangar).airplanes[hangar->airplanes_count].x,
                          &hangar->airplanes[hangar->airplanes_count].y, hangar->airplanes[hangar->airplanes_count].name)) == 3)
    {
        if (hangar->airplanes_count >= hangar->airplanes_size - 1)
        {
            hangar->airplanes_size *= 2;
            hangar->airplanes = (AP *)realloc(hangar->airplanes, hangar->airplanes_size * sizeof(*hangar->airplanes));
        }
        hangar->airplanes_count += 1;
    }
    if (state != EOF || hangar->airplanes_count < 2)
        return 0;

    return 1;
}

int main(void)
{
    HANGAR hangar;
    hangar.airplanes_size = 10;
    hangar.airplanes_count = 0;
    hangar.airplanes = (AP *)malloc(hangar.airplanes_size * sizeof(AP));

    printf("Pozice letadel:\n");

    if (read_airplane(&hangar) == 0)
    {
        printf("Nespravny vstup.\n");
        free(hangar.airplanes);
        return 1;
    }

    DIST *ap_combinations = (DIST *)malloc(hangar.airplanes_size * sizeof(DIST));
    size_t comb_index = 0;

    double min_dist = find_distance(&hangar);
    printf("Vzdalenost nejblizsich letadel: %f\n", min_dist);

    find_pairs(&hangar, &ap_combinations, min_dist, &comb_index);
    printf("Nalezenych dvojic: %zd\n", comb_index);

    print_pairs(ap_combinations, comb_index);

    free(ap_combinations);
    free(hangar.airplanes);
    return 0;
}
