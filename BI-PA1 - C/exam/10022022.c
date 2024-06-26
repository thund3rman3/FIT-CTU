#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct titem
{
    char *word;
    struct titem *m_Next;
    struct titem *nextW;
    struct titem *prevW;

} TITEM;

TITEM *createItem(char *value, TITEM *next, TITEM *nextW, TITEM *prevW)
{
    TITEM *n = (TITEM *)malloc(sizeof(*n));
    n->word = strdup(value);
    n->m_Next = next;
    n->nextW = nextW;
    n->prevW = prevW;
    return n;
}

void deleteList(TITEM *l)
{
    while (l)
    {
        TITEM *tmp = l->m_Next;
        free(l->word);
        free(l);
        l = tmp;
    }
}
/*
while (string != NULL)
    {
        sscanf(string,"%s",str[cnt]);
        printf("%s", str[cnt]);
        int len =strlen(str[cnt]);
        string += len;
        cnt++;
        if(cnt>=max)
        {
            max*=2;
            str = (char**)realloc(str,max * sizeof(*str));
        }
    }
    */
TITEM *string_to_list(char *input)
{
    TITEM *res = NULL, *prev = NULL;
    TITEM *head = res;

    int max = 2;
    int cnt = 0;
    char **str = (char **)malloc(max * sizeof(*str));
    char *tmp = strdup(input);
    int len = strlen(tmp);
    if (tmp[len - 1] == '\n')
        tmp[len - 1] = '\0';

    for (str[cnt] = strtok(tmp, " "); str[cnt] != NULL; str[cnt] = strtok(NULL, " "))
    {
        printf("|%s|\n", str[cnt]);

        res = createItem(str[cnt], NULL, NULL, NULL);
        if (cnt == 0)
            head = res;
        res->m_Next = NULL;
        if (prev != NULL)
            prev->m_Next = res;
        prev = res;
        res = res->m_Next;

        cnt++;
        if (cnt >= max)
        {
            max *= 2;
            str = (char **)realloc(str, max * sizeof(*str));
        }
    }

    int i = 0;
    for (TITEM *temp = head; temp != NULL; temp = temp->m_Next, i++)
    {
        int j = 0;
        for (TITEM *temp2 = head; temp2 != NULL; temp2 = temp2->m_Next, j++)
        {
            if(i == j)
                continue;
            else if(strncmp(temp->word, temp2->word, strlen(temp->word)) == 0)
            {
                if(i<j)
                {
                    temp->nextW = temp2;
                    temp2->prevW = temp;
                    printf("temp %s next %d\n",temp->word,j);            
                    break;
                }
                else if(j<i)
                {
                    temp->prevW = temp2;
                    temp2->nextW = temp;
                    printf("temp %s prev %d\n",temp->word,j);
                }
            }
        }
        printf("___________________________________\n");
        
    }

    free(str);
    free(tmp);
    
    return head;
}

int main(void)
{
    char str[200] = "giga big giga t op op giga drop op pop big t t\n";
    TITEM *head = string_to_list(str);
    TITEM* a = head;

    deleteList(head);
    return 0;
}