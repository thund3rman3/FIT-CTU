#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * domino kostky s 3/4/5 cisly
 * format : [1,1,1]...
 * najdi pocet unikatu
 */

typedef struct
{
    int k[3];
} T3;

typedef struct
{
    T3 *domino3;
    int size;
    int max;
} TW_T3;

typedef struct
{
    int k[4];
} T4;

typedef struct
{
    T4 *domino4;
    int size;
    int max;
} TW_T4;

typedef struct
{
    int k[5];
} T5;

typedef struct
{
    T5 *domino5;
    int size;
    int max;
} TW_T5;

int cmp3(const T3 *a, const T3 *b)
{
    if (a->k[0] != b->k[0])
        return (b->k[0] < a->k[0]) - (a->k[0] < b->k[0]);
    if (a->k[1] != b->k[1])
        return (b->k[1] < a->k[1]) - (a->k[1] < b->k[1]);
    if (a->k[2] != b->k[2])
        return (b->k[2] < a->k[2]) - (a->k[2] < b->k[2]);
    return 0;
}

int cmp4(const T4 *a, const T4 *b)
{
    if (a->k[0] != b->k[0])
        return (b->k[0] < a->k[0]) - (a->k[0] < b->k[0]);
    if (a->k[1] != b->k[1])
        return (b->k[1] < a->k[1]) - (a->k[1] < b->k[1]);
    if (a->k[2] != b->k[2])
        return (b->k[2] < a->k[2]) - (a->k[2] < b->k[2]);
    if (a->k[3] != b->k[3])
        return (b->k[3] < a->k[3]) - (a->k[3] < b->k[3]);
    return 0;
}

int cmp5(const T5 *a, const T5 *b)
{
    if (a->k[0] != b->k[0])
        return (b->k[0] < a->k[0]) - (a->k[0] < b->k[0]);
    if (a->k[1] != b->k[1])
        return (b->k[1] < a->k[1]) - (a->k[1] < b->k[1]);
    if (a->k[2] != b->k[2])
        return (b->k[2] < a->k[2]) - (a->k[2] < b->k[2]);
    if (a->k[3] != b->k[3])
        return (b->k[3] < a->k[3]) - (a->k[3] < b->k[3]);
    if (a->k[4] != b->k[4])
        return (b->k[4] < a->k[4]) - (a->k[4] < b->k[4]);
    return 0;
}

