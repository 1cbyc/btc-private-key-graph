#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include <stddef.h>

#define PRIVATE_KEY_SIZE 32
#define PUBLIC_KEY_SIZE 65
#define COMPRESSED_PUBLIC_KEY_SIZE 33
#define ADDRESS_SIZE 34
#define WIF_SIZE 52

typedef struct {
    uint8_t data[PRIVATE_KEY_SIZE];
} private_key_t;

typedef struct {
    uint8_t data[PUBLIC_KEY_SIZE];
    size_t length;
} public_key_t;

typedef struct {
    uint8_t data[ADDRESS_SIZE];
    size_t length;
} bitcoin_address_t;

int crypto_init(void);
void crypto_cleanup(void);
int generate_secure_private_key(private_key_t *key);
int validate_private_key(const private_key_t *key);
int derive_public_key(const private_key_t *private_key, public_key_t *public_key);
int derive_compressed_public_key(const private_key_t *private_key, public_key_t *public_key);
int generate_bitcoin_address(const public_key_t *public_key, bitcoin_address_t *address);
int private_key_to_wif(const private_key_t *key, char *wif, size_t wif_size);
int wif_to_private_key(const char *wif, private_key_t *key);
void secure_zero_memory(void *ptr, size_t size);

#endif 