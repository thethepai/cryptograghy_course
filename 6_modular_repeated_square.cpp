#include <openssl/bn.h>
#include <stdio.h>

// main functions---------------------------------------------------
// calculate modular repeated square
BIGNUM* modular_cal(BIGNUM* m, BIGNUM* e, BIGNUM* n);
// main functions---------------------------------------------------

BIGNUM* modular_cal(BIGNUM* m, BIGNUM* e, BIGNUM* n)
{
    BIGNUM* fin = BN_new();
    BN_CTX* ctx = BN_CTX_new();
    // create montgomery temp ctx
    BN_MONT_CTX* mont = BN_MONT_CTX_new();
    BN_MONT_CTX_set(mont, n, ctx);

    // calculate with montgomery method
    BN_mod_exp_mont(fin, m, e, n, ctx, mont);

    // free var
    BN_CTX_free(ctx);
    BN_MONT_CTX_free(mont);

    return fin;
}

int main()
{
    int count;
    scanf("%d", &count);

    for (int i = 0; i < count; i++) {
        // var
        BIGNUM* e = BN_new();
        BIGNUM* m = BN_new();
        BIGNUM* p = BN_new();
        BIGNUM* q = BN_new();
        BIGNUM* n = BN_new();
        BIGNUM* phi = BN_new();
        BN_CTX* ctx = BN_CTX_new();

        // read
        char e_string[2048];
        char m_string[2048];
        char p_string[2048];
        char q_string[2048];
        scanf("%s %s %s %s", e_string, m_string, p_string, q_string);
        BN_dec2bn(&e, e_string);
        BN_dec2bn(&m, m_string);
        BN_dec2bn(&p, p_string);
        BN_dec2bn(&q, q_string);

        // calculate
        BN_mul(n, p, q, ctx);

        // decrease the value of E
        BN_sub_word(p, 1);
        BN_sub_word(q, 1);
        BN_mul(phi, p, q, ctx);
        BN_mod(e, e, phi, ctx);
        // montgomery method
        char* fin = BN_bn2dec(modular_cal(m, e, n));

        printf("%s\n", fin);

        // test???
        /*
        OPENSSL_free(fin);
        BN_clear_free(e);
        BN_clear_free(m);
        BN_clear_free(p);
        BN_clear_free(q);
        BN_clear_free(n);
        BN_clear_free(phi);
        BN_CTX_free(ctx);
        */
    }

    return 0;
}