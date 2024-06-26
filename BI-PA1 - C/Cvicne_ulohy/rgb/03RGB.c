#include <stdio.h>

int main ( void ) 
{
    int x,y,z;
    char zavorek;
    printf("Zadejte barvu v RGB formatu:\n");

    if(scanf(" rgb ( %d , %d , %d %c", &x, &y, &z, &zavorek)!=4 
        || x<0 || x>255 || y<0 || y>255 || z<0 || z>255 || zavorek!=')')
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    printf("#%02X%02X%02X\n", x, y, z);

    return 0;
}