int main(void)
{
    char *str = NULL;
    size_t lenstr = 0;
    int len = 0;
    int i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0;
    char c1, c2, c3;

    TW_T3 trojky;
    trojky.max = 2;
    trojky.size = 0;
    trojky.domino3 = (T3 *)malloc(trojky.max * sizeof(*trojky.domino3));

    TW_T4 ctyrky;
    ctyrky.max = 2;
    ctyrky.size = 0;
    ctyrky.domino4 = (T4 *)malloc(ctyrky.max * sizeof(*ctyrky.domino4));

    TW_T5 petky;
    petky.max = 2;
    petky.size = 0;
    petky.domino5 = (T5 *)malloc(petky.max * sizeof(*petky.domino5));

    while ((len = getline(&str, &lenstr, stdin)) > 0)
    {
        if (str[len - 1] == '\n')
            str[len - 1] = '\0';

        int num = sscanf(str, " [ %d , %d , %d %c %d %c %d %c", &i1, &i2, &i3, &c1, &i4, &c2, &i5, &c3);
        //printf("%d\n", num);
        if ((num == 4 && (i1 < 0 || i2 < 0 || i3 < 0 || c1 != ']')) ||
            (num == 6 && (i1 < 0 || i2 < 0 || i3 < 0 || c1 != ',' || i4 < 0 || c2 != ']')) ||
            (num == 8 && (i1 < 0 || i2 < 0 || i3 < 0 || c1 != ',' ||
                          c2 != ',' || c3 != ']' || i4 < 0 || i5 < 0)) ||
            num < 4 || num == 5 || num == 7 || num > 8)
        {
            len = -2;
            break;
        }
        else if (num == 4)
        {
            trojky.domino3[trojky.size].k[0] = i1;
            trojky.domino3[trojky.size].k[1] = i2;
            trojky.domino3[trojky.size].k[2] = i3;
            //printf("%d %d %d\n", trojky.domino3[trojky.size].k[0], trojky.domino3[trojky.size].k[1], trojky.domino3[trojky.size].k[2]);
            trojky.size++;
            if (trojky.size >= trojky.max)
            {
                trojky.max *= 2;
                trojky.domino3 = (T3 *)realloc(trojky.domino3, trojky.max * sizeof(*trojky.domino3));
            }
        }
        else if (num == 6)
        {
            ctyrky.domino4[ctyrky.size].k[0] = i1;
            ctyrky.domino4[ctyrky.size].k[1] = i2;
            ctyrky.domino4[ctyrky.size].k[2] = i3;
            ctyrky.domino4[ctyrky.size].k[3] = i4;
            //printf("%d %d %d %d\n", ctyrky.domino4[ctyrky.size].k[0], ctyrky.domino4[ctyrky.size].k[1], ctyrky.domino4[ctyrky.size].k[2], ctyrky.domino4[ctyrky.size].k[3]);
            ctyrky.size++;
            if (ctyrky.size >= ctyrky.max)
            {
                ctyrky.max *= 2;
                ctyrky.domino4 = (T4 *)realloc(ctyrky.domino4, ctyrky.max * sizeof(*ctyrky.domino4));
            }
        }
        else if (num == 8)
        {
            petky.domino5[petky.size].k[0] = i1;
            petky.domino5[petky.size].k[1] = i2;
            petky.domino5[petky.size].k[2] = i3;
            petky.domino5[petky.size].k[3] = i4;
            petky.domino5[petky.size].k[4] = i5;
            //printf("%d %d %d %d %d\n", petky.domino5[petky.size].k[0], petky.domino5[petky.size].k[1], petky.domino5[petky.size].k[2], petky.domino5[petky.size].k[3], petky.domino5[petky.size].k[4]);
            petky.size++;
            if (petky.size >= petky.max)
            {
                petky.max *= 2;
                petky.domino5 = (T5 *)realloc(petky.domino5, petky.max * sizeof(*petky.domino5));
            }
        }
    }
    if (len != EOF)
    {
        printf("Nespravny vstup.\n");
        free(str);
        free(trojky.domino3);
        free(ctyrky.domino4);
        free(petky.domino5);
        return 1;
    }
    printf("//////////////////// \n");
    for (int i = 0; i < trojky.size; i++)
    {
        printf("%d %d %d\n", trojky.domino3[i].k[0], trojky.domino3[i].k[1], trojky.domino3[i].k[2]);
    }
    for (int i = 0; i < ctyrky.size; i++)
    {
        printf("%d %d %d %d\n", ctyrky.domino4[i].k[0], ctyrky.domino4[i].k[1], ctyrky.domino4[i].k[2], ctyrky.domino4[i].k[3]);
    }
    for (int i = 0; i < petky.size; i++)
    {
        printf("%d %d %d %d %d\n", petky.domino5[i].k[0], petky.domino5[i].k[1], petky.domino5[i].k[2], petky.domino5[i].k[3], petky.domino5[i].k[4]);
    }
    printf("////////////////////////////\n");
    
    
    

    for (int i = 0; i < trojky.size; i++)
    {
        int min = __INT_MAX__;
        int j = 0, idj = 0;
        for (j = 0; j < 3; j++)
        {
            if (trojky.domino3[i].k[j] < min)
            {
                min = trojky.domino3[i].k[j];
                idj = j;
            }
        }
        int pootoceni = 0;
        if (idj == 1)
            pootoceni = 2;
        else if (idj == 2)
            pootoceni = 1;
        for (int k = 0; k < pootoceni; k++)
        {
            int tmp = trojky.domino3[i].k[2];
            trojky.domino3[i].k[2] = trojky.domino3[i].k[1];
            trojky.domino3[i].k[1] = trojky.domino3[i].k[0];
            trojky.domino3[i].k[0] = tmp;
        }
    }

    for (int i = 0; i < ctyrky.size; i++)
    {
        int min = __INT_MAX__;
        int j = 0, idj = 0;
        for (j = 0; j < 4; j++)
        {
            if (ctyrky.domino4[i].k[j] < min)
            {
                min = ctyrky.domino4[i].k[j];
                idj = j;
            }
        }
        int pootoceni = 0;
        if (idj == 1)
            pootoceni = 3;
        else if (idj == 2)
            pootoceni = 2;
        else if (idj == 3)
            pootoceni = 1;

        for (int k = 0; k < pootoceni; k++)
        {
            int tmp = ctyrky.domino4[i].k[3];
            ctyrky.domino4[i].k[3]  = ctyrky.domino4[i].k[2];
            ctyrky.domino4[i].k[2] = ctyrky.domino4[i].k[1];
            ctyrky.domino4[i].k[1] = ctyrky.domino4[i].k[0];
            ctyrky.domino4[i].k[0] = tmp;
        }
    }

    for (int i = 0; i < petky.size; i++)
    {
        int min = __INT_MAX__;
        int j = 0, idj = 0;
        for (j = 0; j < 5; j++)
        {
            if (petky.domino5[i].k[j] < min)
            {
                min = petky.domino5[i].k[j];
                idj = j;
            }
        }
        int pootoceni = 0;
        if (idj == 1)
            pootoceni = 4;
        else if (idj == 2)
            pootoceni = 3;
        else if (idj == 3)
            pootoceni = 2;
        else if(idj == 4)
            pootoceni =1;

        for (int k = 0; k < pootoceni; k++)
        {
            int tmp = petky.domino5[i].k[4];
            petky.domino5[i].k[4] = petky.domino5[i].k[3];
            petky.domino5[i].k[3] = petky.domino5[i].k[2];
            petky.domino5[i].k[2] = petky.domino5[i].k[1];
            petky.domino5[i].k[1] = petky.domino5[i].k[0];
            petky.domino5[i].k[0] = tmp; 
        }
    }

for (int i = 0; i < trojky.size; i++)
    {
        printf("%d %d %d\n", trojky.domino3[i].k[0], trojky.domino3[i].k[1], trojky.domino3[i].k[2]);
    }
    for (int i = 0; i < ctyrky.size; i++)
    {
        printf("%d %d %d %d\n", ctyrky.domino4[i].k[0], ctyrky.domino4[i].k[1], ctyrky.domino4[i].k[2], ctyrky.domino4[i].k[3]);
    }
    for (int i = 0; i < petky.size; i++)
    {
        printf("%d %d %d %d %d\n", petky.domino5[i].k[0], petky.domino5[i].k[1], petky.domino5[i].k[2], petky.domino5[i].k[3], petky.domino5[i].k[4]);
    }
    printf("/////////////////// \n");

    if (trojky.size > 1)
        qsort(trojky.domino3, trojky.size, sizeof(trojky.domino3[0]), (int (*)(const void *, const void *))cmp3);
    if (ctyrky.size > 1)
        qsort(ctyrky.domino4, ctyrky.size, sizeof(ctyrky.domino4[0]), (int (*)(const void *, const void *))cmp4);
    if (petky.size > 1)
        qsort(petky.domino5, petky.size, sizeof(petky.domino5[0]), (int (*)(const void *, const void *))cmp5);

for (int i = 0; i < trojky.size; i++)
    {
        printf("%d %d %d\n", trojky.domino3[i].k[0], trojky.domino3[i].k[1], trojky.domino3[i].k[2]);
    }
    for (int i = 0; i < ctyrky.size; i++)
    {
        printf("%d %d %d %d\n", ctyrky.domino4[i].k[0], ctyrky.domino4[i].k[1], ctyrky.domino4[i].k[2], ctyrky.domino4[i].k[3]);
    }
    for (int i = 0; i < petky.size; i++)
    {
        printf("%d %d %d %d %d\n", petky.domino5[i].k[0], petky.domino5[i].k[1], petky.domino5[i].k[2], petky.domino5[i].k[3], petky.domino5[i].k[4]);
    }
    printf("/////////////////////// \n");


    int unique = 0;
    T3 prev3;
    T4 prev4;
    T5 prev5;
    for (int i = 0; i < trojky.size; i++)
    {
        if (i == 0)
        {
            unique++;
        }
        else
        {   
            if (prev3.k[0] != trojky.domino3[i].k[0] ||
                prev3.k[1] != trojky.domino3[i].k[1] ||
                prev3.k[2] != trojky.domino3[i].k[2])
                unique++;
        }
        prev3 = trojky.domino3[i];
    }

    for (int i = 0; i < ctyrky.size; i++)
    {
        if (i == 0)
        {
            unique++;
        }
        else
        {
            if (prev4.k[0] != ctyrky.domino4[i].k[0] ||
                prev4.k[1] != ctyrky.domino4[i].k[1] ||
                prev4.k[2] != ctyrky.domino4[i].k[2] ||
                prev4.k[3] != ctyrky.domino4[i].k[3])
                unique++;
        }
        prev4 = ctyrky.domino4[i];
    }

    for (int i = 0; i < petky.size; i++)
    {
        if (i == 0)
        {
            unique++;
        }
        else
        {
            if (prev5.k[0] != petky.domino5[i].k[0] ||
                prev5.k[1] != petky.domino5[i].k[1] ||
                prev5.k[2] != petky.domino5[i].k[2] ||
                prev5.k[3] != petky.domino5[i].k[3] ||
                prev5.k[4] != petky.domino5[i].k[4] )
                unique++;
        }
        prev5 = petky.domino5[i];
    }
    printf("UNIKATNI: %d\n", unique);

    free(trojky.domino3);
    free(ctyrky.domino4);
    free(petky.domino5);
    free(str);
    return 0;
}