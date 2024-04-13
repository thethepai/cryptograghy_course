#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define n_rounds 8
#define tu (1 << 21)

typedef unsigned short UNIT16;
typedef unsigned long long UNIT64;

// more rounds, longer keys, more complicated s_box and p_box, each part with 64 bits
UNIT64 keys[9] = {};
// random s_box
UNIT16 s[16][16] = {
    { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 },
    { 7, 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0 },
    { 0, 7, 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9 },
    { 9, 0, 7, 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5 },
    { 5, 9, 0, 7, 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12 },
    { 12, 5, 9, 0, 7, 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6 },
    { 6, 12, 5, 9, 0, 7, 14, 4, 13, 1, 2, 15, 11, 8, 3, 10 },
    { 10, 6, 12, 5, 9, 0, 7, 14, 4, 13, 1, 2, 15, 11, 8, 3 },
    { 3, 10, 6, 12, 5, 9, 0, 7, 14, 4, 13, 1, 2, 15, 11, 8 },
    { 8, 3, 10, 6, 12, 5, 9, 0, 7, 14, 4, 13, 1, 2, 15, 11 },
    { 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 14, 4, 13, 1, 2, 15 },
    { 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 14, 4, 13, 1, 2 },
    { 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 14, 4, 13, 1 },
    { 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 14, 4, 13 },
    { 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 14, 4 },
    { 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7, 14 },
};
// random p_box
UNIT16 p[32] = { 1, 24, 5, 25, 9, 19, 13, 20, 2, 26, 6, 32, 10, 21, 18, 14, 27, 3, 29, 7, 11, 23, 22, 28, 15, 4, 8, 31, 12, 16, 17, 30 };

// main functions---------------------------------------------------
// s transform
UNIT64 s_transform(UNIT64 input);
// p transform
UNIT64 p_transform(UNIT64 input);
// spn encrypt
UNIT64 encrypt(UNIT64 x);
// reading bits flow part by part
UNIT64 read_bits(void);
// generate round keys
void get_keys(UNIT64* key, UNIT64 dec1, UNIT64 dec2);

// main functions---------------------------------------------------

UNIT64 s_transform(UNIT64 input)
{
    UNIT64 fin = 0;
    UNIT64 temp[16];
    temp[0] = (input & 0xF000000000000000) >> 60;
    temp[1] = (input & 0xF00000000000000) >> 56;
    temp[2] = (input & 0xF0000000000000) >> 52;
    temp[3] = (input & 0xF000000000000) >> 48;
    temp[4] = (input & 0xF00000000000) >> 44;
    temp[5] = (input & 0xF0000000000) >> 40;
    temp[6] = (input & 0xF000000000) >> 36;
    temp[7] = (input & 0xF00000000) >> 32;
    temp[8] = (input & 0xF0000000) >> 28;
    temp[9] = (input & 0xF000000) >> 24;
    temp[10] = (input & 0xF00000) >> 20;
    temp[11] = (input & 0xF0000) >> 16;
    temp[12] = (input & 0xF000) >> 12;
    temp[13] = (input & 0xF00) >> 8;
    temp[14] = (input & 0xF0) >> 4;
    temp[15] = (input & 0xF) >> 0;

    temp[0] = s[temp[0]][temp[1]];
    temp[1] = s[temp[2]][temp[3]];
    temp[2] = s[temp[4]][temp[5]];
    temp[3] = s[temp[6]][temp[7]];
    temp[4] = s[temp[8]][temp[9]];
    temp[5] = s[temp[10]][temp[11]];
    temp[6] = s[temp[12]][temp[13]];
    temp[7] = s[temp[14]][temp[15]];

    temp[0] <<= 56;
    temp[1] <<= 48;
    temp[2] <<= 40;
    temp[3] <<= 32;
    temp[4] <<= 24;
    temp[5] <<= 16;
    temp[6] <<= 8;

    fin = temp[0] | temp[1] | temp[2] | temp[3] | temp[4] | temp[5] | temp[6] | temp[7];
    return fin;
}

UNIT64 p_transform(UNIT64 input)
{
    UNIT64 fin = 0;
    int data[64];
    int trans_data[64];
    int i;
    for (i = 63; i >= 0; i--) {
        data[i] = input & 0x1;
        input >>= 1;
    }
    for (i = 0; i < 32; i++)
        trans_data[i] = data[p[i] - 1];
    for (i = 32; i < 64; i++)
        trans_data[i] = data[p[i - 32] + 31];
    for (i = 0; i < 64; i++)
        fin = (fin << 1) + trans_data[i];
    return fin;
}

UNIT64 encrypt(UNIT64 x)
{
    UNIT64 y;
    UNIT64 w[n_rounds] = { 0 };
    UNIT64 u[n_rounds] = { 0 };
    UNIT64 v[n_rounds] = { 0 };

    w[0] = x;
    for (int i = 0; i < n_rounds; i++) {
        u[i] = w[i] ^ keys[i];
        v[i] = s_transform(u[i]);
        if (i < n_rounds - 1)
            w[i + 1] = p_transform(v[i]);
        else
            continue;
    }
    y = v[n_rounds - 1] ^ keys[n_rounds];
    return y;
}

UNIT64 read_bits(void)
{
    UNIT16 dec0, dec1, dec2;
    UNIT64 part = 0;

    for (int i = 0; i < 8; i++) {
        dec0 = getchar();
        part <<= 4;
        dec1 = dec0 >> 4;
        part |= dec1;
        part <<= 4;
        dec2 = dec0 & 0x0f;
        part |= dec2;
    }
    return part;
}

void get_keys(UNIT64* key, UNIT64 part1, UNIT64 part2)
{
    for (int i = 0; i < 9; i++) {
        key[i] = (part1 >> 8 * i) | (part2 << (64 - 8 * i));
    }
    return;
}

int main()
{
    UNIT64 part1 = read_bits();
    UNIT64 part2 = read_bits();
    get_keys(keys, part1, part2);

    UNIT64 x, y, random = 0;
    srand(time(NULL));
    for (int i = 0; i < 64; i++) {
        random <<= 1;
        random |= (rand() & 0x1);
    }
    x = read_bits();
    x ^= random;
    y = encrypt(x);
    fwrite(&y, sizeof(y), 1, stdout);

    for (int i = 1; i < tu; i++) {
        x = read_bits();
        x ^= y;
        y = encrypt(x);
        fwrite(&y, sizeof(y), 1, stdout);
    }
    return 0;
}