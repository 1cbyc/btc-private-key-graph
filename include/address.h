#ifndef ADDRESS_H
#define ADDRESS_H

#include <stdint.h>
#include <stddef.h>
#include "crypto.h"

#define BASE58_ALPHABET "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz"
#define BASE58_ALPHABET_SIZE 58
#define CHECKSUM_SIZE 4
#define VERSION_BYTE_MAINNET 0x00
#define VERSION_BYTE_TESTNET 0x6F

typedef enum {
    ADDRESS_TYPE_P2PKH,
    ADDRESS_TYPE_P2SH,
    ADDRESS_TYPE_P2WPKH
} address_type_t;

typedef struct {
    uint8_t version;
    uint8_t payload[20];
    uint8_t checksum[CHECKSUM_SIZE];
} address_data_t;

int base58_encode(const uint8_t *data, size_t data_len, char *output, size_t output_size);
int base58_decode(const char *input, uint8_t *output, size_t output_size);
int calculate_checksum(const uint8_t *data, size_t data_len, uint8_t *checksum);
int verify_checksum(const uint8_t *data, size_t data_len, const uint8_t *checksum);
int create_p2pkh_address(const public_key_t *public_key, bitcoin_address_t *address);
int create_p2sh_address(const public_key_t *public_key, bitcoin_address_t *address);
int validate_bitcoin_address(const char *address);
int address_to_bytes(const char *address, address_data_t *data);

#endif 