#include <stdio.h>
#include <stdlib.h>

#define MAX 100
typedef struct task
{
    int too_much;
    int begin;
    int end;
} TTASK;

typedef struct
{
    TTASK *pole;
    int count;
    int max;
} TTA;

void print_smth(TTA *a, int max)
{
    for (int i = 0; i < a->count; i++)
    {
        // printf("////%d:%d..%d\n",a->pole->too_much,a->pole->begin, a->pole->end);
        if (a->pole[i].too_much == max)
            printf("%d..%d\n", a->pole[i].begin, a->pole[i].end);
    }
}

int find_big(TTA *a)
{
    int max = 0;
    for (int i = 0; i < a->count; i++)
    {
        if (a->pole[i].too_much > max)
            max = a->pole[i].too_much;
    }
    return max;
}

TTASK intersect(TTASK a, TTASK b)
{
    TTASK res = {0, 0, 0};
    if (a.end <= b.begin || b.end <= a.begin)
    {
        return res;
    }
    res.begin = a.begin > b.begin ? a.begin : b.begin;
    res.end = a.end > b.end ? b.end : a.end;
    res.too_much = a.too_much + b.too_much;
    return res;
}

void find_inst(TTA *task, TTA *inst)
{
    // int breakpoints = 0;
    // int b[MAX];
    for (int i = 0; i < task->count; i++)
    {
        // for(int k = 0; k<breakpoints; k++)
        //     if(i == b[k])
        //         continue;
        if (inst->count >= inst->max - 1)
        {
            inst->max *= 2;
            inst->pole = (TTASK *)realloc(inst->pole, inst->max * sizeof(*inst->pole));
        }
        inst->pole[inst->count++] = task->pole[i];

        for (int j = i + 1; j < task->count; j++)
        {
            // for(int k = 0; k<breakpoints; k++)
            //     if(j == b[k])
            //         continue;
            TTASK comp = intersect(task->pole[i], task->pole[j]);
            if (comp.begin == 0 && comp.end == 0 && comp.begin == 0)
                continue;
            // printf("::%d:%d..%d\n", inst->pole[i].too_much,inst->pole[i].begin,inst->pole[i].end);
            // printf("::%d:%d..%d\n", task->pole[j].too_much,task->pole[j].begin,task->pole[j].end);
            inst->pole[inst->count++] = comp;
            // printf("--::%d:%d..%d\n", inst->pole[i].too_much,inst->pole[i].begin,inst->pole[i].end);
            // b[breakpoints] = j;
            // breakpoints++;
        }
    }
}

int read(TTA *tasks_struct)
{
    int state = 0;
    while ((state = scanf(" %d: %d..%d", &tasks_struct->pole[tasks_struct->count].too_much,
                          &tasks_struct->pole[tasks_struct->count].begin,
                          &tasks_struct->pole[tasks_struct->count].end)) == 3)
    {
        if (tasks_struct->count >= tasks_struct->max - 1)
        {
            tasks_struct->max *= 2;
            tasks_struct->pole = (TTASK *)realloc(tasks_struct->pole, tasks_struct->max * sizeof(*tasks_struct->pole));
        }
        tasks_struct->count++;
    }
    if (state != EOF)
    {
        printf("bad guy\n");
        return 0;
    }
    return 1;
}

int main(void)
{
    TTA tasks_struct;
    tasks_struct.count = 0;
    tasks_struct.max = 10;
    tasks_struct.pole = (TTASK *)malloc(tasks_struct.max * sizeof(*tasks_struct.pole));
    // printf("%ld, %ld\n", sizeof(*tasks_struct.pole), sizeof(tasks_struct.pole));

    if (read(&tasks_struct) == 0)
    {
        free(tasks_struct.pole);
        return 1;
    }

    TTA inst_struct;
    inst_struct.count = 0;
    inst_struct.max = 10;
    inst_struct.pole = (TTASK *)malloc(inst_struct.max * sizeof(*inst_struct.pole));
    int max = 0;

    find_inst(&tasks_struct, &inst_struct);
    max = find_big(&inst_struct);

    printf("nejvic : %d\n", max);
    for (int i = 0; i < inst_struct.count; i++)
        printf("++::%d:%d..%d\n", inst_struct.pole[i].too_much, inst_struct.pole[i].begin, inst_struct.pole[i].end);
    print_smth(&inst_struct, max);

    free(inst_struct.pole);
    free(tasks_struct.pole);
    return 0;
}