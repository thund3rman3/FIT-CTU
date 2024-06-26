#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 *
 */

typedef struct
{
    int value;
    char reg;
} REG;

typedef struct
{
    int max;
    int size;
    REG *regions;
} TWREG;

void ini_reg(TWREG *a)
{
    a->max = 2;
    a->size = 0;
    a->regions = (REG *)malloc(a->max * sizeof(*a->regions));
}

void re_reg(TWREG *a)
{
    a->max *= 2;
    a->regions = (REG *)realloc(a->regions, a->max * sizeof(*a->regions));
}

int main(void)
{
    printf("vstup:\n");

    char c;
    if (scanf(" %c", &c) != 1 || c != '{')
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    TWREG t;
    ini_reg(&t);
    int state = 0;
    while ((state = scanf(" %d %c %c", &t.regions[t.size].value, &t.regions[t.size].reg, &c)) == 3)
    {
        if (t.regions[t.size].value < 0 || (c != '}' && c != ',') ||
            t.regions[t.size].reg < 'A' ||
            t.regions[t.size].reg > 'Z')
        {
            printf("Nespravny vstup.\n");
            free(t.regions);
            return 1;
        }
        t.size++;
        if (t.size >= t.max)
            re_reg(&t);
        if (c == '}')
            break;
    }
    if (state != 3)
    {
        printf("Nespravny vstup.\n");
        free(t.regions);
        return 1;
    }

    int len = 0;
    char *str = NULL;
    size_t len_str = 0;
    while ((len = getline(&str, &len_str, stdin)) > 0 && len <= 82)
    {
        if (str[len - 1] == '\n')
            str[len - 1] = '\0';

        for (char *token = strtok(str, " \t"); token != NULL; token = strtok(NULL, "\t "))
        {
            int cnt = 0;
            int sum = 0;
            int toklen = strlen(token);
            for (int i = 0; i < toklen; i++)
            {

                for (int j = 0; j < t.size; j++)
                {
                    if (token[i] == t.regions[j].reg)
                    {
                        cnt++;
                        sum += t.regions[j].value;
                    }
                }
            }
            
            
            if (cnt == 0)
            {
                printf("= N/A\n");
            }
            else
            {
                double avg = 1.0 * sum / cnt;
                printf("= %lf\n", avg);
            }
        }
    }
    if (len != EOF)
    {
        printf("Nespravny vstup.\n");
        free(t.regions);
        free(str);
        return 1;
    }

    free(str);
    free(t.regions);
    return 0;
}
