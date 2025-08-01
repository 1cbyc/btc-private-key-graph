#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include "crypto.h"
#include "utils.h"

static EC_GROUP *secp256k1_group = NULL;
static BN_CTX *bn_ctx = NULL;

int crypto_init(void) {
    OpenSSL_add_all_algorithms();
    secp256k1_group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    if (!secp256k1_group) {
        return -1;
    }
    bn_ctx = BN_CTX_new();
    if (!bn_ctx) {
        EC_GROUP_free(secp256k1_group);
        return -1;
    }
    return 0;
}

void crypto_cleanup(void) {
    if (bn_ctx) {
        BN_CTX_free(bn_ctx);
        bn_ctx = NULL;
    }
    if (secp256k1_group) {
        EC_GROUP_free(secp256k1_group);
        secp256k1_group = NULL;
    }
    EVP_cleanup();
}

int generate_secure_private_key(private_key_t *key) {
    if (!key) return -1;
    
    BIGNUM *bn = BN_new();
    if (!bn) return -1;
    
    int result = -1;
    do {
        if (!BN_rand_range(bn, EC_GROUP_get0_order(secp256k1_group))) {
            break;
        }
        
        if (BN_is_zero(bn)) {
            continue;
        }
        
        if (BN_num_bytes(bn) > PRIVATE_KEY_SIZE) {
            continue;
        }
        
        memset(key->data, 0, PRIVATE_KEY_SIZE);
        BN_bn2bin(bn, key->data + PRIVATE_KEY_SIZE - BN_num_bytes(bn));
        result = 0;
    } while (0);
    
    BN_free(bn);
    return result;
}

int validate_private_key(const private_key_t *key) {
    if (!key) return -1;
    
    BIGNUM *bn = BN_new();
    if (!bn) return -1;
    
    BN_bin2bn(key->data, PRIVATE_KEY_SIZE, bn);
    
    BIGNUM *order = BN_new();
    if (!order) {
        BN_free(bn);
        return -1;
    }
    
    EC_GROUP_get_order(secp256k1_group, order, bn_ctx);
    
    int result = (BN_is_zero(bn) || BN_cmp(bn, order) >= 0) ? -1 : 0;
    
    BN_free(order);
    BN_free(bn);
    return result;
}

int derive_public_key(const private_key_t *private_key, public_key_t *public_key) {
    if (!private_key || !public_key) return -1;
    
    EC_KEY *ec_key = EC_KEY_new();
    if (!ec_key) return -1;
    
    if (!EC_KEY_set_group(ec_key, secp256k1_group)) {
        EC_KEY_free(ec_key);
        return -1;
    }
    
    BIGNUM *bn = BN_bin2bn(private_key->data, PRIVATE_KEY_SIZE, NULL);
    if (!bn) {
        EC_KEY_free(ec_key);
        return -1;
    }
    
    if (!EC_KEY_set_private_key(ec_key, bn)) {
        BN_free(bn);
        EC_KEY_free(ec_key);
        return -1;
    }
    
    const EC_POINT *pub_point = EC_KEY_get0_public_key(ec_key);
    if (!pub_point) {
        BN_free(bn);
        EC_KEY_free(ec_key);
        return -1;
    }
    
    size_t pub_len = EC_POINT_point2oct(secp256k1_group, pub_point, 
                                       POINT_CONVERSION_UNCOMPRESSED,
                                       public_key->data, PUBLIC_KEY_SIZE, bn_ctx);
    
    if (pub_len == 0) {
        BN_free(bn);
        EC_KEY_free(ec_key);
        return -1;
    }
    
    public_key->length = pub_len;
    
    BN_free(bn);
    EC_KEY_free(ec_key);
    return 0;
}

int derive_compressed_public_key(const private_key_t *private_key, public_key_t *public_key) {
    if (!private_key || !public_key) return -1;
    
    EC_KEY *ec_key = EC_KEY_new();
    if (!ec_key) return -1;
    
    if (!EC_KEY_set_group(ec_key, secp256k1_group)) {
        EC_KEY_free(ec_key);
        return -1;
    }
    
    BIGNUM *bn = BN_bin2bn(private_key->data, PRIVATE_KEY_SIZE, NULL);
    if (!bn) {
        EC_KEY_free(ec_key);
        return -1;
    }
    
    if (!EC_KEY_set_private_key(ec_key, bn)) {
        BN_free(bn);
        EC_KEY_free(ec_key);
        return -1;
    }
    
    const EC_POINT *pub_point = EC_KEY_get0_public_key(ec_key);
    if (!pub_point) {
        BN_free(bn);
        EC_KEY_free(ec_key);
        return -1;
    }
    
    size_t pub_len = EC_POINT_point2oct(secp256k1_group, pub_point, 
                                       POINT_CONVERSION_COMPRESSED,
                                       public_key->data, COMPRESSED_PUBLIC_KEY_SIZE, bn_ctx);
    
    if (pub_len == 0) {
        BN_free(bn);
        EC_KEY_free(ec_key);
        return -1;
    }
    
    public_key->length = pub_len;
    
    BN_free(bn);
    EC_KEY_free(ec_key);
    return 0;
}

int generate_bitcoin_address(const public_key_t *public_key, bitcoin_address_t *address) {
    if (!public_key || !address) return -1;
    
    uint8_t sha256_hash[32];
    uint8_t ripemd160_hash[20];
    
    SHA256(public_key->data, public_key->length, sha256_hash);
    RIPEMD160(sha256_hash, 32, ripemd160_hash);
    
    address->data[0] = VERSION_BYTE_MAINNET;
    memcpy(address->data + 1, ripemd160_hash, 20);
    
    uint8_t checksum[CHECKSUM_SIZE];
    SHA256(SHA256(address->data, 21, NULL), 32, checksum);
    memcpy(address->data + 21, checksum, CHECKSUM_SIZE);
    
    address->length = 25;
    return 0;
}

int private_key_to_wif(const private_key_t *key, char *wif, size_t wif_size) {
    if (!key || !wif || wif_size < WIF_SIZE) return -1;
    
    uint8_t extended_key[37];
    extended_key[0] = 0x80;
    memcpy(extended_key + 1, key->data, PRIVATE_KEY_SIZE);
    extended_key[33] = 0x01;
    
    uint8_t checksum[CHECKSUM_SIZE];
    SHA256(SHA256(extended_key, 34, NULL), 32, checksum);
    memcpy(extended_key + 34, checksum, CHECKSUM_SIZE);
    
    return base58_encode(extended_key, 38, wif, wif_size);
}

int wif_to_private_key(const char *wif, private_key_t *key) {
    if (!wif || !key) return -1;
    
    uint8_t decoded[38];
    size_t decoded_len;
    
    if (base58_decode(wif, decoded, sizeof(decoded)) != 0) {
        return -1;
    }
    
    if (decoded_len != 38 || decoded[0] != 0x80) {
        return -1;
    }
    
    uint8_t checksum[CHECKSUM_SIZE];
    SHA256(SHA256(decoded, 34, NULL), 32, checksum);
    
    if (memcmp(checksum, decoded + 34, CHECKSUM_SIZE) != 0) {
        return -1;
    }
    
    memcpy(key->data, decoded + 1, PRIVATE_KEY_SIZE);
    return 0;
}

void secure_zero_memory(void *ptr, size_t size) {
    if (!ptr) return;
    
    volatile uint8_t *p = (volatile uint8_t *)ptr;
    while (size--) {
        *p++ = 0;
    }
} 