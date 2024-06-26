#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct TItem
{
  struct TItem *m_Next;
  char *m_Name;
  char m_Secret[24];
} TITEM;

int cmpName(const TITEM *a,
            const TITEM *b)
{
  return strcmp(a->m_Name, b->m_Name);
}

int cmpNameInsensitive(const TITEM *a,
                       const TITEM *b)
{
  return strcasecmp(a->m_Name, b->m_Name);
}

int cmpNameLen(const TITEM *a,
               const TITEM *b)
{
  size_t la = strlen(a->m_Name), lb = strlen(b->m_Name);
  return (lb < la) - (la < lb);
}
#endif /* __PROGTEST__ */

TITEM *newItem(const char *name, TITEM *next)
{
  TITEM *tmp = (TITEM *)malloc(sizeof(TITEM));
  tmp->m_Name = strdup(name);
  tmp->m_Next = next;
  return tmp;
}

void freeList(TITEM *src)
{
  if (src == NULL)
    return;

  TITEM *tmp = src->m_Next;
  free(src->m_Name);
  free(src);
  freeList(tmp);
}
/*
        if (cmpFn(l, tm) > 0)
        {
          char *tmp = l->m_Name;
          l->m_Name = strdup(tm->m_Name);
          tm->m_Name = strdup(tmp);
          if (cnt == 1)
            res = tm;
          free(tmp);
        }
        tm = next(tm);*/

// porovnavam 1.prvek s n ostatnimi, 2. s n-1 atd
// prohazuju Name, podle cmp
TITEM *sortListCmp(TITEM *l, int ascending, int (*cmpFn)(const TITEM *, const TITEM *))
{
  if (l == NULL || l->m_Next == NULL) {
    return l;
  }
  int swap = 1;
  //bubble sort
  //projizdi list odznova
  while (swap) 
  {
    swap = 0;
    TITEM* prev = NULL;
    TITEM* curr = l;
    //porovnava kazde 2 prvky po sobe
    while (curr != NULL && curr->m_Next != NULL) 
    {
      if((ascending != 0 && cmpFn(curr, curr->m_Next) > 0) ||
        (ascending == 0 && cmpFn(curr, curr->m_Next) < 0))
      {
          TITEM* tmp = curr->m_Next;
          curr->m_Next = curr->m_Next->m_Next;
          tmp->m_Next = curr;
          //pri prvnim porovanani se meni pocatek listu
          if (prev == NULL)
            l = tmp;
          else 
            prev->m_Next = tmp;
          swap = 1;
      }
      prev = curr;
      curr = curr->m_Next;
    }
  }
  return l;
  
  }


#ifndef __PROGTEST__
int main(int argc, char *argv[])
{
  TITEM *l;
  char tmp[50];

  assert(sizeof(TITEM) == sizeof(TITEM *) + sizeof(char *) + 24 * sizeof(char));
  l = NULL;
  l = newItem("BI-PA1", l);
  l = newItem("BIE-PA2", l);
  l = newItem("NI-PAR", l);
  l = newItem("lin", l);
  l = newItem("AG1", l);
  assert(l && !strcmp(l->m_Name, "AG1"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "BIE-PA2"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  l = sortListCmp(l, 1, cmpName);
  assert(l && !strcmp(l->m_Name, "AG1"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "BIE-PA2"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  l = sortListCmp(l, 1, cmpNameInsensitive);
  assert(l && !strcmp(l->m_Name, "AG1"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "BIE-PA2"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  l = sortListCmp(l, 1, cmpNameLen);
  assert(l && !strcmp(l->m_Name, "AG1"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "BIE-PA2"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  l = newItem("AAG.3", l);
  assert(l && !strcmp(l->m_Name, "AAG.3"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "AG1"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Next->m_Name, "BIE-PA2"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  l = sortListCmp(l, 0, cmpNameLen);
  assert(l && !strcmp(l->m_Name, "BIE-PA2"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "AAG.3"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "AG1"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  freeList(l);
  l = NULL;
  strncpy(tmp, "BI-PA1", sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';
  l = newItem(tmp, l);
  strncpy(tmp, "BIE-PA2", sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';
  l = newItem(tmp, l);
  strncpy(tmp, "NI-PAR", sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';
  l = newItem(tmp, l);
  strncpy(tmp, "lin", sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';
  l = newItem(tmp, l);
  strncpy(tmp, "AG1", sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';
  l = newItem(tmp, l);
  assert(l && !strcmp(l->m_Name, "AG1"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "BIE-PA2"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  l = sortListCmp(l, 1, cmpName);
  assert(l && !strcmp(l->m_Name, "AG1"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "BIE-PA2"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  l = sortListCmp(l, 1, cmpNameInsensitive);
  assert(l && !strcmp(l->m_Name, "AG1"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "BIE-PA2"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  l = sortListCmp(l, 1, cmpNameLen);
  assert(l && !strcmp(l->m_Name, "AG1"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "BIE-PA2"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  strncpy(tmp, "AAG.3", sizeof(tmp) - 1);
  tmp[sizeof(tmp) - 1] = '\0';
  l = newItem(tmp, l);
  assert(l && !strcmp(l->m_Name, "AAG.3"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "AG1"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Next->m_Name, "BIE-PA2"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  l = sortListCmp(l, 0, cmpNameLen);
  assert(l && !strcmp(l->m_Name, "BIE-PA2"));
  assert(l->m_Next && !strcmp(l->m_Next->m_Name, "BI-PA1"));
  assert(l->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Name, "NI-PAR"));
  assert(l->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Name, "AAG.3"));
  assert(l->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Name, "AG1"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next && !strcmp(l->m_Next->m_Next->m_Next->m_Next->m_Next->m_Name, "lin"));
  assert(l->m_Next->m_Next->m_Next->m_Next->m_Next->m_Next == NULL);
  freeList(l);
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
