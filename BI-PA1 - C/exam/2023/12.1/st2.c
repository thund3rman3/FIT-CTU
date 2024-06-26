#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 33
/**
 * sprava studentu uchazejicich se o zkousku
 * vstup: 1-3 jmena case-insesitive a muzou byt prohozena
 *  X: jmeno1[max 32 charu] j2 j3
 *  X = 'R' zaregistroval se/'P' prisel
 *  nejdriv se zadavaji R a potom P
 * vystup:
 * vypsat studenty kteri R ale !P , P a !R
 *
 */

int cmpS(const char **a, const char **b)
{
    return strncasecmp(*a, *b, MAX);
}

typedef struct
{
    char **names;
    char **sorted;
    int cnt_jm;
    int max;
    int used =0;
} ST;

typedef struct
{
    int size;
    int max;
    ST *sts;
} TWST;

void ini_st(TWST *s)
{
    s->max = 2;
    s->size = 0;
    s->sts = (ST *)malloc(s->max * sizeof(*s->sts));
    for (int i = 0; i < s->max; i++)
    {
        s->sts[i].cnt_jm = 0;
        s->sts[i].max = 2;
        s->sts[i].names = (char **)malloc(s->sts[i].max * sizeof(char *));
        s->sts[i].sorted = (char **)malloc(s->sts[i].max * sizeof(char *));
    }
}

void re_twst(TWST *s)
{
    int i = s->max;
    s->max *= 2;
    s->sts = (ST *)realloc(s->sts, s->max * sizeof(*s->sts));
    for (; i < s->max; i++)
    {
        s->sts[i].cnt_jm = 0;
        s->sts[i].max = 2;
        s->sts[i].names = (char **)malloc(s->sts[i].max * sizeof(char *));
        s->sts[i].sorted = (char **)malloc(s->sts[i].max * sizeof(char *));
    }
}

void re_st(TWST *s, int i)
{
    s->sts[i].max *= 2;
    s->sts[i].names = (char **)realloc(s->sts[i].names, s->sts[i].max * sizeof(char *));
    s->sts[i].sorted = (char **)realloc(s->sts[i].sorted, s->sts[i].max * sizeof(char *));
}

void rrree(TWST *s, TWST *t)
{

    for (int i = 0; i < s->max; i++)
    {
        for (int j = 0; j < s->sts[i].cnt_jm; j++)
        {
            free(s->sts[i].names[j]);
            free(s->sts[i].sorted[j]);
        }

        free(s->sts[i].names);
        free(s->sts[i].sorted);
    }
    for (int i = 0; i < t->max; i++)
    {
        for (int j = 0; j < t->sts[i].cnt_jm; j++)
        {
            free(t->sts[i].names[j]);
            free(t->sts[i].sorted[j]);
        }
        free(t->sts[i].names);
        free(t->sts[i].sorted);
    }
    free(s->sts);
    free(t->sts);
}

int main(void)
{
    TWST r, p; // reg= R , came = P
    ini_st(&r);
    ini_st(&p);
    int state = 0;
    char c, d;
    printf("Studenti:\n");
    int flag = 0;
    char *str = NULL;
    size_t lenstr = 0;
    int len = 0;
    while ((state = scanf(" %c %c", &c, &d)) == 2)
    {
        if (d != ':' || (c != 'R' && c != 'P') || (c == 'R' && flag == 1) ||
            (len = getline(&str, &lenstr, stdin)) <= 0)
        {
            printf("Nespravny vstup\n");
            rrree(&r, &p);
            free(str);
            return 1;
        }
        if (str[len - 1] == '\n')
            str[len - 1] = '\0';

        if (c == 'R')
        {
            r.sts[r.size].cnt_jm = 0;
            for (char *token = strtok(str, " "); token != NULL; token = strtok(NULL, " "))
            {
                r.sts[r.size].names[r.sts[r.size].cnt_jm] = strdup(token);
                //printf("[%d]%s %s\n", r.sts[r.size].cnt_jm, r.sts[r.size].names[r.sts[r.size].cnt_jm], token);
                r.sts[r.size].sorted[r.sts[r.size].cnt_jm] = strdup(token);
                r.sts[r.size].cnt_jm++;
                r.sts[r.size].used = 0;
                if (r.sts[r.size].cnt_jm >= r.sts[r.size].max)
                    re_st(&r, r.size);
            }
            qsort(r.sts[r.size].sorted, r.sts[r.size].cnt_jm, sizeof(r.sts[r.size].sorted[0]), (int (*)(const void *, const void *))cmpS);
            r.size++;
            if (r.size >= r.max)
                re_twst(&r);
        }
        else if (c == 'P')
        {
            flag = 1;
            p.sts[p.size].cnt_jm = 0;
            for (char *token = strtok(str, " "); token != NULL; token = strtok(NULL, " "))
            {
                p.sts[p.size].names[p.sts[p.size].cnt_jm] = strdup(token);
                p.sts[p.size].sorted[p.sts[p.size].cnt_jm] = strdup(token);
                //printf("[%d]%s %s\n", p.sts[p.size].cnt_jm, p.sts[p.size].names[p.sts[p.size].cnt_jm], token);
                p.sts[p.size].cnt_jm++;
                p.sts[p.size].used = 0;
                if (p.sts[p.size].cnt_jm >= p.sts[p.size].max)
                    re_st(&p, p.size);
            }
             qsort(p.sts[p.size].sorted, p.sts[p.size].cnt_jm, sizeof(p.sts[p.size].sorted[0]), (int (*)(const void *, const void *))cmpS);
            p.size++;
            if (p.size >= p.max)
                re_twst(&p);
        }
    }
    if (state != EOF)
    {
        printf("Nespravny vstup\n");
        rrree(&r, &p);
        free(str);
        return 1;
    }

    printf("Nepritomni: R a !P\n");
    for (int i = 0; i < r.size; i++)
    {
        int found = 0;
        for (int j = 0; j < p.size; j++)
        {
            if (r.sts[i].cnt_jm == p.sts[j].cnt_jm)
            {
                for (int k = 0; k < r.sts[i].cnt_jm; k++)
                {
                    if (strncasecmp(r.sts[i].sorted[k], p.sts[j].sorted[k], MAX) == 0)
                    {
                        if (k == r.sts[i].cnt_jm - 1)
                            found = 1;
                        continue;
                    }
                }
            }
        }
        if (found == 0)
        {
            printf("*");
            for (int j = 0; j < r.sts[i].cnt_jm; j++)
            {
                printf(" %s", r.sts[i].names[j]);
            }
            printf("\n");
        }
    }

    printf("Neregistrvani: !R a P\n");
    for (int i = 0; i < p.size; i++)
    {
        int found = 0;
        for (int j = 0; j < r.size; j++)
        {
            if (p.sts[i].cnt_jm == r.sts[j].cnt_jm)
            {
                for (int k = 0; k < r.sts[j].cnt_jm; k++)
                {
                    if (strncasecmp(r.sts[j].sorted[k], p.sts[i].sorted[k], MAX) == 0)
                    {
                        if (k == r.sts[j].cnt_jm - 1)
                        {   
                            if(r.sts[j].used == 1)
                            {
                                break;
                            }
                            found = 1;
                            r.sts[j].used =1;
                        }
                        continue;
                        
                    }
                    else
                            break;
                }
            }
        }

        if (found == 0)
        {
            printf("*");
            for (int j = 0; j < p.sts[i].cnt_jm; j++)
            {
                printf(" %s", p.sts[i].names[j]);
            }
            printf("\n");
        }
    }
    rrree(&r, &p);
    free(str);
    return 0;
}