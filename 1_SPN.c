#include <stdio.h>
#include <stdlib.h>

typedef unsigned int UNIT32;
typedef unsigned short UNIT16;
typedef unsigned char UNIT8;

UNIT16 s[16] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };
UNIT16 s_reverse[16] = { 14, 3, 4, 8, 1, 12, 10, 15, 7, 13, 9, 6, 11, 2, 0, 5 };
UNIT16 p[16] = { 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15, 4, 8, 12, 16 };

// main functions---------------------------------------------------
// generate round keys
void get_keys(UNIT32 key, UNIT16* keys);
// s box transform
UNIT16 s_transform(UNIT16 input, const UNIT16* trans_data);
// p box transform
UNIT16 p_transform(UNIT16 input, const UNIT16* trans_data);
// generate spn and encrypt
UNIT16 encrypt(UNIT16 x, const UNIT16* keys);
// decrypt function
UNIT16 decrypt(UNIT16 x, const UNIT16* keys);
// main functions---------------------------------------------------

// other functions--------------------------------------------------
// get bit mask used in p transform
UNIT16 get_bit_mask(int i);
// read key
UNIT32 read_key(void);
// read plain text
UNIT16 read_x(void);
// other functions--------------------------------------------------

void get_keys(UNIT32 key, UNIT16* keys)
{
    for (int i = 0; i < 5; i++) {
        keys[i] = key >> (16 - 4 * i);
    }
    return;
}

UNIT16 s_transform(UNIT16 input, const UNIT16* trans_data)
{
    UNIT16 fin = 0x0000;
    UNIT32 a = input >> 12;
    fin |= trans_data[a] << 12;
    a = (input & 0x0f00) >> 8;
    fin |= trans_data[a] << 8;
    a = (input & 0x00f0) >> 4;
    fin |= trans_data[a] << 4;
    a = input & 0x000f;
    fin |= trans_data[a] << 0;
    return fin;
}

UNIT16 p_transform(UNIT16 input, const UNIT16* trans_data)
{
    UNIT16 fin = 0x0000;
    for (int i = 0; i < 16; i++) {
        UNIT32 pos = trans_data[i];
        UNIT32 mask = get_bit_mask(16 - (i));
        UNIT32 bit = input & mask;
        int j = pos - 1 - i;
        if (j > 0) {
            fin |= bit >> j;
        } else {
            fin |= bit << -j;
        }
    }
    return fin;
}

UNIT16 get_bit_mask(int i)
{
    UNIT16 mask = 0x0001;
    mask = mask << (i - 1);
    return mask;
}

UNIT16 encrypt(UNIT16 x, const UNIT16* keys)
{
    for (int i = 0; i < 4; i++) {
        x ^= keys[i];
        x = s_transform(x, s);
        if (i == 3) {
            break;
        }
        x = p_transform(x, p);
    }
    x ^= keys[4];
    return x;
}

UNIT16 decrypt(UNIT16 x, const UNIT16* keys)
{
    x ^= keys[4];
    x = s_transform(x, s_reverse);
    for (int i = 3; i > 0; i--) {
        x ^= keys[i];
        x = p_transform(x, p);
        x = s_transform(x, s_reverse);
    }
    x ^= keys[0];
    return x;
}

UNIT16 read_x(void)
{
    char c;
    UNIT16 x = 0x0;
    UNIT8 readc = 0x0;
    while ((c = getchar()) != '\n') {
        if ((c >= '0') && (c <= '9')) {
            readc = c - '0';
        }
        if ((c >= 'a') && (c <= 'f')) {
            readc = c - 87;
        }
        x = x << 4;
        x = x | readc;
    }
    return x;
}

UNIT32 read_key(void)
{
    char c;
    UNIT32 key = 0x0;
    UNIT8 readc = 0x0;
    while ((c = getchar()) != ' ') {
        if ((c >= '0') && (c <= '9')) {
            readc = c - '0';
        }
        if ((c >= 'a') && (c <= 'f')) {
            readc = c - 87;
        }
        key = key << 4;
        key = key | readc;
    }
    return key;
}

int main()
{
    UNIT16 x = 0x0;
    UNIT16 y = 0x0;
    UNIT32 key = 0x0;
    UNIT16 keys[5] = { 0, 0, 0, 0, 0 };

    int n = 0;
    scanf("%d", &n);
    getchar();
    for (int i = 0; i < n; i++) {
        key = read_key();
        x = read_x();
        get_keys(key, keys);
        y = encrypt(x, keys);
        UNIT16 m = 1;
        UNIT16 ny = y;
        UNIT16 temp = !(ny & m);
        ny = ny & (~m);
        ny = ny | temp;
        x = decrypt(ny, keys);
        printf("%04x %04x\n", y, x);
    }

    return 0;
}