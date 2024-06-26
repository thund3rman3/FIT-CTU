#include <stdio.h>
#include <stdlib.h>

#define AR_MAX 2000
#define SUM_MAX AR_MAX *(AR_MAX - 1) / 2

int Cmp(const void *a, const void *b)
{
    const int *aPtr = (const int *)a;
    const int *bPtr = (const int *)b;
    return (*bPtr < *aPtr) - (*aPtr < *bPtr);
}

void count_sums(int sum[], int index, int *cnt_sum)
{
    int prev = sum[0];
    int cnt = 1;

    for (int i = 1; i < index; i++)
    {
        if (sum[i] == prev)
            cnt++;
        else if (sum[i] != prev && cnt != 1)
        {
            // printf("dvojic je %d se souctem %d\n", cnt*(cnt-1)/2,prev);
            *cnt_sum += cnt * (cnt - 1) / 2;
            cnt = 1;
        }
        prev = sum[i];
    }
}

void fill_sum_array_with_sums_of_intervals(int sum[], int arr[], int ar_size, int *index)
{
    for (int j = 0; j < ar_size; j++)
    {
        for (int k = j + 1; k < ar_size; k++, (*index)++)
        {
            sum[*index] = arr[j];
            for (int l = j + 1; l <= k; l++)
            {
                sum[*index] += arr[l];
            }
        }
    }

    qsort(sum, *index, sizeof(sum[0]), Cmp);
}

int read_input(int state, int *ar_size, int arr[])
{
    while ((state = scanf(" %d", &arr[*ar_size])) == 1)
    {
        *ar_size+=1;
        if (*ar_size > AR_MAX)
        {
            break;
        }
    }
    if (state != EOF || *ar_size == 0)
    {
        printf("Nespravny vstup.\n");
        return 0;
    }
    return 1;
}

int main()
{
    int arr[AR_MAX];
    int state=0, ar_size = 0;
    printf("Posloupnost:\n");

    if(read_input(state,&ar_size,arr)==0)
        return 1;

    int sum[SUM_MAX];
    int index = 0;

    fill_sum_array_with_sums_of_intervals(sum,arr,ar_size,&index);
    int cnt_sum = 0;
    count_sums(sum,index,&cnt_sum);

    printf("Pocet dvojic: %d\n", cnt_sum);
    return 0;
}
