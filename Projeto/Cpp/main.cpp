//
//  main.cpp
//

// g++ main.cpp -o slave libcryptopp.a

#include <iostream>
#include "cryptopp/hex.h"
#include "cryptopp/sha.h"
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "cryptopp/md5.h"
#include "cryptopp/filters.h"
#include "cryptopp/blowfish.h"
#include "cryptopp/aes.h"
#include "cryptopp/modes.h"
#include "cryptopp/secblock.h"
#include "cryptopp/files.h"
#include "cryptopp/des.h"
#include "cryptopp/integer.h"
#include "cryptopp/rsa.h"
#include "cryptopp/osrng.h"

using namespace std;
using namespace CryptoPP;

char* data_file_path;
int mode;

unsigned char* key = (unsigned char*)"ligTNvexz0Zr1cJQEBrEbRwyHkTYJORV";
unsigned char* iv = (unsigned char*)"9ycqc1GAguyrdHVXdXQBGbA4MSTD44G3";

char* rsa_key_n = (char*)"0x00dd3b4da85453efe54bcea931c356e61ea4f2bbe9f5f363a8803c74c6f862310d9e8926b4744f5cf0efc5ad3a2bc4455e0cd8febc09ab9928279f768381142ec8b55033be4aa998512bf1c67ec6625a402a6b5b92900d1ca0c01de485436862d48e1f74a81bf72cf5e1ce46f041237e7b0c81e87a0d9f65711dfed3219a260a213a6da771a4885063ed26335a0e7d3b22415cd0dd4e3c4c641f856301a32189a5bba26a2f3a9f6ae0e293a1c2dc52d4125506ea26c2eedb5fcce48fc862b26840f40d384aeedfa5f8240e47bde4f02f93c0547cf0ee0d829ac92a827f3a5852a83d56b9c017e0f54955293433aecf4b3bf4e0f24c05aa06c2f499c1eb129267a70adbe351f947d46bcb5c541c608defabfa9152eddf2f30906d72938a063e3a0613b8c5b82e7c0cd6da71dcca78dde25cfab6cd9973b1207fa9a49564e580f7cd5cc672cfaf6aba50564633851ea47df824fc09d9fb8d4490dda39492caa6b8ec0d2a7b5dc30e508b03dd57a9144d1f2d34f76eecb4f47ad9157e0fec183ed611bdb3b771e7be484bac27139706483ee01c0c2a18e50632759192b491532b41227718627718d95e6b845c2c7c6118118d4d355ae121b26e329860baabf509eded99d57c54e9330fcd66fb92792132624dd3db5cfbf1a8bfa291388f12e1f77a114df84f4b46d8a6415f5775c634d6fdb7b484c2623cec31551a53bb23758d1d2b";
char* rsa_key_e = (char*)"0x10001";

// MARK: - Cryptopp

#define ENCRYPT_MACRO(algorithm, keysize) \
CBC_Mode<algorithm>::Encryption alg; \
alg.SetKeyWithIV(key, keysize, iv); \
FileSource file(data_file_path, true, new StreamTransformationFilter(alg, new FileSink("/dev/null")));

#define DECRYPT_MACRO(algorithm, keysize) \
CBC_Mode<algorithm>::Decryption alg; \
alg.SetKeyWithIV(key, keysize, iv); \
FileSource file(data_file_path, true, new StreamTransformationFilter(alg, new FileSink("/dev/null")));

#define DIGEST_MACRO(algorithm) \
algorithm hash; \
FileSource file(data_file_path, true, new HashFilter(hash, new FileSink("/dev/null"))); \

void cryptopp_md5() {
    DIGEST_MACRO(Weak::MD5)
}
void cryptopp_sha1() {
    DIGEST_MACRO(SHA1)
}
void cryptopp_sha256() {
    DIGEST_MACRO(SHA256)
}
void cryptopp_3des() {
    if (mode == 0) { ENCRYPT_MACRO(DES_EDE3, 24) }
    else if (mode == 1) { DECRYPT_MACRO(DES_EDE3, 24) }
}
void cryptopp_aes256() {
    if (mode == 0) { ENCRYPT_MACRO(AES, 32) }
    else if (mode == 1) { DECRYPT_MACRO(AES, 32) }
}
void cryptopp_blowfish() {
    if (mode == 0) { ENCRYPT_MACRO(Blowfish, 16) }
    else if (mode == 1) { DECRYPT_MACRO(Blowfish, 16) }
}


void cryptopp_rsa() {
    static Integer n(rsa_key_n);
    static Integer e(rsa_key_e);
    static RSA::PublicKey publicKey;
    static bool initialized = false;
    if (!initialized) {
        publicKey.Initialize(n, e);
        initialized = true;
    }
    
    AutoSeededRandomPool rng;
    RSAES_OAEP_SHA_Encryptor enc(publicKey);
    FileSource file(data_file_path, true, new PK_EncryptorFilter(rng, enc, new FileSink("/dev/null")));
}

// MARK: - MAIN

typedef void (*fptr)(void);
fptr functions[10][10] = {
    {cryptopp_md5, cryptopp_sha1, cryptopp_sha256, cryptopp_3des, cryptopp_aes256, cryptopp_blowfish, cryptopp_rsa},
};

int main(int argc, char* argv[]) {
    if (argc < 6) {
        cout << "Incomplete argument list" << endl;
        return -1;
    }

    data_file_path = argv[1];
    int iter_count = strtol(argv[2], (char**)NULL, 10);
    int algorithm = strtol(argv[3], (char**)NULL, 10);
    int library = strtol(argv[4], (char**)NULL, 10);
    mode = strtol(argv[5], (char**)NULL, 10);
    
    if (functions[library][algorithm]) {
        int i;
        for (i = 0; i < iter_count; i++) {
            functions[library][algorithm]();
        }
    }
    else {
        cout << "Library/algorithm combination unavailable" << endl;
    }
    
    return 0;
}
