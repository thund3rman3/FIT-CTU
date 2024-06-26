#include <stdio.h>

int foo(int a[],int b[])
{
    return sizeof(a);
}

int main()
{
    int a[20];
    int b[29];
    printf("%d,%d|   %zd, %zd, %d", *a,*b,sizeof(a)/sizeof(a[0]),sizeof(b)/sizeof(b[0]), foo(a,b));

    return 0;
}