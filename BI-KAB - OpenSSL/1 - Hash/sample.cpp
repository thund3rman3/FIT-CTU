#ifndef __PROGTEST__
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <openssl/evp.h>
#include <openssl/rand.h>

#endif /* __PROGTEST__ */
using namespace std;

bool check_bit_cnt(int bits, unsigned char *str)
{
    int cnt = 0;
    //cout << "_______________________________________________________________" << endl;
    for (size_t i = 0; i < 64; i++)
    {
        //cout << "str i :   " << str[i] << " cnt: " << cnt << endl;
        int max = 255;
        int minus = 128;
        char c = str[i];
        for (size_t j = max - minus; j > 0; minus /= 2, j -= minus) // j0=127, j1=255-64=191,j2=255-32=223
        {                                                           // minus=1 j=254, minus=0 j ==255
            //cout << "   " << (c | j) << "  j:" << j << endl;
            if ((c | j) != j)
            {
                i = 65;
                break;
            }
            else
            {
                cnt++;
                continue;
            }
        }
        if ((c | 0) == 0)
            cnt++;
        else break;
        //cout << "........................." << i << endl;
    }
    // cout << endl;
    if(bits == 0 && cnt != 0)
        return false;
    if (cnt >= bits)
        return true;
    return false;
}

int findHash(int bits, char **message, char **hash)
{
    if (bits < 0 || bits > 512)
        return 0;

    EVP_MD_CTX *ctx;     // struktura kontextu
    const EVP_MD *type;  // typ pouzite hashovaci funkce
    unsigned int length; // vysledna delka hashe
    char hashFunction[] = "SHA512";

    const int MAX = 10;
    unsigned char message_tmp[MAX];
    unsigned char hash_tmp[EVP_MAX_MD_SIZE]; // char pole pro hash - 64 bytu (max pro sha 512)

    /* Inicializace OpenSSL hash funkci */
    OpenSSL_add_all_digests();

    /* Zjisteni, jaka hashovaci funkce ma byt pouzita */
    type = EVP_get_digestbyname(hashFunction);

    *message = (char *)malloc(MAX * 2 + 1);
    *hash = (char *)malloc(EVP_MAX_MD_SIZE * 2 + 1);

    /* Pokud predchozi prirazeni vratilo -1, tak nebyla zadana spravne hashovaci funkce */
    if (!type)
    {
        printf("Hash %s neexistuje.\n", hashFunction);
        return 0;
    }

    ctx = EVP_MD_CTX_new(); // create context for hashing
    if (ctx == NULL)
        return 0;

    while (true)
    {
        if (RAND_bytes(message_tmp, sizeof(message_tmp)) == false)
            return 0;

        /* Hash the text */
        if (!EVP_DigestInit_ex(ctx, type, NULL)) // context setup for our hash type
            return 0;

        if (!EVP_DigestUpdate(ctx, message_tmp, sizeof(message_tmp))) // feed the message in
            return 0;

        if (!EVP_DigestFinal_ex(ctx, hash_tmp, &length)) // get the hash
            return 0;

        // cout << "hash : " << hash_tmp << length << endl;

        if (check_bit_cnt(bits, hash_tmp) == true)
        {
            int i = 0, j = 0;
            for (; j < MAX && i < MAX * 2; i += 2, j++)
                sprintf(*message + i, "%02X", message_tmp[j]);
            (*message)[MAX * 2] = '\0';

            int k = 0, l = 0;
            for (; l < EVP_MAX_MD_SIZE && k < EVP_MAX_MD_SIZE * 2; k += 2, l++)
                sprintf(*hash + k, "%02X", hash_tmp[l]);
            (*hash)[EVP_MAX_MD_SIZE * 2] = '\0';

            //cout << "MESS:" << *message << " " << strlen(*message) << endl;
            //cout << "hash:" << *hash << " " << strlen(*hash) << endl;
            break;
        }
    }
    EVP_MD_CTX_free(ctx); // destroy the context
    return 1;
}

int findHashEx(int bits, char **message, char **hash, const char *hashFunction)
{
    /* TODO or use dummy implementation */
    return 1;
}

#ifndef __PROGTEST__

bool checkHash(int bits, char *hexString)
{
    int idx = 0;
    int zero_cnt = 0;
    char c;
    while (true)
    {
        c = hexString[idx++];
        if (c == '0')
        {
            zero_cnt += 4;
            continue;
        }
        else if (c < 8 + 48 && c > 3 + 48) // 4-7
        {
            zero_cnt += 1;
            break;
        }
        else if (c > 7 + 48 || c < 48) //<0 ||>7
            break;
        else if (c == '2' || c == '3')
        {
            zero_cnt += 2;
            break;
        }
        else if (c == '1')
        {
            zero_cnt += 3;
            break;
        }
        else
            break;
    }
    if (bits <= zero_cnt)
    {
        return true;
    }
    return false;
}

int main(void)
{
    char *message, *hash;
    assert(findHash(0, &message, &hash) == 1);
    // std::cout << "vysledekM0:  " << message << std::endl;
    // std::cout << "vysledekH0:  " << hash << std::endl;
    assert(message && hash && checkHash(0, hash));
    free(message);
    free(hash);

    assert(findHash(1, &message, &hash) == 1);
    // std::cout << "vysledekM1:  " << message << std::endl;
    // std::cout << "vysledekH1:  " << hash << std::endl;
    assert(message && hash && checkHash(1, hash));
    free(message);
    free(hash);

    assert(findHash(2, &message, &hash) == 1);
    assert(message && hash && checkHash(2, hash));
    // std::cout << "vysledekM2:  " << message << std::endl;
    // std::cout << "vysledekH2:  " << hash << std::endl;
    free(message);
    free(hash);

    assert(findHash(3, &message, &hash) == 1);
    // std::cout << "vysledekM3:  " << message << std::endl;
    // std::cout << "vysledekH3:  " << hash << std::endl;
    assert(message && hash && checkHash(3, hash));
    free(message);
    free(hash);

    assert(findHash(20, &message, &hash) == 1);
    // std::cout << "vysledekM3:  " << message << std::endl;
    // std::cout << "vysledekH3:  " << hash << std::endl;
    assert(message && hash && checkHash(20, hash));
    free(message);
    free(hash);

    assert(findHash(-1, &message, &hash) == 0);

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */