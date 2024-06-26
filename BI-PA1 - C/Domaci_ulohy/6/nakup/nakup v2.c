#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define STR_MAX 1000

typedef struct o_list
{
    int regal_num;
    char *regal_product;
    char *list_product;
} O_LIST;

typedef struct optimize
{
    int optimized_max;
    int optimized_count;
    O_LIST *opt_list;
} OPT;

typedef struct s_seznam
{
    int product_count;
    int product_max;
    char **products;
} S_LIST;

typedef struct s_lists
{
    int lists_count;
    int lists_max;
    S_LIST *list;
} S_LISTS;

typedef struct s_regal
{
    // int product_count;
    int regal_number;
    char *product;
} REGAL;

typedef struct s_nakup
{
    int regal_size;
    int regal_count;
    REGAL *regals;
} SHOP;

// z proseminare
void remove_LF(char *str)
{
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

void print_list(OPT *opt, SHOP **shop, int *seq)
{
    for (int i = 0; i < (*shop)->regal_count; i++)
    {
        for (int j = 0; j < opt->optimized_count; j++)
        {
            if (opt->opt_list[j].regal_num == i)
            {
                printf(" %d. %s -> #%d %s\n", *seq, opt->opt_list[j].list_product, opt->opt_list[j].regal_num, opt->opt_list[j].regal_product);
                (*seq)++;
            }
        }
    }
}

void optimize_lists(S_LISTS *lists, SHOP *shop)
{
    // pro kazdy seznam
    for (int a = 0; a <= lists->lists_count; a++)
    {
        printf("Optimalizovany seznam:\n");
        int seq = 0;
        int not_stored[STR_MAX];
        size_t ns_index = 0;
        OPT opt = {20, 0, (O_LIST *)malloc(opt.optimized_max * sizeof(*opt.opt_list))};

        // zjisti, jestli je produkt v obchode a kdyz neni, zapise ho do not_stored
        //  prodkuty v seznamu
        for (int j = 0; j < lists->list[a].product_count; j++)
        {
            int found = 0;
            // prodkuty v regalu
            for (int i = 0; i < shop->regal_count; i++)
            {
                int cmp = strncasecmp(shop->regals[i].product, lists->list[a].products[j], STR_MAX);
                if (cmp == 0)
                {
                    found = 1;
                    if (opt.optimized_count >= opt.optimized_max - 1)
                    {
                        opt.optimized_max *= 2;
                        opt.opt_list = (O_LIST *)realloc(opt.opt_list, opt.optimized_max * sizeof(*opt.opt_list));
                    }
                    opt.opt_list[opt.optimized_count].regal_num = shop->regals[i].regal_number;
                    opt.opt_list[opt.optimized_count].regal_product = strdup(shop->regals[i].product);
                    opt.opt_list[opt.optimized_count].list_product = strdup(lists->list[a].products[j]);
                    opt.optimized_count++;
                    break;
                }
            }

            if (found == 1)
            {
                found = 0;
                continue;
            }
            // prodkuty v regalu
            for (int i = 0; i < shop->regal_count; i++)
            {
                if (strcasestr(shop->regals[i].product, lists->list[a].products[j]) != NULL)
                {
                    if (opt.optimized_count >= opt.optimized_max - 1)
                    {
                        opt.optimized_max *= 2;
                        opt.opt_list = (O_LIST *)realloc(opt.opt_list, opt.optimized_max * sizeof(*opt.opt_list));
                    }
                    opt.opt_list[opt.optimized_count].regal_num = shop->regals[i].regal_number;
                    opt.opt_list[opt.optimized_count].regal_product = strdup(shop->regals[i].product);
                    opt.opt_list[opt.optimized_count].list_product = strdup(lists->list[a].products[j]);
                    opt.optimized_count++;
                    break;
                }
                else if (shop->regal_count - 1 == i)
                {
                    not_stored[ns_index] = j;
                    ns_index++;
                }
            }
        }
        print_list(&opt, &shop, &seq);

        free(opt.opt_list);

        for (size_t k = 0; k < ns_index; k++)
        {
            printf(" %d. %s -> N/A\n", seq, lists->list[a].products[not_stored[k]]);
            seq++;
        }
    }
}

void get_shopping_lists(S_LISTS *lists)
{
    int state;
    size_t product_len = 0;
    char *str = NULL;
    int cnt = 0, size = 0;

    while ((state = getline(&str, &product_len, stdin)) != EOF)
    {
        if (str[0] == '\n')
        {
            lists->lists_count++;
            continue;
        }
        if (lists->lists_count >= lists->lists_max - 1)
        {
            lists->lists_max *= 2;
            lists->list = (S_LIST *)realloc(lists->list, lists->lists_max * sizeof(*lists->list));
        }

        remove_LF(str);
        cnt = lists->list[lists->lists_count].product_count;
        lists->list[lists->lists_count].product_max = 20;
        size = lists->list[lists->lists_count].product_max;
        lists->list[lists->lists_count].products[cnt] = (char *)malloc(size * sizeof(*lists->list[lists->lists_count].products[cnt]));
        lists->list[lists->lists_count].products[cnt] = strdup(str);
        lists->list[lists->lists_count].product_count++;
    }

    free(str);
}

int get_regals(SHOP *shop)
{
    int state;
    int r_number = -1, prev = -1;
    char c;
    size_t product_len = 0;
    char *str = NULL;

    while ((state = getline(&str, &product_len, stdin)) != EOF)
    {
        if (str[0] == '\n')
            break;

        if ((state = sscanf(str, "%c%d", &c, &r_number)) == 2)
        {
            if (r_number > prev + 1 || c != '#')
                return 0;

            // printf("%d\n", r_number);
            prev = r_number;
        }
        else if ((str[0] == '#' && state != 2) || (r_number == -1 && c != '#'))
            return 0;

        else
        {
            if (shop->regal_count == shop->regal_size - 1)
            {
                shop->regal_size *= 2;
                shop->regals = (REGAL *)realloc(shop->regals, shop->regal_size * sizeof(REGAL));
            }
            remove_LF(str);
            shop->regals[shop->regal_count].regal_number = r_number;
            shop->regals[shop->regal_count].product = strdup(str);
            // printf("%s\n", shop->regals[shop->regal_count].product);
            shop->regal_count++;
        }
    }
    if (state == EOF && (r_number == -1 || str[0] != '\n'))
    {
        free(str);
        return 0;
    }
    free(str);
    return 1;
}

int main(void)
{
    SHOP shop;
    shop.regal_count = 0;
    shop.regal_size = 200;
    shop.regals = (REGAL *)malloc(shop.regal_size * sizeof(*shop.regals));

    if (get_regals(&shop) == 0)
    {
        printf("Nespravny vstup.\n");
        free(shop.regals);
        return 1;
    }
    S_LISTS lists;
    lists.lists_max = 20;
    lists.lists_count = 0;
    lists.list = (S_LIST *)malloc(lists.lists_max * sizeof(*lists.list));
    lists.list->product_max = 20;
    lists.list->product_count = 0;
    for (int i = 0; i < lists.lists_max; i++)
    {
        lists.list[i].products = (char **)malloc(lists.list->product_max * sizeof(*lists.list->products));
    }
    get_shopping_lists(&lists);

    optimize_lists(&lists, &shop);

    free(shop.regals);

    for (int i = 0; i < lists.lists_max; i++)
    {/*
        for (int j = 0; j < lists.list[i].product_count; j++)
        {
            free(lists.list[i].products[j]);
        }*/
        free(lists.list[i].products);
    }
    free(lists.list);

    return 0;
}