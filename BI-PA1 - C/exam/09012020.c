#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char str[200];
    int cnt = 0;
    printf("napis slovo:\n");
    scanf(" %199s", str);
    int len = strlen(str);
    for (int i = 1; i < len - 1; i++)
    {
        char temp[200];
        char prev;
        strncpy(temp, str, strlen(str));
        
        for (int j = i; j < len; j++)
        {
            
            if(j==i)
            {
                prev=temp[i];
                temp[j] = ' ';
            }
            else
            {
                temp[j] = prev;
            }
            prev=temp[j];
            
        }
        cnt++;
        printf("%s\n", temp);
    }
    printf("%s\n", str);
    printf("celkem %d\n", cnt);
    return 0;
}