#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 0 = insesitive-nezavisly na velikosti pismen, 1 = sensitive

int is_palindrom_2(char *text)
{
    size_t len = strlen(text);

    for (size_t index = 0; index < len / 2; index++)
    {
        if (tolower(text[index]) != tolower(text[len - index - 1]))
            return 0;
    }
    return 1;
}

int is_palindrom_1(char *text)
{
    size_t len = strlen(text);

    for (size_t index = 0; index < len / 2; index++)
    {
        if (text[index] != text[len - index - 1])
            return 0;
    }
    return 1;
}

char *removeSpaces(char *str)
{
    size_t len = strlen(str);
    char *res = str;
    int not_ws = 0;
    for (size_t index = 0; index < len; index++)
    {
        if (!isspace(str[index]))
        {
            res[not_ws++] = str[index];
        }
    }
    if (not_ws == 0 || str[len - 1] != '\n')
    {
        free(str);
        return NULL;
    }

    res[not_ws] = '\0';
    return res;
}

int main(void)
{
    size_t text_size = 0;
    char *text = NULL;
    int chars = 0;

    printf("Zadejte retezec:\n");
    while ((chars = getline(&text, &text_size, stdin)) != EOF )
    {
        if ((text = removeSpaces(text)) == NULL || *text == '\n')
        {
            free(text);
            printf("Nespravny vstup.\n");
            return 1;
        }

        if (is_palindrom_1(text) == 1)
            printf("Retezec je palindrom (case-sensitive).\n");

        else if (is_palindrom_2(text) == 1)
            printf("Retezec je palindrom (case-insensitive).\n");

        else
            printf("Retezec neni palindrom.\n");
    }
    if (chars != EOF)
    {
        free(text);
        printf("Nespravny vstup.\n");
        return 1;
    }

    free(text);
    return 0;
}
