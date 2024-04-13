#include <fstream>
#include <iostream>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/pkcs7.h>
#include <openssl/sm2.h>
#include <openssl/sm3.h>
#include <openssl/x509.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
using namespace std;

const char* CACert = "\
-----BEGIN CERTIFICATE----- \n\
MIIB/zCCAaagAwIBAgIJAKKa0PAt9M1FMAoGCCqBHM9VAYN1MFsxCzAJBgNVBAYT \n\
AkNOMQ4wDAYDVQQIDAVIdUJlaTEOMAwGA1UEBwwFV3VIYW4xDTALBgNVBAoMBEhV \n\
U1QxDDAKBgNVBAsMA0NTRTEPMA0GA1UEAwwGY2Fyb290MB4XDTIwMDkyMDIwNTkx \n\
OVoXDTMwMDkxODIwNTkxOVowWzELMAkGA1UEBhMCQ04xDjAMBgNVBAgMBUh1QmVp \n\
MQ4wDAYDVQQHDAVXdUhhbjENMAsGA1UECgwESFVTVDEMMAoGA1UECwwDQ1NFMQ8w \n\
DQYDVQQDDAZjYXJvb3QwWTATBgcqhkjOPQIBBggqgRzPVQGCLQNCAASJ8mm28JJR \n\
bZKLr6DCo1+KWimpKEsiTfZM19Zi5ao7Au6YLosyN71256MWmjwkwXxJeLa0lCfm \n\
kF/YWCX6qGQ0o1MwUTAdBgNVHQ4EFgQUAL5hW3RUzqvsiTzIc1gUHeK5uzQwHwYD \n\
VR0jBBgwFoAUAL5hW3RUzqvsiTzIc1gUHeK5uzQwDwYDVR0TAQH/BAUwAwEB/zAK \n\
BggqgRzPVQGDdQNHADBEAiAaZMmvE5zzXHx/TBgdUhjtpRH3Jpd6OZ+SOAfMtKxD \n\
LAIgdKq/v2Jkmn37Y9U8FHYDfFqk5I0qlQOAmuvbVUi3yvM= \n\
-----END CERTIFICATE----- \n\
";

const char PKeyB[] = "\
-----BEGIN EC PARAMETERS----- \n\
BggqgRzPVQGCLQ== \n\
-----END EC PARAMETERS----- \n\
-----BEGIN EC PRIVATE KEY----- \n\
MHcCAQEEINQhCKslrI3tKt6cK4Kxkor/LBvM8PSv699Xea7kTXTToAoGCCqBHM9V \n\
AYItoUQDQgAEH7rLLiFASe3SWSsGbxFUtfPY//pXqLvgM6ROyiYhLkPxEulwrTe8 \n\
kv5R8/NA7kSSvcsGIQ9EPWhr6HnCULpklw== \n\
-----END EC PRIVATE KEY----- \n\
";

char fin[10000];

X509* getX509(const char* cert)
{
    BIO* bio;
    bio = BIO_new(BIO_s_mem());
    BIO_puts(bio, cert);
    return PEM_read_bio_X509(bio, NULL, NULL, NULL);
}
EVP_PKEY* getpkey(const char* private_key)
{
    BIO* bio_pkey = BIO_new_mem_buf((char*)private_key, strlen(private_key));
    if (bio_pkey == NULL)
        return NULL;
    return PEM_read_bio_PrivateKey(bio_pkey, NULL, NULL, NULL);
}

// main functions---------------------------------------------------
// calculate encode message, check encode, print result
void encode_pkcs7(void);
// main functions---------------------------------------------------

void encode_pkcs7(void)
{
    /*
    BIO *bio_out, *mem;
    PKCS7 *p7;
    X509_STORE *cal;
    size_t length;
    char buf[100], fin[5000];
    int signerCount;
    string input = "";
    */

    // var mem storing BIO*
    BIO* mem = BIO_new_fd(fileno(stdin), BIO_NOCLOSE);

    // decode
    // original key
    EVP_PKEY* p_key = getpkey(PKeyB);
    PKCS7* p7 = PEM_read_bio_PKCS7(mem, NULL, NULL, NULL);
    free(mem);
    BIO* bio_out = PKCS7_dataDecode(p7, p_key, NULL, NULL);
    int length = BIO_read(bio_out, fin, 10000);
    if (length <= 0) {
        printf("ERROR\n");
        return;
    }

    // signature
    STACK_OF(PKCS7_SIGNER_INFO)* sk = PKCS7_get_signer_info(p7);
    if ((!sk) == true) {
        printf("ERROR\n");
        return;
    }
    // start adding certification
    int n = sk_PKCS7_SIGNER_INFO_num(sk);
    PKCS7_SIGNER_INFO* sign_info;
    X509_STORE* cal = X509_STORE_new();
    X509_STORE_CTX* ctx = X509_STORE_CTX_new();
    X509_STORE_add_cert(cal, getX509(CACert));
    // sign cal
    for (int j = 0; j < n; j++) {
        sign_info = sk_PKCS7_SIGNER_INFO_value(sk, j);
        if (PKCS7_dataVerify(cal, ctx, bio_out, p7, sign_info) <= 0) {
            printf("ERROR\n");
            return;
        }
    }

    // check if fin[] is not legal
    for (int i = 0; i < length; i++) {
        if (isprint(fin[i]) == 0) {
            printf("ERROR\n");
            return;
        }
    }

    // end
    fin[length] = 0;
    if (fin[length] == 0) {
        printf("%s", fin);
    }

    return;
}

int main(void)
{
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    encode_pkcs7();
    return 0;
}