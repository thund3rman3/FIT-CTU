#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 31
/**
 * zadani slovniku:
 *  slova oddelena " \n"
 *  ukonceni zadavani: "*****"
 * zadani kontrolovanych slov:
 *  slova oddelena " \n"
 * slova max 30char
 * EOF->vypis:
 *  case-insensitive
 *  mezi slovy mezera
 *  radka ma 80 charu
 *  kontrolovana slova obsazena ve slovniku normalne vypis
 *                      neobsazena vypis: <err>slovo</err>
 */

typedef struct
{
    char str[MAX];
} Tstring;

int main(void)
{
    int size = 0;
    int max = 2;
    Tstring *slovnik = (Tstring *)malloc(max * sizeof(*slovnik));
    int state = 0;

    printf("Slovnik:\n");
    while ((state = scanf(" %30s", slovnik[size].str)) == 1)
    {
        if (strncmp(slovnik[size].str, "*****", strlen("*****")) == 0)
            break;

        size++;
        if (size >= max)
        {
            max *= 2;
            slovnik = (Tstring *)realloc(slovnik, max * sizeof(*slovnik));
        }
    }
    if (state != 1)
    {
        printf("Nespravny vstup.\n");
        free(slovnik);
        return 1;
    }
    printf("Text:\n");

    int text_size = 0;
    int text_max = 2;
    Tstring *text = (Tstring *)malloc(text_max * sizeof(*text));

    while ((state = scanf(" %30s", text[text_size].str)) == 1)
    {
        text_size++;
        if (text_size >= text_max)
        {
            text_max *= 2;
            text = (Tstring *)realloc(text, text_max * sizeof(*text));
        }
    }
    if (state != EOF)
    {
        printf("Nespravny vstup.\n");
        free(slovnik);
        free(text);
        return 1;
    }
    int first = 1;
    int len = 0;
    int lenerr = strlen("<>err<>/err");

    for (int i = 0; i < text_size; i++)
    {
        int found = 0;
        for (int j = 0; j < size; j++)
        {
            if (strcasecmp(slovnik[j].str, text[i].str) == 0)
            {
               // printf("\n/%s-%s/\n",slovnik[j].str,text[i].str);
                if (first != 1)
                {
                    if (1 + len + strlen(text[i].str) >= 80)
                    {
                        printf("\n%s",text[i].str);
                        len = strlen(text[i].str);
                    }
                    else
                    {
                        len += strlen(text[i].str) + 1;
                        printf(" %s", text[i].str);
                    }
                }
                else
                {
                    len = strlen(text[i].str);
                    printf("%s",text[i].str);
                    first = 0;
                }
                found = 1;
                break;
            }
        }

        if (found == 0)
        {
            if (first != 1)
            {
                if (1 + len + lenerr + strlen(text[i].str) >= 80)
                {
                    printf("\n<err>%s</err>",text[i].str);
                    len = strlen(text[i].str) + lenerr;
                }
                else
                {
                    printf(" <err>%s</err>", text[i].str);
                    len += strlen(text[i].str) + lenerr + 1;
                }
            }
            else
            {
                len = strlen(text[i].str) + lenerr;
                printf("<err>%s</err>",text[i].str);
                first = 0;
            }
        }
    }
    printf("\n");
        free(text);
        free(slovnik);
        return 0;
}