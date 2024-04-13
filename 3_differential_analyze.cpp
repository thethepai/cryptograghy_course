#include <algorithm>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <utility>

// norm
#define textInforlenth 4
#define Nr 4
#define maxn 100000
#define maxm ((1 << 16) + 3)
#define maxdig (1 << (4 * textInforlenth))
#define maxbyte 18
#define outt(x) printf("%s = %d    ", #x, x)
#define hh puts("")
#define numoflinear1 15
#define numoflinear2 10
#define numoftest 3

using namespace std;
typedef unsigned short UNIT16;

const int SeqofTest[] = { 1654, 9484, 15156, 48511, 34516, 451, 2, 16715, 65532, 13245, 74 };

// S_box
const int SPN_Pi_S[maxbyte] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };
const int Rev_SPN_Pi_S[maxbyte] = { 14, 3, 4, 8, 1, 12, 10, 15, 7, 13, 9, 6, 11, 2, 0, 5 };
int MAP_SPN_PI_S[maxm], Rev_MAP_SPN_PI_S[maxm];

// P_box
const int SPN_Pi_P[maxbyte] = { 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 };
const int Rev_SPN_Pi_P[maxbyte] = { 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 };
int MAP_SPN_PI_P[maxm], Rev_MAP_SPN_PI_P[maxm];
int MAP_SPN_SP[maxm], Rev_MAP_SPN_SP[maxm];

