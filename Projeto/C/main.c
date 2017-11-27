//
//  main.c
//

// gcc main.c -o slave -lgcrypt -lssl -lcrypto -L/usr/local/opt/openssl/lib -I/usr/local/opt/openssl/include libnacl.a

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define FILE_READ_BLOCK_SIZE 4096 // Smaller values create an IO bottleneck and algorithms are slower

FILE *data_file;
int mode;

unsigned char* key = (unsigned char*)"ligTNvexz0Zr1cJQEBrEbRwyHkTYJORV";
unsigned char* iv = (unsigned char*)"9ycqc1GAguyrdHVXdXQBGbA4MSTD44G3";

char* rsa_key_n = "0x00dd3b4da85453efe54bcea931c356e61ea4f2bbe9f5f363a8803c74c6f862310d9e8926b4744f5cf0efc5ad3a2bc4455e0cd8febc09ab9928279f768381142ec8b55033be4aa998512bf1c67ec6625a402a6b5b92900d1ca0c01de485436862d48e1f74a81bf72cf5e1ce46f041237e7b0c81e87a0d9f65711dfed3219a260a213a6da771a4885063ed26335a0e7d3b22415cd0dd4e3c4c641f856301a32189a5bba26a2f3a9f6ae0e293a1c2dc52d4125506ea26c2eedb5fcce48fc862b26840f40d384aeedfa5f8240e47bde4f02f93c0547cf0ee0d829ac92a827f3a5852a83d56b9c017e0f54955293433aecf4b3bf4e0f24c05aa06c2f499c1eb129267a70adbe351f947d46bcb5c541c608defabfa9152eddf2f30906d72938a063e3a0613b8c5b82e7c0cd6da71dcca78dde25cfab6cd9973b1207fa9a49564e580f7cd5cc672cfaf6aba50564633851ea47df824fc09d9fb8d4490dda39492caa6b8ec0d2a7b5dc30e508b03dd57a9144d1f2d34f76eecb4f47ad9157e0fec183ed611bdb3b771e7be484bac27139706483ee01c0c2a18e50632759192b491532b41227718627718d95e6b845c2c7c6118118d4d355ae121b26e329860baabf509eded99d57c54e9330fcd66fb92792132624dd3db5cfbf1a8bfa291388f12e1f77a114df84f4b46d8a6415f5775c634d6fdb7b484c2623cec31551a53bb23758d1d2b";
char* rsa_key_e = "0x10001";
char* rsa_key_openssl = "-----BEGIN PUBLIC KEY-----\nMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA3TtNqFRT7+VLzqkxw1bm\nHqTyu+n182OogDx0xvhiMQ2eiSa0dE9c8O/FrTorxEVeDNj+vAmrmSgnn3aDgRQu\nyLVQM75KqZhRK/HGfsZiWkAqa1uSkA0coMAd5IVDaGLUjh90qBv3LPXhzkbwQSN+\newyB6HoNn2VxHf7TIZomCiE6badxpIhQY+0mM1oOfTsiQVzQ3U48TGQfhWMBoyGJ\npbuiai86n2rg4pOhwtxS1BJVBuomwu7bX8zkj8hismhA9A04Su7fpfgkDke95PAv\nk8BUfPDuDYKaySqCfzpYUqg9VrnAF+D1SVUpNDOuz0s79ODyTAWqBsL0mcHrEpJn\npwrb41H5R9Rry1xUHGCN76v6kVLt3y8wkG1yk4oGPjoGE7jFuC58DNbacdzKeN3i\nXPq2zZlzsSB/qaSVZOWA981cxnLPr2q6UFZGM4UepH34JPwJ2fuNRJDdo5SSyqa4\n7A0qe13DDlCLA91XqRRNHy00927stPR62RV+D+wYPtYRvbO3cee+SEusJxOXBkg+\n4BwMKhjlBjJ1kZK0kVMrQSJ3GGJ3GNlea4RcLHxhGBGNTTVa4SGybjKYYLqr9Qnt\n7ZnVfFTpMw/NZvuSeSEyYk3T21z78ai/opE4jxLh93oRTfhPS0bYpkFfV3XGNNb9\nt7SEwmI87DFVGlO7I3WNHSsCAwEAAQ==\n-----END PUBLIC KEY-----";


