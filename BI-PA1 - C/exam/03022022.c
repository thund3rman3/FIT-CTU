#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

typedef struct TItem
{
    struct TItem *m_Next;
    int m_Val;
} TITEM;

typedef struct
{
    int num;
    int cnt;
} T_RES;

TITEM *createItem(int value, TITEM *next)
{
    TITEM *n = (TITEM *)malloc(sizeof(n));
    n->m_Val = value;
    n->m_Next = next;
    return n;
}

void deleteList(TITEM *l)
{
    while (l)
    {
        TITEM *tmp = l->m_Next;
        free(l);
        l = tmp;
    }
}

TITEM *cloneLimit(TITEM *a, int limit)
{
    TITEM* head = a, *prev = NULL;
    int max = 2;
    int cnt = 0;
    int found = 0;
    T_RES *res = (T_RES *)malloc(max * sizeof(*res));
    for (; a != NULL;  a = a->m_Next)
    {
        for (int i = 0; i < cnt; i++)
        {
            if(res[i].num == a->m_Val)
            {
                if(res[i].cnt >= limit)
                {
                    if(a->m_Next == NULL)
                        prev->m_Next = NULL;
                    else{
                        prev->m_Next = a->m_Next;
                    }
                    found = 1;
                    break;
                }
                else{
                    found = 1;
                    res[i].cnt++;
                    prev = a;
                }
            }
        }
        if(found != 1)
        {
            res[cnt].num = a->m_Val;
            res[cnt].cnt = 1;
            cnt++;
            if(cnt >= max)
            {
                max*=2;
                res = (T_RES*)realloc(res, max * sizeof(*res));
            }
            prev = a;
        }
        found = 0;
        
    }
    free(res);
    return head;
}


int main()
{
    TITEM* a, *head;
    a = createItem(1,createItem(2,createItem(1,createItem(6,createItem(1, createItem(2,NULL))))));
    head = a;
    for (; a != NULL; a = a->m_Next)
    {
        printf("%d ", a->m_Val);
    }
    printf("\n");
    a = cloneLimit(head,1);
    for (; a != NULL; a = a->m_Next)
    {
        printf("%d ", a->m_Val);
    }
    printf("\n");
    deleteList(head);


        a = createItem(1,createItem(2,createItem(1,createItem(6,createItem(1, createItem(2,
        createItem(6,(createItem(2,(createItem(0,createItem(6,NULL))))))))))));
    head =a;
    for (; a != NULL; a = a->m_Next)
    {
        printf("%d ", a->m_Val);
    }
    printf("\n");
    a = cloneLimit(head,2);
    for (; a != NULL; a = a->m_Next)
    {
        printf("%d ", a->m_Val);
    }
    deleteList(head);
    printf("\n");

        a = createItem(1,createItem(1,createItem(1,createItem(2,createItem(2, createItem(2,
        createItem(6,(createItem(2,(createItem(1,createItem(1,NULL))))))))))));
    head =a;
    for (; a != NULL; a = a->m_Next)
    {
        printf("%d ", a->m_Val);
    }
    printf("\n");
    a = cloneLimit(head,4);
    for (; a != NULL; a = a->m_Next)
    {
        printf("%d ", a->m_Val);
    }
    printf("\n");
    deleteList(head);    
    return 0;
}