#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 33
/**
 * sprava student uchazejici se o zkousku
 * vstup: 1-3 jmena case-insesitive a muzou byt prohozena
 *  X: jmeno1[max 32 charu] j2 j3
 *  X = 'R' zaregistroval se/'P' prisel
 *  nejdriv se zadavaji R a potom P
 * vystup:
 * vypsat studenty kteri R ale !P , P a !R
 *
 */

typedef struct
{
    char name1[MAX];
    char name3[MAX];
    char name2[MAX];
    int num_names;
} Tstud;

typedef struct
{
    Tstud *studs;
    int size;
    int max;
} TW_stud;

void ini_stud(TW_stud *st)
{
    st->max = 2;
    st->size = 0;
    st->studs = (Tstud *)malloc(st->max * sizeof(*st->studs));
}

void reallocate_stud(TW_stud *st)
{
    (st)->max *= 2;
    (st)->studs = (Tstud *)realloc((st)->studs, (st)->max * sizeof(*((st)->studs)));
}

int cmp(const Tstud *a, const Tstud *b)
{
    return strcasecmp(a->name1, b->name1);
}

int input(TW_stud *s)
{
    char *str = NULL;
    size_t lenstr = 0;
    int len = 0;
    if ((len = getline(&str, &lenstr, stdin)) < 0)
        return 0;
    if (str[len - 1] == '\n')
        str[len - 1] = '\0';
    // printf("|%s| len=%d, strlen=%zd\n", str, len, strlen(str));
    s->studs[s->size].num_names = 0;
    for (char *token = strtok(str, " \t"); token != NULL; token = strtok(NULL, " \t"))
    {
        if (s->studs[s->size].num_names == 0)
            strncpy(s->studs[s->size].name1, token,MAX);
        else if (s->studs[s->size].num_names == 1)
            strncpy(s->studs[s->size].name2, token, MAX);
        else if (s->studs[s->size].num_names == 2)
            strncpy(s->studs[s->size].name3, token, MAX);
        s->studs[s->size].num_names++;
    }
    for (int i = 0; i < s->studs[s->size].num_names; i++)
    {
        if (i == 0)
            printf("%s ", s->studs[s->size].name1);
        else if (i == 1)
            printf("%s ", s->studs[s->size].name2);
        else if (i == 2)
            printf("%s ", s->studs[s->size].name3);
    }
    printf("\n");
    s->size++;
    if (s->size >= s->max)
        reallocate_stud(s);

    free(str);
    return 1;
}

