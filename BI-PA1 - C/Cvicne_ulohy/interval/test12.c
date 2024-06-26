 #include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int t1_ms;
    int t;
    for(int i = 100;i>=1;i/=10){
        t=0;
        t = (int)getchar()-48;
        if(t>0 && t<10){
            t1_ms+=t*i;
        }
        if(t==-38){
            break;
        }
    }
    printf("%d\n",t1_ms);
    return 0;
}