char* to_hex(unsigned char* bytes, unsigned long long length) {
    char* out = (char*)malloc(length * 2);
    unsigned long long n;
    for (n = 0; n < length; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)bytes[n]);
    }
    return out;
}
void print_bytes(unsigned char* bytes, unsigned long long length) {
    char* out = to_hex(bytes, length);
    printf("%s\n", out);
    free(out);
}
unsigned long long load_file(FILE* file, unsigned char** input) {
    *input = NULL;
    unsigned long long length;
    
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    *input = malloc(length);
    
    if (input) {
        fread(*input, 1, length, file);
        return length;
    }
    else {
        return 0;
    }
}

// MARK: - libgcrypt

#include <gcrypt.h>

void gcrypt_init() {
    static int initialised = 0;
    if (initialised) { return; }
    
    // Version check should be the very first call because it
    // makes sure that important subsystems are initialized
    if (!gcry_check_version(GCRYPT_VERSION)) {
        printf("libgcrypt version mismatch\n");
        exit(-1);
    }
    
    // Disable secure memory
    gcry_control(GCRYCTL_DISABLE_SECMEM, 0);
    
    // Tell Libgcrypt that initialization has completed
    gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
    
    initialised = 1;
}

void gcrypt_digest(int hash) {
    gcrypt_init();
    
    int block_size = FILE_READ_BLOCK_SIZE;
    unsigned char input[block_size];
    
    gcry_md_hd_t md;
    gcry_md_open(&md, hash, 0);
    
    size_t bytesRead = 0;
    while ((bytesRead = fread(input, 1, block_size, data_file)) > 0) {
        gcry_md_write(md, input, bytesRead);
    }
    
    // Simulate a read because this envolves finalizing the hash
    gcry_md_read(md, hash);
    //print_bytes(gcry_md_read(md, hash), gcry_md_get_algo_dlen(hash));
    
    gcry_md_close(md);
}
void gcrypt_encrypt(int algorithm) {
    gcrypt_init();
    
    int block_size = FILE_READ_BLOCK_SIZE;
    unsigned char input[block_size];
    unsigned char output[block_size];
    
    gcry_cipher_hd_t cipher;
    gcry_cipher_open(&cipher, algorithm, GCRY_CIPHER_MODE_CBC, 0);
    
    gcry_cipher_setkey(cipher, key, gcry_cipher_get_algo_keylen(algorithm));
    gcry_cipher_setiv(cipher, iv, gcry_cipher_get_algo_blklen(algorithm));
    
    size_t bytesRead = 0;
    while ((bytesRead = fread(input, 1, block_size, data_file)) > 0) {
        // Depending on the selected algorithms and encryption mode, the length of the buffers must be a multiple of the block size
        // Since padding is not applied, we need to give this function the total block size "block_size", not "bytesRead"
        gcry_cipher_encrypt(cipher, output, block_size, input, bytesRead);
        //print_bytes(output, block_size);
    }
    
    gcry_cipher_close(cipher);
}
void gcrypt_decrypt(int algorithm) {
    gcrypt_init();
    
    int block_size = FILE_READ_BLOCK_SIZE;
    unsigned char input[block_size];
    unsigned char output[block_size];
    
    gcry_cipher_hd_t cipher;
    gcry_cipher_open(&cipher, algorithm, GCRY_CIPHER_MODE_CBC, 0);
    
    gcry_cipher_setkey(cipher, key, gcry_cipher_get_algo_keylen(algorithm));
    gcry_cipher_setiv(cipher, iv, gcry_cipher_get_algo_blklen(algorithm));
    
    size_t bytesRead = 0;
    while ((bytesRead = fread(input, 1, block_size, data_file)) > 0) {
        // Depending on the selected algorithms and encryption mode, the length of the buffers must be a multiple of the block size
        // Since padding is not applied, we need to give this function the total block size "block_size", not "bytesRead"
        gcry_cipher_decrypt(cipher, output, block_size, input, bytesRead);
        //print_bytes(output, block_size);
    }
    
    gcry_cipher_close(cipher);
}

