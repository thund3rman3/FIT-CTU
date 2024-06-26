#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Program which checks if taget is located in radar radius

//target structure - x,y coordinates
typedef struct t_object
{
    double x;
    double y;
} T_OBJECT;

//radar structure - x,y coordinates and range=radius
typedef struct r_object
{
    double x;
    double y;
    double range;
} R_OBJECT;


/*Prints out radars and number of targets in radius
 *
 */
void print_radars(int count_size_R, int count_size_T, R_OBJECT *object_array_R, T_OBJECT *object_array_T)
{
    for (int radar_index = 0; radar_index < count_size_R; radar_index++)
    {
        int encountered = 0; 
        for (int taget_index = 0; taget_index < count_size_T; taget_index++)
        {
            // sqrt((x1-x2)^2+(y2-y1)^2)
            double distance = sqrt(((object_array_T)[taget_index].x - (object_array_R)[radar_index].x) * ((object_array_T)[taget_index].x - (object_array_R)[radar_index].x) +
                                ((object_array_T)[taget_index].y - (object_array_R)[radar_index].y) * ((object_array_T)[taget_index].y - (object_array_R)[radar_index].y));
            if (distance - (object_array_R)[radar_index].range <= distance * __DBL_EPSILON__)
                encountered++;
        }
        printf("Radar #%d: %d\n", radar_index, encountered);
    }
}


/*Reads radars into structure
 *
 */
R_OBJECT *read_R(int *count_size_R, int *size_R, R_OBJECT **object_array_R)
{
    if (*count_size_R == *size_R - 1)
    {
        *size_R *= 2;
        *object_array_R = (R_OBJECT *)realloc(*object_array_R, *size_R * sizeof(R_OBJECT));
    }
    if (scanf(" %lf %lf %lf", &(*object_array_R)[*count_size_R].x, &(*object_array_R)[*count_size_R].y, &(*object_array_R)[*count_size_R].range) != 3 ||
        (*object_array_R)[*count_size_R].range <= 0)
        return NULL;

    //printf("x=%f y=%f r=%f\n", (*object_array_R)[*count_size_R].x, (*object_array_R)[*count_size_R].y, (*object_array_R)[*count_size_R].range);
    *count_size_R += 1;
    return *object_array_R;
}



/*Reads targets into structure
 *
 */
T_OBJECT *read_T(int *count_size_T, int *size_T, T_OBJECT **object_array_T)
{
    if (*count_size_T == *size_T - 1)
    {
        *size_T *= 2;
        *object_array_T = (T_OBJECT *)realloc(*object_array_T, *size_T * sizeof(T_OBJECT));
    }
    if (scanf(" %lf %lf", &(*object_array_T)[*count_size_T].x, &(*object_array_T)[*count_size_T].y) != 2)
        return NULL;
    //printf("x=%f y=%f\n", (*object_array_T)[*count_size_T].x, (*object_array_T)[*count_size_T].y);
    *count_size_T += 1;

    return *object_array_T;
}


/*Reads input till EOF
 *
 */
int read(int *count_size_R, int *count_size_T, R_OBJECT **object_array_R, T_OBJECT **object_array_T, int *size_R, int *size_T)
{
    int state;
    char type;

    while ((state = scanf(" %c", &type)) == 1 &&
           (type == 'R' || type == 'T'))
    {
        if (type == 'R')
        {
            if ((*object_array_R = read_R(&(*count_size_R), &(*size_R), &(*object_array_R))) == NULL)
                return 0;
        }
        else if (type == 'T')
            if ((*object_array_T = read_T(&(*count_size_T), &(*size_T), &(*object_array_T))) == NULL)
                return 0;
    }
    if (state != EOF || (*count_size_R == 0 && *count_size_T == 0))
        return 0;

    return 1;
}

int main(void)
{
    int count_size_R = 0;
    int count_size_T= 0;
    int size_T = 10;
    int size_R = 10;
    T_OBJECT *object_array_T = (T_OBJECT *)malloc(size_T * sizeof(T_OBJECT));
    R_OBJECT *object_array_R = (R_OBJECT *)malloc(size_R * sizeof(R_OBJECT));

    printf("Seznam cilu a radaru:\n");
    if (read(&count_size_R, &count_size_T, &object_array_R,
             &object_array_T, &size_R, &size_T) == 0)
    {
        printf("Nespravny vstup.\n");

        free(object_array_T);
        free(object_array_R);
        return 1;
    }
    printf("Pokryti:\n");

    print_radars(count_size_R, count_size_T, object_array_R, object_array_T);

    free(object_array_T);
    free(object_array_R);
    return 0;
}