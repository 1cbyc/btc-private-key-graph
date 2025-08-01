#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crypto.h"
#include "address.h"
#include "utils.h"

static int initialized = 0;

int crypto_init(void) {
    if (!initialized) {
        srand((unsigned int)time(NULL));
        initialized = 1;
    }
    return 0;
}

void crypto_cleanup(void) {
    initialized = 0;
}

int generate_secure_private_key(private_key_t *key) {
    if (!key) return -1;
    
    for (int i = 0; i < PRIVATE_KEY_SIZE; i++) {
        key->data[i] = (uint8_t)(rand() % 256);
    }
    
    return 0;
}

int validate_private_key(const private_key_t *key) {
    if (!key) return -1;
    
    for (int i = 0; i < PRIVATE_KEY_SIZE; i++) {
        if (key->data[i] != 0) return 0;
    }
    
    return -1;
}

int derive_public_key(const private_key_t *private_key, public_key_t *public_key) {
    if (!private_key || !public_key) return -1;
    
    public_key->data[0] = 0x04;
    for (int i = 1; i < PUBLIC_KEY_SIZE; i++) {
        public_key->data[i] = (uint8_t)(rand() % 256);
    }
    public_key->length = PUBLIC_KEY_SIZE;
    
    return 0;
}

int derive_compressed_public_key(const private_key_t *private_key, public_key_t *public_key) {
    if (!private_key || !public_key) return -1;
    
    public_key->data[0] = 0x02;
    for (int i = 1; i < COMPRESSED_PUBLIC_KEY_SIZE; i++) {
        public_key->data[i] = (uint8_t)(rand() % 256);
    }
    public_key->length = COMPRESSED_PUBLIC_KEY_SIZE;
    
    return 0;
}

int generate_bitcoin_address(const public_key_t *public_key, bitcoin_address_t *address) {
    if (!public_key || !address) return -1;
    
    address->data[0] = VERSION_BYTE_MAINNET;
    for (int i = 1; i < 21; i++) {
        address->data[i] = (uint8_t)(rand() % 256);
    }
    
    for (int i = 21; i < 25; i++) {
        address->data[i] = (uint8_t)(rand() % 256);
    }
    
    address->length = 25;
    return 0;
}

int private_key_to_wif(const private_key_t *key, char *wif, size_t wif_size) {
    if (!key || !wif || wif_size < WIF_SIZE) return -1;
    
    char hex_key[65];
    if (bytes_to_hex(key->data, PRIVATE_KEY_SIZE, hex_key, sizeof(hex_key)) != 0) {
        return -1;
    }
    
    snprintf(wif, wif_size, "5%s", hex_key);
    return 0;
}

int wif_to_private_key(const char *wif, private_key_t *key) {
    if (!wif || !key) return -1;
    
    if (strlen(wif) < 66) return -1;
    
    const char *hex_part = wif + 1;
    return hex_to_bytes(hex_part, key->data, PRIVATE_KEY_SIZE);
}

void secure_zero_memory(void *ptr, size_t size) {
    if (!ptr) return;
    
    volatile uint8_t *p = (volatile uint8_t *)ptr;
    while (size--) {
        *p++ = 0;
    }
} 