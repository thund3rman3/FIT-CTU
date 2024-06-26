#include <stdio.h>
#include <stdlib.h>

#define MAX 1000000

typedef struct mince
{
    int coin_ar_size;
    int coin_count;
    int *coins;
} MINCE;

// algo ze cvika na rekurze
int get_num(int val, MINCE M)
{
    if (val <= 0)
        return 0;
    int low_enc = MAX;
    int tmp = 0;
    for (int i = 0; i < M.coin_count; i++)  
    {
        if (M.coins[i] <= val)
        {
            tmp = 1 + get_num(val - M.coins[i], M);
            if (tmp < low_enc)
                low_enc = tmp;
        }
    }
    return low_enc;
}

int get_values(MINCE M)
{
    int state = 0;
    int val = 0;
    while ((state = scanf(" %d", &val)) == 1)
    {
        int count = get_num(val, M);

        if (val < 0)
            return 0;
        else if(val == 0)
            printf("= %d\n", val);
        else if (count != 0 && count != MAX)
        {
            printf("= %d\n", count);
        }
        else
            printf("= nema reseni\n");
    }
    if (state != EOF)
        return 0;
    return 1;
}

int get_coins(MINCE *M)
{
    int state = 0;

    while ((state = scanf(" %d", &(*M).coins[M->coin_count])) == 1 && M->coins[M->coin_count] >= 0)
    {
        if (M->coin_count + 1 == M->coin_ar_size)
        {
            M->coin_ar_size *= 2;
            M->coins = (int *)realloc(M->coins, M->coin_ar_size * sizeof(int));
        }
        if (M->coins[M->coin_count] == 0)
            break;
        M->coin_count++;
    }
    if (state != 1 || M->coins[M->coin_count] != 0 || (M->coin_count == 0))
        return 0;
    return 1;
}

int main(void)
{

    printf("Mince:\n");

    MINCE M = {10, 0, (int *)malloc(M.coin_ar_size * sizeof(int))};

    if (get_coins(&M) == 0)
    {
        printf("Nespravny vstup.\n");
        free(M.coins);
        return 1;
    }

    printf("Castky:\n");

    if (get_values(M) == 0)
    {
        printf("Nespravny vstup.\n");
        free(M.coins);
        return 1;
    }

    free(M.coins);
    return 0;
}