int main(void)
{
    printf("Studenti:\n");

    int state = 0, flag = 0;
    char c, d;
    TW_stud stR, stP;
    ini_stud(&stR);
    ini_stud(&stP);

    while ((state = scanf(" %c %c", &c, &d)) == 2 && d == ':')
    {
        switch (c)
        {
        case 'R':
        {
            if (flag != 0 || input(&stR) == 0)
            {
                printf("Nespravny vstup.\n");
                free(stP.studs);
                free(stR.studs);
                return 1;
            }
            break;
        }
        case 'P':
        {
            flag = 1;
            if (input(&stP) == 0)
            {
                printf("Nespravny vstup.\n");
                free(stP.studs);
                free(stR.studs);
                return 1;
            }
            break;
        }
        default:
        {
            printf("Nespravny vstup.\n");
            free(stP.studs);
            free(stR.studs);
            return 1;
        }
        }
    }
    if (state != EOF || d != ':')
    {
        printf("Nespravny vstup.\n");
        free(stP.studs);
        free(stR.studs);
        return 1;
    }
    qsort(stR.studs, stR.size, sizeof(Tstud), (int (*)(const void *, const void *))cmp);
    qsort(stP.studs, stP.size, sizeof(Tstud), (int (*)(const void *, const void *))cmp);
    printf("\n");/*
    for (int j = 0; j < stR.size; j++)
    {
        for (int i = 0; i < stR.studs[j].num_names; i++)
        {
            if (i == 0)
                printf("%s ", stR.studs[j].name1);
            else if (i == 1)
                printf("%s ", stR.studs[j].name2);
            else if (i == 2)
                printf("%s ", stR.studs[j].name3);
        }
        printf("\n");
    }
    printf("\n");
    for (int j = 0; j < stP.size; j++)
    {
        for (int i = 0; i < stP.studs[j].num_names; i++)
        {
            if (i == 0)
                printf("%s ", stP.studs[j].name1);
            else if (i == 1)
                printf("%s ", stP.studs[j].name2);
            else if (i == 2)
                printf("%s ", stP.studs[j].name3);
        }
        printf("\n");
    }
    printf("\n");*/

    printf("bonus...Registrovani a prisli:\n");
    for (int j = 0; j < stR.size; j++)
    {

        for (int i = 0; i < stP.size; i++)
        {
            int found = 0;
            if (stR.studs[j].num_names == stP.studs[i].num_names)
            {
                for (int k = 0; k < stR.studs[j].num_names; k++)
                {
                    switch (k)
                    {
                    case 0:
                        if (strcasecmp(stR.studs[j].name1, stP.studs[i].name1) != 0)
                            found = 1;
                        break;
                    case 1:
                        if (strcasecmp(stR.studs[j].name2, stP.studs[i].name2) != 0)
                            found = 1;
                        break;
                    case 2:
                        if (strcasecmp(stR.studs[j].name3, stP.studs[i].name3) != 0)
                            found = 1;
                        break;
                    }
                    

                }
                if (found == 1)
                    continue;
                for (int k = 0; k < stR.studs[j].num_names; k++)
                {
                    if (k == 0)
                        printf("%s ", stR.studs[j].name1);
                    else if (k == 1)
                        printf("%s ", stR.studs[j].name2);
                    else if (k == 2)
                        printf("%s ", stR.studs[j].name3);
                }
                printf("\n");
            }
        }
    }

    printf("Nespritomni:R,!P\n");
    for (int j = 0; j < stR.size; j++)
    {
        int found = 0;
        for (int i = 0; i < stP.size; i++)
        {
            if (stR.studs[j].num_names == stP.studs[i].num_names)
            {
                for (int k = 0; k < stR.studs[j].num_names; k++)
                {
                    if (k == 0)
                    {
                        if (strcasecmp(stR.studs[j].name1, stP.studs[i].name1) == 0)
                        {
                            if (k == stR.studs[j].num_names - 1)
                            {
                                found = 1;
                            }
                        }
                        else break;
                    }
                    if (k == 1)
                    {
                        if (strcasecmp(stR.studs[j].name2, stP.studs[i].name2) == 0)
                        {
                            if (k == stR.studs[j].num_names - 1)
                            {
                                found = 1;
                            }
                        }
                        else break;
                    }
                    if (k == 2)
                    {
                        if (strcasecmp(stR.studs[j].name1, stP.studs[i].name1) == 0)
                        {
                            if (k == stR.studs[j].num_names - 1)
                            {
                                found = 1;
                            }
                        }
                        else break;
                    }
                }
                if (found == 1)
                    break;
                
            }
        }
        if(found == 0)
        {
            //printf("j = %d, num names %d\n", j,stR.studs[j].num_names);
            for (int k = 0; k < stR.studs[j].num_names; k++)
                {
                    if (k == 0)
                        printf("%s ", stR.studs[j].name1);
                    else if (k == 1)
                        printf("%s ", stR.studs[j].name2);
                    else if (k == 2)
                        printf("%s ", stR.studs[j].name3);
                }
                printf("\n");
        }
    }
    printf("Bez registrace:!R,P\n");
     for (int j = 0; j < stP.size; j++)
    {
        int found = 0;
        for (int i = 0; i < stR.size; i++)
        {
            if (stR.studs[i].num_names == stP.studs[j].num_names)
            {
                for (int k = 0; k < stR.studs[i].num_names; k++)
                {
                    if (k == 0)
                    {
                        if (strcasecmp(stR.studs[i].name1, stP.studs[j].name1) == 0)
                        {
                            if (k == stR.studs[i].num_names - 1)
                            {
                                found = 1;
                            }
                        }
                        else break;
                    }
                    if (k == 1)
                    {
                        if (strcasecmp(stR.studs[i].name2, stP.studs[j].name2) == 0)
                        {
                            if (k == stR.studs[i].num_names - 1)
                            {
                                found = 1;
                            }
                        }
                        else break;
                    }
                    if (k == 2)
                    {
                        if (strcasecmp(stR.studs[i].name1, stP.studs[j].name1) == 0)
                        {
                            if (k == stR.studs[i].num_names - 1)
                            {
                                found = 1;
                            }
                        }
                        else break;
                    }
                }
                if (found == 1)
                    break;
                
            }
        }
        if(found == 0)
        {
            for (int k = 0; k < stP.studs[j].num_names; k++)
                {
                    if (k == 0)
                        printf("%s ", stP.studs[j].name1);
                    else if (k == 1)
                        printf("%s ", stP.studs[j].name2);
                    else if (k == 2)
                        printf("%s ", stP.studs[j].name3);
                }
                printf("\n");
        }
    }
    free(stP.studs);
    free(stR.studs);
    return 0;
}