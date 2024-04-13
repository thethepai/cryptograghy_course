#include <algorithm>
#include <iterator>
#include <map>
#include <stdio.h>
#include <string.h>

#define KEY (16)
#define SPNPART (8)
#define INPUT (1 << 24)

typedef unsigned short UNIT16;
typedef unsigned int UNIT32;
typedef unsigned long long UNIT64;

using namespace std;
UNIT64 s_box[2][16] = {
    { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
    { 14, 3, 4, 8, 1, 12, 10, 15, 7, 13, 9, 6, 11, 2, 0, 5 }
};

UNIT64 p_arr[64];
int p_box[2][64] = {
    { 0, 16, 32, 48, 1, 17, 33, 49, 2, 18, 34, 50, 3, 19, 35, 51, 4, 20, 36, 52, 5, 21, 37, 53, 6, 22, 38, 54, 7, 23, 39, 55, 8, 24, 40, 56, 9, 25, 41, 57, 10, 26, 42, 58, 11, 27, 43, 59, 12, 28, 44, 60, 13, 29, 45, 61, 14, 30, 46, 62, 15, 31, 47, 63 },
    { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63 }
};

// main functions---------------------------------------------------
// do the permutation
UNIT64 p_transform(UNIT64 input, int mode);
// do the spn_enhance encryption
UNIT64 spn_encrypt(UNIT64 k[5], UNIT64 input, int mode);
// main functions---------------------------------------------------

UNIT64 p_transform(UNIT64 input, int mode)
{
    UNIT64 mask = 1;
    UNIT64 fin = 0;
    for (int i = 0; i < 64; i++) {
        p_arr[i] = ((input & mask) == 0 ? 0 : 1);
        mask <<= 1;
    }
    for (int i = 0; i < 64; i++) {
        fin ^= (p_arr[p_box[mode][i]] << i);
    }
    return fin;
}

UNIT64 spn_encrypt(UNIT64 k[5], UNIT64 input, int mode)
{
    for (int i = 0; i < 3; i++) {
        if (mode == 1) {
            if (i == 0) {
                input ^= k[4];
            } else {
                input ^= p_transform(k[4 - i], mode);
            }
        } else {
            input ^= k[i];
        }
        input = s_box[mode][input & 0xf] | ((s_box[mode][(input & 0xf0) >> 4]) << 4) | ((s_box[mode][(input & 0xf00) >> 8]) << 8) | ((s_box[mode][(input & 0xf000) >> 12]) << 12) | ((s_box[mode][(input & 0xf0000) >> 16]) << 16) | ((s_box[mode][(input & 0xf00000) >> 20]) << 20) | ((s_box[mode][(input & 0xf000000) >> 24]) << 24) | ((s_box[mode][(input & 0xf0000000) >> 28]) << 28) | ((s_box[mode][(input & 0xf00000000) >> 32]) << 32) | ((s_box[mode][(input & 0xf000000000) >> 36]) << 36) | ((s_box[mode][(input & 0xf0000000000) >> 40]) << 40) | ((s_box[mode][(input & 0xf00000000000) >> 44]) << 44) | ((s_box[mode][(input & 0xf000000000000) >> 48]) << 48) | ((s_box[mode][(input & 0xf0000000000000) >> 52]) << 52) | ((s_box[mode][(input & 0xf00000000000000) >> 56]) << 56) | ((s_box[mode][(input & 0xf000000000000000) >> 60]) << 60);
        input = p_transform(input, mode);
    }

    if (mode == 1) {
        input ^= p_transform(k[1], mode);
    } else {
        input ^= k[3];
    }

    input = s_box[mode][input & 0xf] | ((s_box[mode][(input & 0xf0) >> 4]) << 4) | ((s_box[mode][(input & 0xf00) >> 8]) << 8) | ((s_box[mode][(input & 0xf000) >> 12]) << 12) | ((s_box[mode][(input & 0xf0000) >> 16]) << 16) | ((s_box[mode][(input & 0xf00000) >> 20]) << 20) | ((s_box[mode][(input & 0xf000000) >> 24]) << 24) | ((s_box[mode][(input & 0xf0000000) >> 28]) << 28) | ((s_box[mode][(input & 0xf00000000) >> 32]) << 32) | ((s_box[mode][(input & 0xf000000000) >> 36]) << 36) | ((s_box[mode][(input & 0xf0000000000) >> 40]) << 40) | ((s_box[mode][(input & 0xf00000000000) >> 44]) << 44) | ((s_box[mode][(input & 0xf000000000000) >> 48]) << 48) | ((s_box[mode][(input & 0xf0000000000000) >> 52]) << 52) | ((s_box[mode][(input & 0xf00000000000000) >> 56]) << 56) | ((s_box[mode][(input & 0xf000000000000000) >> 60]) << 60);

    if (mode == 1) {
        input ^= k[0];
    } else {
        input ^= k[4];
    }
    return input;
}

int main()
{
    UNIT64 k[5] = { 0 };
    UNIT64 input[1] = { 0 };
    UNIT64 fin[1] = { 0 };
    UNIT64 val = 22220012;

    fread(k, SPNPART, 1, stdin);
    fread(k + 4, SPNPART, 1, stdin);

    k[1] = ((k[0] << 16) & 0xffffffffffff0000) ^ ((k[4] >> 48) & 0xffff);
    k[2] = ((k[0] << 32) & 0xffffffff00000000) ^ ((k[4] >> 32) & 0xffffffff);
    k[3] = ((k[0] << 48) & 0xffff000000000000) ^ ((k[4] >> 16) & 0xffffffffffffff);

    fread(input, SPNPART, 1, stdin);
    input[0] ^= val;
    fin[0] = spn_encrypt(k, input[0], 0);
    fwrite(fin, SPNPART, 1, stdout);

    // CBC mode
    for (int i = 1; i < (INPUT / SPNPART); i++) {
        fread(input, SPNPART, 1, stdin);
        input[0] ^= fin[0];
        fin[0] = spn_encrypt(k, input[0], 0);
        fwrite(fin, SPNPART, 1, stdout);
    }
    return 0;
}