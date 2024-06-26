#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    char* res;
    int num;
}T_RES;

int main(void)
{
    int jmena_size = 10;
    int jmena_count = 0;
    char **jmena = (char **)malloc(jmena_size * sizeof(*jmena));
    char *str = NULL;
    size_t line_len = 0;
    while (getline(&str, &line_len, stdin) > 0)
    {
        char *token = NULL, *last = NULL;
        last = token = strtok(str, " ");
        while ((token = strtok(NULL, " ")) != NULL)
        {
            jmena[jmena_count] = strdup(last);
            last = token;

            jmena_count++;
            if (jmena_count >= jmena_size)
            {
                jmena_size *= 2;
                jmena = (char **)realloc(jmena, jmena_size * sizeof(*jmena));
            }
        }
    }

    for (int i = 0; i < jmena_count; i++)
    {
        for (int j = i + 1; j < jmena_count; j++)
        {
            if (strcmp(jmena[i], jmena[j]) > 0)
            {
                char *tmp = jmena[i];
                // free(jmena[i]);
                jmena[i] = jmena[j];
                // free(jmena[j]);
                jmena[j] = tmp;

                // free(tmp);
            }
        }
    }

    for (int i = 0; i < jmena_count; i++)
        printf("%s\n", jmena[i]);

    int max = 0;
   // char *res = NULL;
    int tmp_num = 1;

    printf("pocet: ");

    int cnt = 0;
    int res_max = 2;
    T_RES* t  = (T_RES*)malloc(res_max* sizeof(*t));

    for (int i = 0; i < jmena_count;)
    {
        tmp_num = 1;
        for (int j = i + 1; j < jmena_count; j++)
        {
            if (strcmp(jmena[i], jmena[j]) == 0)
            {
                tmp_num++;
            }
            else break;
        }
        if(max < tmp_num)
            max = tmp_num;
        t[cnt].res = strdup(jmena[i]);
        t[cnt].num = tmp_num;
        cnt++;
            if (cnt >= res_max)
            {
                res_max *= 2;
                t = (T_RES*)realloc(t, res_max * sizeof(*t));
            }
        i += tmp_num;
    }
    printf("%d\n", max);
    for (int i = 0; i < cnt; i++)
        if(t[i].num == max)
            printf("%s\n", t[i].res);


    free(str);
    for (int i = 0; i < jmena_count; i++)
    {
        free(jmena[i]);
    }
    for (int i = 0; i < cnt; i++)
    {
        free(t[i].res);
    }
    
    free(jmena);
    free(t);
    return 0;
}