#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

typedef struct TItem
{
    struct TItem *m_Next;
    int m_Mul; // nasobek
    int m_Pow; // mocnina
} TITEM;       // mul*x^pow

TITEM *createItem(int mul, int pow, TITEM *next)
{
    TITEM *n = (TITEM *)malloc(sizeof(*n));
    n->m_Mul = mul;
    n->m_Pow = pow;
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
#endif /* __PROGTEST__ */

 int test(TITEM *a)
{
    if(a == NULL || (a->m_Mul == 0 && a->m_Pow != 0))
        return 0;
    TITEM* prev = a;
    a = a->m_Next;
    while(a != NULL)
    {
        if(prev->m_Mul == 0 && prev->m_Pow == 0 && a != NULL)
            return 0;
        if(prev->m_Pow >= a->m_Pow || (a->m_Mul == 0 && a->m_Pow != 0))
            return 0;
        
        prev = a;
        a=a->m_Next;
    }
    return 1;
}

TITEM *addPoly(TITEM *a, TITEM *b)
{
    TITEM *res, *res_head;
    res = createItem(0, 0, NULL);
    res_head = res;
    int first =1;
    if(test(a) == 0 || test(b) == 0)
        return NULL;
    while (666)
    {
        if (a == NULL && b == NULL)
            break;
        
        else if (b == NULL || a->m_Pow < b->m_Pow)
        {
            if(a->m_Pow == 0 && a->m_Mul == 0)
            {
                a = a->m_Next;
                continue;
            }
            if(first != 1)
            {
            res->m_Next = createItem(0, 0, NULL);
        res = res->m_Next;
            }
            first = 0;
            res->m_Mul = a->m_Mul;
            res->m_Pow = a->m_Pow;
            a = a->m_Next;
        }
        else if (a == NULL || a->m_Pow > b->m_Pow)
        {
            if(b->m_Pow == 0 && b->m_Mul == 0)
            {
                b = b->m_Next;
                continue;
            }
            if(first != 1)
            {
            res->m_Next = createItem(0, 0, NULL);
        res = res->m_Next;
            }
            first = 0;
            res->m_Pow = b->m_Pow;
            res->m_Mul = b->m_Mul;
            b = b->m_Next;
        }
        else if (a->m_Pow == b->m_Pow)
        {
            int x = a->m_Mul + b->m_Mul;
            if(x == 0)
            {
                if(a->m_Pow == 0 && b->m_Pow == 0 && a->m_Mul == 0 && b->m_Mul == 0 && a->m_Next == NULL && b->m_Next == NULL)
                    return res_head;
                a = a->m_Next;
                b = b->m_Next;
                continue;
            }
            if(first != 1)
            {
            res->m_Next = createItem(0, 0, NULL);
        res = res->m_Next;
            }
            first = 0;
            res->m_Pow = a->m_Pow;
            res->m_Mul = x;
            a = a->m_Next;
            b = b->m_Next;
        }
        if (a == NULL && b == NULL)
            break;
        
    }
    return res_head;
}

#ifndef __PROGTEST__
int main(int argc, char *argv[])
{
    TITEM *a, *b;
    TITEM *res;

    a = createItem(2, 1, NULL);
    b = createItem(0, 0, NULL);
    res = addPoly(a, b);
    assert(res->m_Mul == 2);
    assert(res->m_Pow == 1);
    assert(res->m_Next == NULL);
    deleteList(a);
    deleteList(b);
    deleteList(res);

    a = createItem(2, 1, NULL);
    b = createItem(3, 1, createItem(4, 2, createItem(2, 3, createItem(1, 0, NULL))));
    res = addPoly(a, b);
    assert(res == NULL);
    deleteList(a);
    deleteList(b);
    deleteList(res);

    a = createItem(2, 1, NULL);
    b = createItem(3, 1, createItem(4, 1, NULL));
    res = addPoly(a, b);
    assert(res == NULL);
    deleteList(a);
    deleteList(b);
    deleteList(res);

    a = createItem(3, 0, createItem(2, 1, createItem(9, 3, NULL)));
    b = createItem(0, 0, createItem(4, 2, createItem(-1, 3, NULL)));
    res = addPoly(a, b);
    assert(res == NULL);
    deleteList(a);
    deleteList(b);
    deleteList(res);

    a = createItem(3, 0, createItem(2, 1, createItem(5, 3, NULL)));
    b = createItem(-7, 0, createItem(-2, 1, createItem(-5, 3, NULL)));
    res = addPoly(a, b);
    assert(res->m_Mul == -4);
    assert(res->m_Pow == 0);
    assert(res->m_Next == NULL);
    deleteList(a);
    deleteList(b);
    deleteList(res);

    a = createItem(3, 1, createItem(-2, 2, createItem(4, 3, NULL)));
    b = createItem(-3, 1, createItem(2, 2, createItem(-4, 3, NULL)));
    res = addPoly(a, b);
    assert(res->m_Mul == 0);
    assert(res->m_Pow == 0);
    assert(res->m_Next == NULL);
    deleteList(a);
    deleteList(b);
    deleteList(res);

    return 0;
}
#endif /* __PROGTEST__ */