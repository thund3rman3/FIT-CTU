#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 12
// 1,2,5,10,20,50,100,200,500,1000,2000,5000 - 12
/**
 *
 */
typedef struct
{
    int times;
    int note;
} BANK;

int main(void)
{
    BANK kasa[MAX];
    BANK tmp[MAX];

    kasa[0].note = 1;
    kasa[1].note = 2;
    kasa[2].note = 5;
    kasa[3].note = 10;
    kasa[4].note = 20;
    kasa[5].note = 50;
    kasa[6].note = 100;
    kasa[7].note = 200;
    kasa[8].note = 500;
    kasa[9].note = 1000;
    kasa[10].note = 2000;
    kasa[11].note = 5000;
    for (int i = 0; i < MAX; i++)
    {
        kasa[i].times = 0;
        tmp[i] = kasa[i];
    }

    int input = 0, state = 0, times = 0, note = 0;
    char leva, prava;
    while ((state = scanf(" %d %c", &input, &leva)) == 2 && leva == '[')
    {
        int fr_input = 0;
        while ((state = scanf(" %dx%d Kc %c", &times, &note, &prava)) == 3 && (prava == ',' || prava == ']'))
        {
            fr_input += note * times;
            int found = 0;
            for (int i = 0; i < MAX; i++)
            {
                if (tmp[i].note == note)
                {
                    found = 1;
                    tmp[i].times += times;
                    break;
                }
            }
            if (found == 0)
            {
                printf("Nespravny vstup.\n");
                return 1;
            }

            if (prava == ']')
                break;
        }
        if (state != 3 || prava != ']')
        {
            printf("Nespravny vstup.\n");
            return 1;
        }
        if (fr_input < input)
        {
            for (int i = 0; i < MAX; i++)
            {
                tmp[i].times = 0;
            }
            printf("Nedostatecna hotovost.\n");
            continue;
        }
        for (int i = 0; i < MAX; i++)
        {
            kasa[i].times += tmp[i].times;
        }

        printf("Kasa: [ ");
        int p = 1;
        for (int i = MAX - 1; i >= 0; i--)
        {
            if (kasa[i].times != 0)
            {
                if (p == 1)
                    printf("%dx%d Kc", kasa[i].times, kasa[i].note);
                else
                    printf(", %dx%d Kc", kasa[i].times, kasa[i].note);
                p++;
            }
        }
        printf(" ]\n");
        printf("Spropitne: %d\n", fr_input - input);
        for (int i = 0; i < MAX; i++)
        {
            tmp[i].times = 0;
        }
    }
    if (state != EOF)
    {
        printf("Nespravny vstup.\n");
        return 1;
    }

    return 0;
}