void gcrypt_md5() {
    gcrypt_digest(GCRY_MD_MD5);
}
void gcrypt_sha1() {
    gcrypt_digest(GCRY_MD_SHA1);
}
void gcrypt_sha256() {
    gcrypt_digest(GCRY_MD_SHA256);
}
void gcrypt_3des() {
    if (mode == 0) { gcrypt_encrypt(GCRY_CIPHER_3DES); }
    else if (mode == 1) { gcrypt_decrypt(GCRY_CIPHER_3DES); }
}
void gcrypt_aes256() {
    if (mode == 0) { gcrypt_encrypt(GCRY_CIPHER_AES256); }
    else if (mode == 1) { gcrypt_decrypt(GCRY_CIPHER_AES256); }
}
void gcrypt_blowfish() {
    if (mode == 0) { gcrypt_encrypt(GCRY_CIPHER_BLOWFISH); }
    else if (mode == 1) { gcrypt_decrypt(GCRY_CIPHER_BLOWFISH); }
}
void gcrypt_rsa() {
    gcrypt_init();
    
    size_t erroff;
    static gcry_mpi_t n, e;
    static gcry_sexp_t key;
    static int initialized = 0;
    static char* data_bytes_in_hex;
    
    if (!initialized) {
        gcry_mpi_scan(&n, GCRYMPI_FMT_HEX, rsa_key_n, 0, NULL);
        gcry_mpi_scan(&e, GCRYMPI_FMT_HEX, rsa_key_e, 0, NULL);
        gcry_sexp_build(&key, &erroff, "(public-key (rsa (n %M) (e %M)))", n, e);
        
        unsigned char* input = NULL;
        unsigned long long length = load_file(data_file, &input);
        data_bytes_in_hex = to_hex(input, length); // Free after use. Not done here because "after use" is at the end of the program's execution
        
        initialized = 1;
    }
    
    gcry_mpi_t data_mpi;
    gcry_mpi_scan(&data_mpi, GCRYMPI_FMT_HEX, data_bytes_in_hex, 0, NULL);
    
    //gcry_mpi_dump(data_mpi);
    
    gcry_sexp_t data;
    gcry_sexp_build(&data, &erroff, "(data (flags oaep) (value %M))", data_mpi);
    
    gcry_sexp_t cipher;
    gcry_pk_encrypt(&cipher, data, key);
    
    //gcry_sexp_dump(cipher);

    gcry_mpi_release(data_mpi);
    gcry_sexp_release(data);
    gcry_sexp_release(cipher);
}

// MARK: - NaCl

//#include "nacl/crypto_hash_sha256.h"

// void nacl_sha256() {
//     static unsigned char* input = NULL;
//     static unsigned long long length;
//     static int initialized = 0;
    
//     if (!initialized) {
//         length = load_file(data_file, &input);
//         if (!input) {
//             printf("Malloc failed on nacl_sha256\n");
//             exit(-1);
//         }
//         initialized = 1;
//     }
    
//     unsigned char digest[32];
//     crypto_hash_sha256(digest, input, length);
    
//     //print_bytes(digest, 32);
// }

// MARK: - OpenSSL

#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

