qsort(ap_combinations, alfa.airplanes_size, sizeof(ap_combinations[0]), Cmp);

int Cmp(const void *a, const void *b)
{
    const double *aPtr = (const double *)a;
    const double *bPtr = (const double *)b;
    //return (*bPtr < *aPtr) - (*aPtr < *bPtr);
    return (*bPtr-*aPtr < fabs(*bPtr)*__DBL_EPSILON__)-(*aPtr-*bPtr < fabs(*aPtr)*__DBL_EPSILON__);
}
