#include <assert.h>
#include <bits/stdc++.h>

// norm
#define textInforlenth 4
#define Nr 4
#define maxn 10000
#define maxm (1 << 17)
#define maxdig (1 << (4 * textInforlenth))
#define outt(x) cout << #x << " = " << x << "   "
#define hh cout << endl
#define numoflinear1 15
#define numoflinear2 10
#define numoftest 2

using namespace std;
typedef long long UNIT64;

// S_box
const int SPN_Pi_S[maxn] = { 14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7 };
int Rev_SPN_Pi_S[maxn] = { 14, 3, 4, 8, 1, 12, 10, 15, 7, 13, 9, 6, 11, 2, 0, 5 };
int MAP_SPN_PI_S[maxm];
int Rev_MAP_SPN_PI_S[maxm];

// P_box
const int SPN_Pi_P[maxn] = { 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 };
int Rev_SPN_Pi_P[maxn] = { 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 };
int MAP_SPN_PI_P[maxm], Rev_MAP_SPN_PI_P[maxm];
int MAP_SPN_SP[maxm], Rev_MAP_SPN_SP[maxm];

// num ar
char NumtoChar[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

// others
bool Bool_Buffer[maxn];
int nowwa, Cnt;
char cc;
int n;
int qwer;

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
    int Infor[maxn], Key[maxn], nowval;

    // Constructor
    SPNTools();

    // Get information and keys from input
    void getInInfo();
    void getInKey();

    // Print information and keys
    void printInfo();
    void printKey();

    // Print the current state
    void printNorm();

    // Generate one value
    int generateOneVal();

    // Divide one value into bits
    void divideOne(int OneNum);

    // Initialize operations
    void initOp();

    // Byte substitution operation
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
void SPNTools::getInInfo()
{
    cc = getchar();
    for (register int i = 1; i <= textInforlenth; ++i) {
        if (isdigit(cc))
            Infor[i] = cc - '0';
        else
            Infor[i] = cc - 'a' + 10;
        cc = getchar();
    }
    nowval = generateOneVal();
    return;
}
void SPNTools::getInKey()
{
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
    for (int i = 1; i <= textInforlenth + Nr; ++i)
        putchar(NumtoChar[Key[i]]);
    return;
}
void SPNTools::printNorm()
{
    cout << "*******************************" << endl
         << "Infor: ";
    for (int i = 1; i <= textInforlenth; ++i)
        putchar(NumtoChar[Infor[i]]);
    cout << endl
         << "Key: ";
    for (int i = 1; i <= textInforlenth + Nr; ++i)
        putchar(NumtoChar[Key[i]]);
    cout << endl;
    return;
}
int SPNTools::generateOneVal()
{
    return (Infor[1] << 12) | (Infor[2] << 8) | (Infor[3] << 4) | (Infor[4]);
}
void SPNTools::divideOne(int OneNum)
{
    for (register int i = textInforlenth; i >= 1; i--)
        Infor[i] = OneNum & 0xf, OneNum >>= 4;
    return;
}
void SPNTools::initOp()
{
    for (register int val = 0; val < maxdig; ++val) {
        divideOne(val);
        for (register int i = 1; i <= textInforlenth; ++i)
            Infor[i] = SPN_Pi_S[Infor[i]];
        MAP_SPN_PI_S[val] = generateOneVal();
        Rev_MAP_SPN_PI_S[MAP_SPN_PI_S[val]] = val;

        divideOne(val);
        Cnt = 0;
        for (register int i = 1; i <= textInforlenth; ++i) {
            for (register int j = (1 << (textInforlenth - 1)); j; j >>= 1) {
                Bool_Buffer[SPN_Pi_P[Cnt]] = (Infor[i] & j) ? 1 : 0;
                Cnt++;
            }
        }
        for (register int i = 1; i <= textInforlenth; ++i)
            Cnt = ((i - 1) << 2), Infor[i] = (Bool_Buffer[Cnt] << 3) | (Bool_Buffer[Cnt + 1] << 2) | (Bool_Buffer[Cnt + 2] << 1) | (Bool_Buffer[Cnt + 3]);
        MAP_SPN_PI_P[val] = generateOneVal();
        Rev_MAP_SPN_PI_P[MAP_SPN_PI_P[val]] = val;
    }
    for (register int val = 0; val < maxdig; ++val) {
        MAP_SPN_SP[val] = MAP_SPN_PI_P[MAP_SPN_PI_S[val]];
        Rev_MAP_SPN_SP[MAP_SPN_SP[val]] = val;
    }
}
inline void SPNTools::substitute(int typ)
{
    if (typ == 0)
        nowval = MAP_SPN_PI_S[nowval];
    if (typ == 1)
        nowval = Rev_MAP_SPN_PI_S[nowval];
    return;
}
inline void SPNTools::xOrKey(int val)
{
    nowval ^= val;
    return;
}
inline void SPNTools::pTransform(int typ)
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
void SPNTools::encode() {
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

    // Substitution using S-box
    substitute(0);

    // XOR with the final subkey
    xOrKey(Lastval);

    // Divide the result into bits
    divideOne(nowval);

    return;
}

void SPNTools::decode() {
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

    // Continue with the inverse SPN decryption rounds
    for (int r = Nr - 1; r; --r) {
        // Inverse substitution using inverse S-box
        nowval = Rev_MAP_SPN_SP[nowval];

        // XOR with the current subkey
        xOrKey(Lastval);

        // Circular right shift by 4 bits
        Lastval >>= 4;
        Lastval |= (Key[r - 1] << ((textInforlenth - 1) * 4));
    }

    // Divide the result into bits
    divideOne(nowval);

    return;
}


bool count_1(int val)
{
    bool ans = 0;
    while (val) {
        if (val & 1)
            ans ^= 1;
        val >>= 1;
    }
    return ans;
}

SPNTools SPN;
int XorX[10], XorU4[10], ZeroNum[17][17][17][17], cnt, nowdiviation;
pair<int, int> PossibleKey[3][maxn];
pair<int, int> Ciphertextpair[maxm];
vector<int> Linear_attck_ans[5];
bool CipherU4[10000], CipherX[10000];
int ans = 0, maxnowwa, maxnum[10], minnum[10];
bool count_1Num[1 << 18];

inline void linear(int pls, int tim, int ansnum, int typ4, int typ3, int typ2, int typ1) {
    ans = 0;
    maxnowwa = 0;

    // Initialize PossibleKey array
    for (int i = 0; i <= ansnum; ++i) {
        PossibleKey[pls][i].first = PossibleKey[pls][i].second = 0;
    }
    PossibleKey[pls][0].first = 1;

    // Calculate maxnum and minnum values
    maxnum[4] = (XorU4[pls] & 0xf) ? 16 : 1;
    maxnum[3] = (XorU4[pls] & 0xf0) ? 16 : 1;
    maxnum[2] = (XorU4[pls] & 0xf00) ? 16 : 1;
    maxnum[1] = (XorU4[pls] & 0xf000) ? 16 : 1;
    minnum[4] = (typ4 == -1) ? 0 : typ4;
    minnum[3] = (typ3 == -1) ? 0 : typ3;
    minnum[2] = (typ2 == -1) ? 0 : typ2;
    minnum[1] = (typ1 == -1) ? 0 : typ1;
    maxnum[4] = (typ4 == -1) ? maxnum[4] : typ4 + 1;
    maxnum[3] = (typ3 == -1) ? maxnum[3] : typ3 + 1;
    maxnum[2] = (typ2 == -1) ? maxnum[2] : typ2 + 1;
    maxnum[1] = (typ1 == -1) ? maxnum[1] : typ1 + 1;

    memset(ZeroNum, 0, sizeof(ZeroNum));

    // Calculate CipherX values
    for (int i = 1; i <= tim; ++i) {
        CipherX[i] = Ciphertextpair[i].first & XorX[pls];
    }

    for (int l1 = minnum[1]; l1 < maxnum[1]; ++l1) {
        for (int l2 = minnum[2]; l2 < maxnum[2]; ++l2) {
            for (int l3 = minnum[3]; l3 < maxnum[3]; ++l3) {
                for (int l4 = minnum[4]; l4 < maxnum[4]; ++l4) {
                    nowwa = (l1 << 12) | (l2 << 8) | (l3 << 4) | l4;
                    ZeroNum[l1][l2][l3][l4] = 0;
                    Cnt = 0;

                    // Calculate CipherU4 values
                    for (int casee = 1; casee <= tim; ++casee) {
                        CipherU4[casee] = Ciphertextpair[casee].second ^ nowwa;
                        CipherU4[casee] = Rev_MAP_SPN_PI_S[CipherU4[casee]];
                        if (count_1Num[CipherX[casee] ^ (CipherU4[casee] & XorU4[pls])] == 0) {
                            ZeroNum[l1][l2][l3][l4]++;
                        }
                    }

                    nowdiviation = abs(ZeroNum[l1][l2][l3][l4] - (tim >> 1));

                    // Check if current deviation satisfies conditions
                    if (nowdiviation > (tim >> 6) && nowdiviation > PossibleKey[pls][PossibleKey[pls][0].first].first) {
                        PossibleKey[pls][PossibleKey[pls][0].first] = make_pair(nowdiviation, nowwa);

                        // Find the maximum deviation in PossibleKey[pls] array
                        for (int i = 1; i <= ansnum; ++i) {
                            if (PossibleKey[pls][PossibleKey[pls][0].first].first > PossibleKey[pls][i].first) {
                                PossibleKey[pls][0].first = i;
                            }
                        }
                    }
                }
            }
        }
    }
}

bool numbyte1[10000][20], numbyte2[10000][20], numbyte3[10000][20], numbyte4[10000][20];

void linear1(int pls, int tim, int ansnum) {
    ans = 0;
    maxnowwa = 0;
    PossibleKey[pls][0].first = 0;

    // Calculate CipherX values
    for (int i = 1; i <= tim; ++i) {
        CipherX[i] = count_1Num[Ciphertextpair[i].first & XorX[pls]];
    }

    // Calculate numbyte2 and numbyte4 values
    for (int i = 0; i < 16; ++i) {
        for (int casee = 1; casee <= tim; ++casee) {
            numbyte2[casee][i] = count_1Num[Rev_SPN_Pi_S[((Ciphertextpair[casee].second >> 8) ^ i) & 0xf] & (XorU4[pls] >> 8)];
            numbyte4[casee][i] = count_1Num[Rev_SPN_Pi_S[((Ciphertextpair[casee].second) ^ i) & 0xf] & XorU4[pls]];
        }
    }

    for (int l2 = 0; l2 < 16; ++l2) {
        for (int l4 = 0; l4 < 16; ++l4) {
            nowwa = (l2 << 8) | l4;
            ZeroNum[0][l2][0][l4] = 0;
            Cnt = 0;

            // Calculate ZeroNum values
            for (int casee = 1; casee <= tim; ++casee) {
                Cnt += !(numbyte2[casee][l2] ^ numbyte4[casee][l4] ^ CipherX[casee]);
            }

            ZeroNum[0][l2][0][l4] = Cnt;
            nowdiviation = abs(ZeroNum[0][l2][0][l4] - (tim >> 1));

            // Update PossibleKey if deviation conditions are met
            if (nowdiviation > PossibleKey[pls][0].first) {
                PossibleKey[pls][++PossibleKey[pls][0].first] = make_pair(nowdiviation, nowwa);
            }
        }
    }

    // Sort PossibleKey array
    sort(PossibleKey[pls] + 1, PossibleKey[pls] + 1 + PossibleKey[pls][0].first);
    return;
}

void linear2(int pls, int tim, int ansnum, int key2) {
    ans = 0;
    maxnowwa = 0;
    PossibleKey[pls][0].first = 0;

    // Calculate CipherX values
    for (int i = 1; i <= tim; ++i) {
        CipherX[i] = count_1Num[Ciphertextpair[i].first & XorX[pls]];
    }

    for (int casee = 1; casee <= tim; ++casee) {
        // Calculate numbyte2 values
        numbyte2[casee][key2] = count_1Num[Rev_SPN_Pi_S[((Ciphertextpair[casee].second >> 8) ^ key2) & 0xf] & (XorU4[pls] >> 8)] ^ CipherX[casee];

        // Calculate numbyte1 and numbyte3 values
        for (int i = 0; i < 16; ++i) {
            numbyte1[casee][i] = count_1Num[Rev_SPN_Pi_S[((Ciphertextpair[casee].second >> 12) ^ i) & 0xf] & (XorU4[pls] >> 12)];
            numbyte3[casee][i] = count_1Num[Rev_SPN_Pi_S[((Ciphertextpair[casee].second >> 4) ^ i) & 0xf] & (XorU4[pls] >> 4)];
        }
    }

    for (int l1 = 0; l1 < 16; ++l1) {
        for (int l3 = 0; l3 < 16; ++l3) {
            nowwa = (l1 << 12) | (key2 << 8) | (l3 << 4);
            ZeroNum[l1][key2][l3][0] = 0;
            Cnt = 0;

            // Calculate ZeroNum values
            for (int casee = 1; casee <= tim; ++casee) {
                Cnt += !(numbyte1[casee][l1] ^ numbyte2[casee][key2] ^ numbyte3[casee][l3]);
            }

            ZeroNum[l1][key2][l3][0] = Cnt;
            nowdiviation = abs(ZeroNum[l1][key2][l3][0] - (tim >> 1));

            // Update PossibleKey if deviation conditions are met
            {
                PossibleKey[pls][++PossibleKey[pls][0].first] = make_pair(nowdiviation, nowwa);
            }
        }
    }

    // Sort PossibleKey array
    sort(PossibleKey[pls] + 1, PossibleKey[pls] + 1 + PossibleKey[pls][0].first);
    return;
}

bool loopTest(int maxnum)
{
    for (int casee = 2654; casee < 2654 + maxnum; ++casee) {
        SPN.nowval = Ciphertextpair[casee].first;
        SPN.divideOne(Ciphertextpair[casee].first);
        SPN.encode();
        if (SPN.nowval != Ciphertextpair[casee].second)
            return false;
    }
    return true;
}

int main() {
    n = read();
    SPN.initOp();
    XorX[1] = 0x0b00;
    XorU4[1] = 0x0505;
    XorX[2] = 0x0c00;
    XorU4[2] = 0x5550;

    // Precompute count_1Num values
    for (int i = 0; i < (1 << 16); ++i) {
        count_1Num[i] = count_1(i);
    }

    while (n--) {
        // Initialize Ciphertextpair array
        for (int i = 1; i <= 8000; ++i) {
            SPN.getInInfo();
            nowwa = SPN.nowval;
            SPN.getInInfo();
            Ciphertextpair[i] = make_pair(nowwa, SPN.nowval);
        }

        linear1(1, 8000, numoflinear1);

        for (int i = PossibleKey[1][0].first; i >= 1; --i) {
            linear2(2, 8000, numoflinear2, (PossibleKey[1][i].second >> 8) & 0xf);
            SPN.Key[6] = (PossibleKey[1][i].second & 0xf00) >> 8;
            SPN.Key[8] = (PossibleKey[1][i].second & 0xf);

            for (int j = PossibleKey[2][0].first; j >= max(1, PossibleKey[2][0].first - numoftest); --j) {
                SPN.Key[5] = (PossibleKey[2][j].second & 0xf000) >> 12;
                SPN.Key[7] = (PossibleKey[2][j].second & 0xf0) >> 4;

                // Iterate over key4 values
                for (int key4 = 0; key4 < 65536; ++key4) {
                    qwer = key4;
                    SPN.Key[4] = qwer & 0xf;
                    qwer >>= 4;
                    SPN.Key[3] = qwer & 0xf;
                    qwer >>= 4;
                    SPN.Key[2] = qwer & 0xf;
                    qwer >>= 4;
                    SPN.Key[1] = qwer & 0xf;
                    qwer >>= 4;

                    if (loopTest(numoftest)) {
                        SPN.printKey();
                        goto endd;
                    }
                }
            }
        }

    endd:
        cout << endl;
        Linear_attck_ans[1].clear();
        Linear_attck_ans[2].clear();
    }

    return 0;
}