void openssl_digest(const EVP_MD* hash) {
    int read_block_size = FILE_READ_BLOCK_SIZE;
    unsigned char input[read_block_size];
    
    EVP_MD_CTX *mdctx;
    unsigned char *digest;
    
    if ((mdctx = EVP_MD_CTX_create()) == NULL) exit(-1);
    
    if (1 != EVP_DigestInit_ex(mdctx, hash, NULL)) exit(-1);
    
    size_t bytesRead = 0;
    while ((bytesRead = fread(input, 1, sizeof(input), data_file)) > 0) {
        if (1 != EVP_DigestUpdate(mdctx, input, bytesRead)) exit(-1);
    }
    
    if ((digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(hash))) == NULL) exit(-1);
    
    unsigned int digest_len;
    if (1 != EVP_DigestFinal_ex(mdctx, digest, &digest_len)) exit(-1);
    
    //print_bytes(digest, digest_len);
    
    OPENSSL_free(digest);
    EVP_MD_CTX_destroy(mdctx);
    
}
void openssl_encrypt(const EVP_CIPHER* cipher) {
    int read_block_size = FILE_READ_BLOCK_SIZE;
    unsigned char input[read_block_size];
    unsigned char output[read_block_size];
    
    EVP_CIPHER_CTX *ctx;
    int written_length;
    
    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) exit(-1);
    
    /* Initialise the encryption operation */
    if (1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv)) exit(-1);
    
    size_t bytesRead = 0;
    while ((bytesRead = fread(input, 1, sizeof(input), data_file)) > 0) {
        /* Provide the message to be encrypted, and obtain the encrypted output.
         * EVP_EncryptUpdate can be called multiple times if necessary
         */
        
        if (1 != EVP_EncryptUpdate(ctx, output, &written_length, input, bytesRead)) exit(-1);
        //print_bytes(output, written_length);
    }
    
    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (1 != EVP_EncryptFinal_ex(ctx, output, &written_length)) exit(-1);
    
    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
}
void openssl_decrypt(const EVP_CIPHER* cipher) {
    int read_block_size = FILE_READ_BLOCK_SIZE;
    unsigned char input[read_block_size];
    unsigned char output[read_block_size];
    
    EVP_CIPHER_CTX *ctx;
    int written_length;
    
    /* Create and initialise the context */
    if (!(ctx = EVP_CIPHER_CTX_new())) exit(-1);
    
    /* Initialise the encryption operation */
    if (1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv)) exit(-1);
    
    size_t bytesRead = 0;
    while ((bytesRead = fread(input, 1, sizeof(input), data_file)) > 0) {
        /* Provide the message to be encrypted, and obtain the encrypted output.
         * EVP_EncryptUpdate can be called multiple times if necessary
         */
        if (1 != EVP_DecryptUpdate(ctx, output, &written_length, input, bytesRead)) exit(-1);
        //print_bytes(output, written_length);
    }
    
    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if (1 != EVP_DecryptFinal_ex(ctx, output, &written_length)) exit(-1);
    
    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
}

void openssl_md5() {
    openssl_digest(EVP_md5());
}
void openssl_sha1() {
    openssl_digest(EVP_sha1());
}
void openssl_sha256() {
    openssl_digest(EVP_sha256());
}
void openssl_3des() {
    if (mode == 0) { openssl_encrypt(EVP_des_ede3_cbc()); }
    else if (mode == 1) { openssl_decrypt(EVP_des_ede3_cbc()); }
}
void openssl_aes256() {
    if (mode == 0) { openssl_encrypt(EVP_aes_256_cbc()); }
    else if (mode == 1) { openssl_decrypt(EVP_aes_256_cbc()); }
}
void openssl_blowfish() {
    if (mode == 0) { openssl_encrypt(EVP_bf_cbc()); }
    else if (mode == 1) { openssl_decrypt(EVP_bf_cbc()); }
}
void openssl_rsa() {
    static RSA* key;
    static unsigned char* input;
    static unsigned long long length;
    static int initialized = 0;
    
    if (!initialized) {
        BIO* bio = BIO_new_mem_buf(rsa_key_openssl, -1);
        key = PEM_read_bio_RSA_PUBKEY(bio, &key, NULL, NULL);
        length = load_file(data_file, &input);
        
        initialized = 1;
    }
    
    unsigned char encrypted[4096] = {0};
    int result = RSA_public_encrypt(length, input, encrypted, key, RSA_PKCS1_OAEP_PADDING);
    //print_bytes(encrypted, 4096);
}

// MARK: - MAIN

typedef void (*fptr)(void);
fptr functions[10][10] = {
    {openssl_md5, openssl_sha1, openssl_sha256, openssl_3des, openssl_aes256, openssl_blowfish, openssl_rsa},
    {gcrypt_md5, gcrypt_sha1, gcrypt_sha256, gcrypt_3des, gcrypt_aes256, gcrypt_blowfish, gcrypt_rsa}
    //{NULL, NULL, nacl_sha256, NULL, NULL, NULL, NULL},
};


int main(int argc, char **argv) {
    if (argc < 6) {
        printf("Incomplete argument list\n");
        return -1;
    }
    
    data_file = fopen(argv[1], "rb");
    
    int iter_count = strtol(argv[2], (char**)NULL, 10);
    int algorithm = strtol(argv[3], (char**)NULL, 10);
    int library = strtol(argv[4], (char**)NULL, 10);
    mode = strtol(argv[5], (char**)NULL, 10);
    
    if (functions[library][algorithm]) {
        int i;
        for (i = 0; i < iter_count; i++) {
            rewind(data_file);
            functions[library][algorithm]();
        }
    }
    else {
        printf("Library/algorithm combination unavailable\n");
    }
    
    fclose(data_file);
    return 0;
}

