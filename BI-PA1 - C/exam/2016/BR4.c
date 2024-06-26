#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int x;
    int y;
} COORDINATES;

typedef struct
{
    int size;
    int max;
    COORDINATES *array;
} TWCOO;

void ini_ar(TWCOO *a)
{
    a->max = 2;
    a->size = 0;
    a->array = (COORDINATES *)malloc(a->max * sizeof(a->array));
}

void re_ar(TWCOO *a)
{
    a->max *= 2;
    a->array = (COORDINATES *)realloc(a->array, a->max * sizeof(*a->array));
}

int cmp(const COORDINATES *a, const COORDINATES *b)
{
    if (a->x == b->x)
        return (b->y < a->y) - (a->y < b->y);
    return (b->x < a->x) - (a->x < b->x);
}

int main(void)
{
    TWCOO t;
    ini_ar(&t);

    int state = 0;
    int satX = 0, satY = 0;
    char leva, prava;
    printf("Snimky:\n");
    while ((state = scanf(" [ %d , %d ] %c", &satX, &satY, &leva)) == 3 && leva == '{')
    {

        while ((state = scanf(" %c %d , %d ] %c", &leva, &t.array[t.size].x, &t.array[t.size].y, &prava)) == 4 && (prava == ',' || prava == '}'))
        {
            t.array[t.size].x += satX;
            t.array[t.size].y += satY;
            t.size++;
            if (t.size >= t.max)
                re_ar(&t);

            if (prava == '}')
                break;
        }
        if (state != 4 || prava != '}')
        {
            if(leva == '}') 
                continue;
            printf("Nespravny vstup.\n");
            free(t.array);
            return 1;
        }
    }
    if (state != EOF)
    {
        printf("Nespravny vstup.\n");
        free(t.array);
        return 1;
    }

    qsort(t.array, t.size, sizeof(t.array[0]), (int (*)(const void *, const void *))cmp);

    int uni = 0;
    COORDINATES prev;
    for (int i = 0; i < t.size; i++)
    {
        if (i == 0)
            uni++;
        else
        {
            if (t.array[i].x != prev.x || t.array[i].y != prev.y)
                uni++;
        }
        prev = t.array[i];
    }

    printf("Celkem jednotek: %d\n", uni);

    free(t.array);
    return 0;
}