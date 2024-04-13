#include <map>
#include <stdio.h>
#include <string.h>

#pragma GCC optimize("O3")

#define ModN 2176782336
#define SHA1_ROTL(a, b) (sha1_tmp = (a), ((sha1_tmp >> (32 - b)) & (0x7fffffff >> (31 - b))) | (sha1_tmp << b))
#define SHA1_F(B, C, D, t) ((t < 40) ? ((t < 20) ? ((B & C) | ((~B) & D)) : (B ^ C ^ D)) : ((t < 60) ? ((B & C) | (B & D) | (C & D)) : (B ^ C ^ D)))

using namespace std;

// sha_1 value temp variable
unsigned int sha1_tmp;
// begin string
char start_string[10000][9] = { 0 };
// end string
char end_string[10000][9] = { 0 };
// relation variable
int m;
// 2 maps storing string_norm and num relation
map<string, int> map1, map2;

// main functions---------------------------------------------------
// main functions---------------------------------------------------

inline void cal_string(unsigned sha_1[5], char string_norm[8], int i)
{
    // calculate n
    unsigned n = (sha_1[0] + sha_1[1] * i) % ModN;

    // initialize string_norm
    string_norm[0] = 'a';
    string_norm[1] = '0';
    string_norm[2] = '0';
    string_norm[3] = '0';
    string_norm[4] = '0';
    string_norm[5] = '0';
    string_norm[6] = '0';
    string_norm[7] = '0';

    // convert n to base-36 and store it in string_norm.
    int index = 2;
    while (n) {
        unsigned tmp = n % 36;
        if (tmp < 10) {
            string_norm[index++] = tmp + '0';
        } else {
            string_norm[index++] = tmp - 10 + 'a';
        }
        n /= 36;
    }
}

inline int UnitSHA1(const char* string_norm, int length, unsigned sha_1[5])
{
    /*
    计算字符串SHA-1
    参数说明：
    string_norm         字符串指针
    length      字符串长度
    sha1         用于保存SHA-1的字符串指针
    返回值为参数sha1
    */
    unsigned char *pp, *ppend;
    unsigned int l, i, K[80], W[80], TEMP, A, B, C, D, E, H0, H1, H2, H3, H4;
    H0 = 0x67452301, H1 = 0xEFCDAB89, H2 = 0x98BADCFE, H3 = 0x10325476, H4 = 0xC3D2E1F0;
    for (i = 0; i < 20; K[i++] = 0x5A827999)
        ;
    for (i = 20; i < 40; K[i++] = 0x6ED9EBA1)
        ;
    for (i = 40; i < 60; K[i++] = 0x8F1BBCDC)
        ;
    for (i = 60; i < 80; K[i++] = 0xCA62C1D6)
        ;
    l = length + ((length % 64 > 56) ? (128 - length % 64) : (64 - length % 64));
    if (!(pp = (unsigned char*)malloc((unsigned int)l)))
        return -1;
    for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = string_norm[i], i++)
        ;
    for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; pp[i + 3 - 2 * (i % 4)] = 0, i++)
        ;
    *((unsigned int*)(pp + l - 4)) = length << 3;
    *((unsigned int*)(pp + l - 8)) = length >> 29;
    for (ppend = pp + l; pp < ppend; pp += 64) {
        for (i = 0; i < 16; W[i] = ((unsigned int*)pp)[i], i++)
            ;
        for (i = 16; i < 80; W[i] = SHA1_ROTL((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]), 1), i++)
            ;
        A = H0, B = H1, C = H2, D = H3, E = H4;
        for (i = 0; i < 80; i++) {
            TEMP = SHA1_ROTL(A, 5) + SHA1_F(B, C, D, i) + E + W[i] + K[i];
            E = D, D = C, C = SHA1_ROTL(B, 30), B = A, A = TEMP;
        }
        H0 += A, H1 += B, H2 += C, H3 += D, H4 += E;
    }
    free(pp - l);
    sha_1[0] = H0, sha_1[1] = H1, sha_1[2] = H2, sha_1[3] = H3, sha_1[4] = H4;
    return 0;
}

inline bool find_string(char string_norm[9], unsigned sha_1[5], int p)
{
    int m;
    unsigned temp_sha1[5];
    char temp_string[9];
    strcpy(temp_string, start_string[p]);
    for (int i = 0; i < 10000; i++) {
        UnitSHA1(temp_string, 8, temp_sha1); // sha1
        for (m = 0; m < 5; m++) {
            if (temp_sha1[m] != sha_1[m]) {
                break;
            }
        }
        if (m == 5) {
            strcpy(string_norm, temp_string);
            return true;
        }
        cal_string(temp_sha1, temp_string, i % 100 + 1); // string_norm
    }

    return false;
}

int main()
{
    bool if_found_flag = false;
    unsigned sha_1[5], temp_sha1[5];
    char string_norm[9] = { 0 };
    scanf("%d", &m);
    for (int i = 0; i < m; i++) {
        scanf("%s%s", start_string[i], end_string[i]);
        string begin = string(start_string[i]);
        string end = string(end_string[i]);
        // speed up
        map1.insert(pair<string, int>(begin, i));
        map2.insert(pair<string, int>(end, i));
    }

    // read
    scanf("%8x%8x%8x%8x%8x", &(sha_1[0]), &(sha_1[1]), &(sha_1[2]), &(sha_1[3]), &(sha_1[4]));
    
    // cal
    for (int i = 0; i < 100; i++) {
        for (int m = 0; m < 5; m++) {
            temp_sha1[m] = sha_1[m];
        }
        for (int m = 0; m < 10000; m++) {
            cal_string(temp_sha1, string_norm, (i + m) % 100 + 1); // string_norm
            if (map2.find(string_norm) != map2.end()) {
                int p = map2[string_norm];
                if_found_flag = find_string(string_norm, sha_1, p);
                if (if_found_flag) {
                    break;
                }
            }
            UnitSHA1(string_norm, 8, temp_sha1); // sha1
        }
        if (if_found_flag) {
            break;
        }
    }

    // print
    if (if_found_flag == true) {
        printf("%s\n", string_norm);
    } else {
        printf("None\n");
    }
    return 0;
}