// num ar
char NumtoChar[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
int ChartoNum[500];

// others
bool Bool_Buffer[maxm];
int nowwa, Cnt;
char cc;
int n;

// read input
inline int read()
{
    int s = 0, f = 1;
    char c = getchar();
    while (c < '0' || c > '9')
        f *= c == '-' ? -1 : 1, c = getchar();
    while (c >= '0' && c <= '9')
        s = s * 10 + c - '0', c = getchar();
    return s * f;
}

class SPNTools {
private:
public:
    // Arrays to store input information, keys, and the current value
    int Infor[maxbyte], Key[maxbyte], nowval;

    // Constructor
    SPNTools();

    // Get information and keys from input
    inline void getInInfo();
    void getInKey();

    // Print information and keys
    void printInfo();
    void printKey();

    // Print the current state
    void printNorm();

    // Generate one value
    int generateOneVal();

    // Divide one value into bits
    void divideOneVal(int OneNum);

    // Initialize operations
    void initOp();

    // Initialize operations
    inline void substitute(int typ);

    // Key XOR operation
    inline void xOrKey(int val);

    // pTransform operation
    inline void pTransform(int typ);

    // Encryption operation
    void encode();

    // Decryption operation
    void decode();
};

SPNTools::SPNTools()
{
    nowval = 0;
    for (int i = 1; i <= textInforlenth + Nr; ++i)
        Infor[i] = Key[i] = 0;
}
inline void SPNTools::getInInfo()
{
    cc = getchar();
    Infor[1] = ChartoNum[cc];
    cc = getchar();
    Infor[2] = ChartoNum[cc];
    cc = getchar();
    Infor[3] = ChartoNum[cc];
    cc = getchar();
    Infor[4] = ChartoNum[cc];
    cc = getchar();
    nowval = generateOneVal();
    return;
}
void SPNTools::getInKey()
{
    cc = getchar();
    while ((!isdigit(cc)) && (cc > 'f' || cc < 'a'))
        cc = getchar();
    for (register int i = 1; i <= textInforlenth + Nr; ++i) {
        if (isdigit(cc))
            Key[i] = cc - '0';
        else
            Key[i] = cc - 'a' + 10;
        cc = getchar();
    }
    return;
}
void SPNTools::printInfo()
{
    for (int i = 1; i <= textInforlenth; ++i)
        putchar(NumtoChar[Infor[i]]);
    return;
}
void SPNTools::printKey()
{
    putchar(NumtoChar[Key[1]]);
    putchar(NumtoChar[Key[2]]);
    putchar(NumtoChar[Key[3]]);
    putchar(NumtoChar[Key[4]]);
    putchar(NumtoChar[Key[5]]);
    putchar(NumtoChar[Key[6]]);
    putchar(NumtoChar[Key[7]]);
    putchar(NumtoChar[Key[8]]);
    return;
}
void SPNTools::printNorm()
{
    printf("*******************************\nInfor: ");
    for (int i = 1; i <= textInforlenth; ++i)
        putchar(NumtoChar[Infor[i]]);
    printf("\nKey: ");
    for (int i = 1; i <= textInforlenth + Nr; ++i)
        putchar(NumtoChar[Key[i]]);
    puts("");
    return;
}
int SPNTools::generateOneVal()
{
    return (Infor[1] << 12) | (Infor[2] << 8) | (Infor[3] << 4) | (Infor[4]);
}
void SPNTools::divideOneVal(int OneNum)
{
    for (register int i = textInforlenth; i >= 1; i--)
        Infor[i] = OneNum & 0xf, OneNum >>= 4;
    return;
}
void SPNTools::initOp()
{
    for (int i = 0; i < (1 << 4); ++i)
        ChartoNum[(int)NumtoChar[i]] = i;

    for (register int val = 0; val < maxdig; ++val) {
        divideOneVal(val);
        for (register int i = 1; i <= textInforlenth; ++i)
            Infor[i] = SPN_Pi_S[Infor[i]];
        MAP_SPN_PI_S[val] = generateOneVal();
        Rev_MAP_SPN_PI_S[MAP_SPN_PI_S[val]] = val;

        divideOneVal(val);
        Cnt = 0;
        for (register int i = 1; i <= textInforlenth; ++i) {
            for (register int j = (1 << (textInforlenth - 1)); j; j >>= 1) {
                Bool_Buffer[SPN_Pi_P[Cnt]] = (Infor[i] & j) ? 1 : 0;
                Cnt++;
            }
        }
        for (register int i = 1; i <= textInforlenth; ++i)
            Cnt = ((i - 1) << 2), Infor[i] = (Bool_Buffer[Cnt] << 3) | (Bool_Buffer[Cnt + 1] << 2) | (Bool_Buffer[Cnt + 2] << 1) | (Bool_Buffer[Cnt + 3]); // 默认为4*4bit
        MAP_SPN_PI_P[val] = generateOneVal();
        Rev_MAP_SPN_PI_P[MAP_SPN_PI_P[val]] = val;
    }
    for (register int val = 0; val < maxdig; ++val) {
        MAP_SPN_SP[val] = MAP_SPN_PI_P[MAP_SPN_PI_S[val]];
        Rev_MAP_SPN_SP[MAP_SPN_SP[val]] = val;
    }
}
void SPNTools::substitute(int typ)
{
    if (typ == 0)
        nowval = MAP_SPN_PI_S[nowval];
    if (typ == 1)
        nowval = Rev_MAP_SPN_PI_S[nowval];
    return;
}
void SPNTools::xOrKey(int val)
{
    nowval ^= val;
    return;
}
void SPNTools::pTransform(int typ)
{
    if (typ == 0) {
        nowval = MAP_SPN_PI_P[nowval];
        return;
    } else if (typ == 1) {
        nowval = Rev_MAP_SPN_PI_P[nowval];
        return;
    }
    return;
}
void SPNTools::encode()
{
    // Initialize the last value for encoding
    int Lastval = (Key[1] << 12) | (Key[2] << 8) | (Key[3] << 4) | (Key[4]);

    // Perform the SPN encryption rounds
    for (int r = 1; r <= Nr - 1; ++r) {
        // XOR with the current subkey
        xOrKey(Lastval);

        // Circular left shift by 4 bits
        Lastval <<= 4;
        Lastval &= 0xffff;
        // XOR with the next subkey
        Lastval |= Key[r + textInforlenth];
        // Substitution using S-box
        nowval = MAP_SPN_SP[nowval];
    }

    // Final round
    xOrKey(Lastval);
    Lastval <<= 4;
    Lastval &= 0xffff;
    Lastval |= Key[Nr + textInforlenth];

    // Final round
    substitute(0);

    // XOR with the final subkey
    xOrKey(Lastval);

    // Divide the result into bits
    divideOneVal(nowval);

    return;
}
void SPNTools::decode()
{
    // Initialize the last value for decoding
    int Lastval = (Key[Nr + 1] << 12) | (Key[Nr + 2] << 8) | (Key[Nr + 3] << 4) | (Key[Nr + 4]);

    // XOR with the final subkey
    xOrKey(Lastval);

    // Circular right shift by 4 bits
    Lastval >>= 4;
    Lastval |= (Key[Nr] << ((textInforlenth - 1) * 4));

    // Substitution using inverse S-box
    substitute(1);

    // XOR with the next subkey
    xOrKey(Lastval);
    Lastval >>= 4;
    Lastval |= (Key[Nr - 1] << ((textInforlenth - 1) * 4));

    // Continue with the inverse SPN decryption rounds
    for (int r = Nr - 1; r; --r) {
        // Inverse substitution using inverse S-box
        nowval = Rev_MAP_SPN_SP[nowval];

        // XOR with the current subkey
        xOrKey(Lastval);

        // Circular right shift by 4 bits
        Lastval >>= 4;
        Lastval |= (Key[r - 1] << ((textInforlenth - 1) * 4)); // moren
    }

    // Divide the result into bits
    divideOneVal(nowval);
    return;
}

SPNTools SPN;
int XorX[maxbyte], XorU4[maxbyte], ZeroNum[maxbyte][maxbyte][maxbyte][maxbyte], cnt, nowdiviation, qwer;
pair<int, int> PossibleKey1[maxm], PossibleKey2[maxm];
UNIT16 Ciphertextpair[maxm];
UNIT16 numbyte1, numbyte2;

void DifferentialAttack1(int pls, int tim)
{
    // Reset the number of possible keys found
    PossibleKey1[0].first = 0;
    UNIT16 Othe;

    // Iterate over the given number of tries
    for (register UNIT16 casee = 0; casee < tim; ++casee) {
        Othe = casee ^ XorX[pls];

        // Check if the high nibble difference is not equal to 0
        if (((Ciphertextpair[casee] ^ Ciphertextpair[Othe]) & 0xf0f0))
            continue;

        // Iterate over possible values for l2 and l4
        for (register UNIT16 l2 = 0; l2 < 16; ++l2) {
            for (register UNIT16 l4 = 0; l4 < 16; ++l4) {
                ZeroNum[0][l2][0][l4] += ((((Rev_SPN_Pi_S[((Ciphertextpair[casee] >> 8) ^ l2) & 0xf] ^ Rev_SPN_Pi_S[((Ciphertextpair[Othe] >> 8) ^ l2) & 0xf]) << 8) | (Rev_SPN_Pi_S[((Ciphertextpair[casee]) ^ l4) & 0xf] ^ Rev_SPN_Pi_S[((Ciphertextpair[Othe]) ^ l4) & 0xf])) ^ XorU4[pls]) ? 0 : 1;
            }
        }
    }

    // Iterate over all possible l2 and l4 values
    for (register UNIT16 l2 = 0; l2 < 16; ++l2) {
        for (register UNIT16 l4 = 0; l4 < 16; ++l4) {
            nowwa = (l2 << 8) | l4;
            if (ZeroNum[0][l2][0][l4])
                PossibleKey1[++PossibleKey1[0].first] = make_pair(ZeroNum[0][l2][0][l4], nowwa);
            ZeroNum[0][l2][0][l4] = 0;
        }
    }
    // Sort the possible keys based on their counts
    sort(PossibleKey1 + 1, PossibleKey1 + 1 + PossibleKey1[0].first);
    return;
}

void DifferentialAttack2(int pls, int tim)
{
    // Reset the number of possible keys found
    PossibleKey2[0].first = 0;
    UNIT16 Othe;

    // Iterate over the given number of tries
    for (register int casee = 1000; casee < 1000 + tim; ++casee) {
        Othe = casee ^ XorX[pls];

        // Check if the low nibble difference is not equal to 0
        if ((Ciphertextpair[casee] ^ Ciphertextpair[Othe]) & 0x0f0f)
            continue;

        // Iterate over possible values for l1 and l3
        for (register int l1 = 0; l1 < 16; ++l1) {
            for (register int l3 = 0; l3 < 16; ++l3) {
                ZeroNum[l1][0][l3][0] += ((((Rev_SPN_Pi_S[((Ciphertextpair[casee] >> 12) ^ l1) & 0xf] ^ Rev_SPN_Pi_S[((Ciphertextpair[Othe] >> 12) ^ l1) & 0xf]) << 12) | ((Rev_SPN_Pi_S[((Ciphertextpair[casee] >> 4) ^ l3) & 0xf] ^ Rev_SPN_Pi_S[((Ciphertextpair[Othe] >> 4) ^ l3) & 0xf]) << 4)) ^ XorU4[pls]) ? 0 : 1;
            }
        }
    }

    // Iterate over all possible l1 and l3 values
    for (register int l1 = 0; l1 < 16; ++l1) {
        for (register int l3 = 0; l3 < 16; ++l3) {
            nowwa = (l1 << 12) | (l3 << 4);
            if (ZeroNum[l1][0][l3][0])
                PossibleKey2[++PossibleKey2[0].first] = make_pair(ZeroNum[l1][0][l3][0], nowwa);
            ZeroNum[l1][0][l3][0] = 0;
        }
    }

    // Sort the possible keys based on their counts
    sort(PossibleKey2 + 1, PossibleKey2 + 1 + PossibleKey2[0].first);
    return;
}

bool LoopTest(int maxnum)
{
    for (int casee = 0; casee < maxnum; ++casee) {

        SPN.nowval = SeqofTest[casee];
        SPN.divideOneVal(SeqofTest[casee]);
        SPN.encode();
        if (SPN.nowval != Ciphertextpair[SeqofTest[casee]])
            return false;
    }
    return true;
}
time_t Initial, casebegin, afterinput, afterattack, FinalTime, afterenumerate;

UNIT16 readInfor()
{
    char c = getchar();
    UNIT16 re = 0;
    re = (re << 4) | ChartoNum[c];
    c = getchar();
    re = (re << 4) | ChartoNum[c];
    c = getchar();
    re = (re << 4) | ChartoNum[c];
    c = getchar();
    re = (re << 4) | ChartoNum[c];
    c = getchar();
    return re;
}

int main()
{
    n = read();
    SPN.initOp();
    XorX[1] = 0x0b00;
    XorU4[1] = 0x0606;
    XorX[2] = 0x0020;
    XorU4[2] = 0x5050;
    while (n--) {
        for (register int i = 0; i < 65536; ++i) {
            SPN.getInInfo();
            Ciphertextpair[i] = SPN.nowval;
        }
        DifferentialAttack1(1, 1500);
        DifferentialAttack2(2, 8000);
        for (int i = PossibleKey1[0].first; i >= 1; --i) {
            SPN.Key[6] = (PossibleKey1[i].second & 0xf00) >> 8;
            SPN.Key[8] = (PossibleKey1[i].second & 0xf);
            for (int j = PossibleKey2[0].first; j >= 1; --j) {
                SPN.Key[5] = (PossibleKey2[j].second & 0xf000) >> 12;
                SPN.Key[7] = (PossibleKey2[j].second & 0xf0) >> 4;
                for (register int key4 = 0; key4 < 65536; ++key4) {
                    qwer = key4;
                    SPN.Key[4] = qwer & 0xf;
                    qwer >>= 4;
                    SPN.Key[3] = qwer & 0xf;
                    qwer >>= 4;
                    SPN.Key[2] = qwer & 0xf;
                    qwer >>= 4;
                    SPN.Key[1] = qwer & 0xf;
                    qwer >>= 4;
                    if (LoopTest(numoftest)) {
                        SPN.printKey();
                        goto endd;
                    }
                }
            }
        }
    endd:
        hh;
    }
    return 0;
}