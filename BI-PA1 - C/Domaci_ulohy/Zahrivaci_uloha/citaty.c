#include <stdio.h>

int main(void)
{
    int in;
    char c;
    printf("ml' nob:\n");
    if (scanf(" %d", &in) != 1 || in<1 || in>9 )
    {
            printf("luj\n");
            return 1;  
    }
    
    scanf("%c", &c);
    
    if(c!='\n' && c!=' ' && c!='\t' && !feof(stdin))
    {
        printf("luj\n");
        return 1;  
    }
   

    printf("Qapla'\n");

    switch (in)
    {
    case 1:
        printf("noH QapmeH wo' Qaw'lu'chugh yay chavbe'lu' 'ej wo' choqmeH may' DoHlu'chugh lujbe'lu'.\n");
        break;
    case 2:
        printf("bortaS bIr jablu'DI' reH QaQqu' nay'.\n");
        break;
    case 3:
        printf("Qu' buSHa'chugh SuvwI', batlhHa' vangchugh, qoj matlhHa'chugh, pagh ghaH SuvwI''e'.\n");
        break;
    case 4:
        printf("bISeH'eghlaH'be'chugh latlh Dara'laH'be'.\n");
        break;
    case 5:
        printf("qaStaHvIS wa' ram loS SaD Hugh SIjlaH qetbogh loD.\n");
        break;
    case 6:
        printf("Suvlu'taHvIS yapbe' HoS neH.\n");
        break;
    case 7:
        printf("Ha'DIbaH DaSop 'e' DaHechbe'chugh yIHoHQo'.\n");
        break;
    case 8:
        printf("Heghlu'meH QaQ jajvam.\n");
        break;
    case 9:
        printf("leghlaHchu'be'chugh mIn lo'laHbe' taj jej.\n");
        break;
    default:
        printf("luj\n");
        return 1;
    }

    return 0;

}