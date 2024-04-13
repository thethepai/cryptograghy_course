#include <openssl/bn.h>
#include <stdio.h>

// main functions---------------------------------------------------
// calculate d in RSA encryption
BIGNUM* calculate_d(BIGNUM* e, BIGNUM* p, BIGNUM* q);
// validate if it's safe and sound
bool validate(BIGNUM* e, BIGNUM* p, BIGNUM* q);
// main functions---------------------------------------------------

BIGNUM* calculate_d(BIGNUM* e, BIGNUM* p, BIGNUM* q)
{
    BIGNUM* phi = BN_new();
    // p_num = p - 1
    BIGNUM* p_num = BN_new();
    // q_num = q - 1
    BIGNUM* q_num = BN_new();
    BIGNUM* d = BN_new();
    // BIGNUM* temp
    BN_CTX* ctx = BN_CTX_new();

    // calculate with phi(n) = (p - 1) * (q - 1) and d = e^(-1) mod phi(n)
    BN_sub(p_num, p, BN_value_one());
    BN_sub(q_num, q, BN_value_one());
    BN_mul(phi, p_num, q_num, ctx);
    BN_mod_inverse(d, e, phi, ctx);

    // free var
    BN_free(phi);
    BN_free(p_num);
    BN_free(q_num);
    BN_CTX_free(ctx);

    return d;
}

bool validate(BIGNUM* e, BIGNUM* p, BIGNUM* q)
{
    // gcd_num(p - 1, q - 1)
    BIGNUM* gcd_num = BN_new();
    BIGNUM* phi = BN_new();
    BIGNUM* p_num = BN_new();
    BIGNUM* q_num = BN_new();
    // BIGNUM* temp
    BN_CTX* ctx = BN_CTX_new();
    // sub temp
    BIGNUM* delta = BN_new();
    BIGNUM* result = BN_new();
    BIGNUM* divisor = BN_new();
    // other temp
    BIGNUM* t_num = BN_new();

    BN_sub(p_num, p, BN_value_one());
    BN_sub(q_num, q, BN_value_one());
    BN_mul(phi, p_num, q_num, ctx);
    BN_gcd(gcd_num, e, phi, BN_CTX_new());

    if (BN_cmp(gcd_num, BN_value_one()) != 0) {
        return true;
    }

    BN_sub(delta, p, q);
    BN_set_word(divisor, 10);
    BN_div(result, NULL, p, divisor, BN_CTX_new());

    if (BN_cmp(delta, result) == -1) {
        return true;
    }

    BN_gcd(gcd_num, p_num, q_num, BN_CTX_new());
    BN_set_word(t_num, 16);

    if (BN_cmp(gcd_num, t_num) == 1) {
        return true;
    }

    return false;
}

int main()
{
    int cases = 0;
    scanf("%d", &cases);

    for (int i = 0; i < cases; i++) {
        // inside the circulation, no free
        BIGNUM* e = BN_new();
        BIGNUM* p = BN_new();
        BIGNUM* q = BN_new();
        BIGNUM* d = BN_new();

        char e_string[1024];
        char p_string[2048];
        char q_string[2048];

        // read
        scanf("%s %s %s", e_string, p_string, q_string);
        BN_dec2bn(&e, e_string);
        BN_dec2bn(&p, p_string);
        BN_dec2bn(&q, q_string);

        // operate
        if (validate(e, p, q)) {
            printf("ERROR\n");
            continue;
        } else {
            d = calculate_d(e, p, q);
            char* d_string = BN_bn2dec(d);
            printf("%s\n", d_string);
        }
    }

    return 0;
}