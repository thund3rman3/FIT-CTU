#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define len 31

typedef struct
{
    char str[len];
} TStr;

int main(void)
{
    int cnt = 0, size = 2, state = 0;
    char star[] = "*****";

    TStr *ar = (TStr *)malloc(size * sizeof(*ar));

    while ((state = scanf(" %30s", ar[cnt].str)) == 1)
    {
        if (strncmp(ar[cnt].str, star, strlen(star)) == 0)
        {
            state = EOF;
            break;
        }

        cnt++;
        if (cnt >= size)
        {
            size *= 2;
            ar = (TStr *)realloc(ar, size * sizeof(*ar));
        }
    }

    if (state != EOF)
    {
        printf("nespravny vstup..\n");
        free(ar);
        return 1;
    }

    char search[len];
    int outlen = 0, found = 0;
    state = 0;
    while ((state = scanf(" %30s", search)) == 1)
    {
        for (int i = 0; i < cnt; i++)
        {
            if (strncmp(ar[i].str, search, strlen(search)) == 0)
            {
                found =1;
                break;
            }
        }

        if(found == 1)
        {
            outlen += strlen(search);
            if (outlen > 80)
            {
                outlen = strlen(search);
                printf("\n");
            }
            printf(" ");
            printf("%s", search);
        }
        else
        {
            outlen += strlen(search)+11;
            if (outlen > 80)
            {
                outlen = strlen(search)+11;
                printf("\n");
            }
            printf(" ");
            printf("<err>%s</err>", search);
        }
        found = 0;
    }

    if (state != EOF)
    {
        printf("nespravny vstup..\n");
        free(ar);
        return 1;
    }

    free(ar);
    return 0;
}