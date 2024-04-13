#include <openssl/bn.h>
#include <stdio.h>

// main functions---------------------------------------------------
// calculate d
void calculate_d(BIGNUM* d, const BIGNUM* e, const BIGNUM* p, const BIGNUM* q);
// calculate mod with montgomery
void modular_cal(BIGNUM* fin, const BIGNUM* m, const BIGNUM* e, const BIGNUM* num);
// china remainder theorem calculate const_c^d(mod p*q)
BIGNUM* crt_cal(BIGNUM* const_c, BIGNUM* d, BIGNUM* p, BIGNUM* q);
// main functions---------------------------------------------------

void calculate_d(BIGNUM* d, const BIGNUM* e, const BIGNUM* p, const BIGNUM* q)
{
    BIGNUM* phi = BN_new();
    // p_num = p - 1
    BIGNUM* p_num = BN_new();
    // q_num = q - 1
    BIGNUM* q_num = BN_new();
    // BIGNUM* temp
    BN_CTX* ctx = BN_CTX_new();

    // calculate with phi(num) = (p - 1) * (q - 1)
    BN_sub(p_num, p, BN_value_one());
    BN_sub(q_num, q, BN_value_one());
    BN_mul(phi, p_num, q_num, ctx);
    // d = e^(-1) mod phi(num)
    BN_mod_inverse(d, e, phi, ctx);

    // free
    BN_free(p_num);
    BN_free(q_num);
    BN_free(phi);
    BN_CTX_free(ctx);
}

void modular_cal(BIGNUM* fin, const BIGNUM* m, const BIGNUM* e, const BIGNUM* num)
{
    // BIGNUM* temp
    BN_CTX* ctx = BN_CTX_new();
    // montgomery
    BN_MONT_CTX* mont = BN_MONT_CTX_new();
    BN_MONT_CTX_set(mont, num, ctx);

    // calculate with (m^e) mod num
    BN_mod_exp_mont(fin, m, e, num, ctx, mont);

    // free
    BN_CTX_free(ctx);
    BN_MONT_CTX_free(mont);
}

BIGNUM* crt_cal(BIGNUM* const_c, BIGNUM* d, BIGNUM* p, BIGNUM* q)
{
    BIGNUM* fin1 = BN_new();
    BIGNUM* fin2 = BN_new();
    // BIGNUM* temp
    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* d_mod_p = BN_new();
    BIGNUM* d_mod_q = BN_new();
    BIGNUM* c_mod_p = BN_new();
    BIGNUM* c_mod_q = BN_new();
    // const_c^d 's mod
    BIGNUM* mod_p = BN_new();
    BIGNUM* mod_q = BN_new();
    BIGNUM* p_i = BN_new();
    BIGNUM* q_i = BN_new();
    // q * q_i
    BIGNUM* p_and_qi = BN_new();
    // p * p_i
    BIGNUM* q_and_pi = BN_new();
    BIGNUM* num = BN_new();
    BIGNUM* p_num = BN_new();
    BIGNUM* q_num = BN_new();

    // calculate
    BN_mul(num, p, q, ctx);
    BN_copy(p_num, p);
    BN_copy(q_num, q);
    BN_sub_word(p_num, 1);
    BN_sub_word(q_num, 1);

    // c_mod_p = const_c mod p
    BN_mod(c_mod_p, const_c, p, ctx);
    // c_mod_q = const_c mod q
    BN_mod(c_mod_q, const_c, q, ctx);
    // d_mod_p = d mod (p - 1)
    BN_mod(d_mod_p, d, p_num, ctx);
    // d_mod_q = d mod (q - 1)
    BN_mod(d_mod_q, d, q_num, ctx);
    // mod_p = c_mod_p^d_mod_p mod p
    modular_cal(mod_p, c_mod_p, d_mod_p, p);
    // mod_q = c_mod_q^d_mod_q mod q
    modular_cal(mod_q, c_mod_q, d_mod_q, q);
    // inverse
    BN_mod_inverse(p_i, p, q, ctx);
    BN_mod_inverse(q_i, q, p, ctx);
    BN_mul(p_and_qi, q, q_i, ctx);
    BN_mul(q_and_pi, p, p_i, ctx);

    // china remainder theorem
    BN_mod_mul(fin1, mod_p, p_and_qi, num, ctx);
    BN_mod_mul(fin2, mod_q, q_and_pi, num, ctx);
    BN_mod_add(fin1, fin1, fin2, num, ctx);

    // free
    BN_free(d_mod_p);
    BN_free(d_mod_q);
    BN_free(c_mod_p);
    BN_free(c_mod_q);
    BN_free(mod_p);
    BN_free(mod_q);
    BN_free(num);
    BN_free(p_num);
    BN_free(q_num);
    BN_free(p_i);
    BN_free(q_i);
    BN_free(p_and_qi);
    BN_free(q_and_pi);
    BN_CTX_free(ctx);
    BN_free(fin2);

    return fin1;
}

int main()
{
    BIGNUM* e = BN_new();
    BIGNUM* d = BN_new();
    BIGNUM* p = BN_new();
    BIGNUM* q = BN_new();
    BIGNUM* num = BN_new();
    BIGNUM* const_c = BN_new();
    // BIGNUM* temp
    BN_CTX* ctx = BN_CTX_new();

    // read
    int cases;
    scanf("%d", &cases);
    char p_string[2048];
    char q_string[2048];
    char e_string[2048];
    char c_string[2048];
    scanf("%s %s %s", p_string, q_string, e_string);
    BN_dec2bn(&e, e_string);
    BN_dec2bn(&p, p_string);
    BN_dec2bn(&q, q_string);

    calculate_d(d, e, p, q);
    for (int i = 0; i < cases; i++) {
        scanf("%s", c_string);
        BN_dec2bn(&const_c, c_string);
        // calculate with const_c^d(mod p*q)
        BIGNUM* fin = crt_cal(const_c, d, p, q);

        // print
        char* fin_string = BN_bn2dec(fin);
        printf("%s\n", fin_string);

        // free
        BN_free(fin);
        OPENSSL_free(fin_string); 
    }

    // free
    BN_free(d);
    BN_free(e);
    BN_free(q);
    BN_free(p);
    BN_free(num);
    BN_CTX_free(ctx);
    BN_free(const_c);

    return 0;
}