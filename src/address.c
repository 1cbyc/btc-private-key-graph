#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include "address.h"
#include "utils.h"

static const char *base58_chars = BASE58_ALPHABET;

int base58_encode(const uint8_t *data, size_t data_len, char *output, size_t output_size) {
    if (!data || !output || output_size == 0) return -1;
    
    uint8_t *temp = malloc(data_len * 2);
    if (!temp) return -1;
    
    memcpy(temp, data, data_len);
    size_t temp_len = data_len;
    
    char *out_ptr = output;
    size_t out_len = 0;
    
    while (temp_len > 0) {
        int remainder = 0;
        for (size_t i = 0; i < temp_len; i++) {
            int digit = temp[i] + remainder * 256;
            temp[i] = digit / 58;
            remainder = digit % 58;
        }
        
        if (out_len >= output_size - 1) {
            free(temp);
            return -1;
        }
        
        *out_ptr++ = base58_chars[remainder];
        out_len++;
        
        while (temp_len > 0 && temp[0] == 0) {
            temp++;
            temp_len--;
        }
    }
    
    for (size_t i = 0; i < data_len && data[i] == 0; i++) {
        if (out_len >= output_size - 1) {
            free(temp);
            return -1;
        }
        *out_ptr++ = base58_chars[0];
        out_len++;
    }
    
    output[out_len] = '\0';
    
    for (size_t i = 0; i < out_len / 2; i++) {
        char temp = output[i];
        output[i] = output[out_len - 1 - i];
        output[out_len - 1 - i] = temp;
    }
    
    free(temp);
    return 0;
}

int base58_decode(const char *input, uint8_t *output, size_t output_size) {
    if (!input || !output || output_size == 0) return -1;
    
    size_t input_len = strlen(input);
    if (input_len == 0) return -1;
    
    uint8_t *temp = malloc(input_len);
    if (!temp) return -1;
    
    size_t temp_len = 0;
    
    for (size_t i = 0; i < input_len; i++) {
        const char *pos = strchr(base58_chars, input[i]);
        if (!pos) {
            free(temp);
            return -1;
        }
        temp[temp_len++] = pos - base58_chars;
    }
    
    uint8_t *out_ptr = output;
    size_t out_len = 0;
    
    while (temp_len > 0) {
        int remainder = 0;
        for (size_t i = 0; i < temp_len; i++) {
            int digit = temp[i] + remainder * 58;
            temp[i] = digit / 256;
            remainder = digit % 256;
        }
        
        if (out_len >= output_size) {
            free(temp);
            return -1;
        }
        
        *out_ptr++ = remainder;
        out_len++;
        
        while (temp_len > 0 && temp[0] == 0) {
            temp++;
            temp_len--;
        }
    }
    
    for (size_t i = 0; i < input_len && input[i] == base58_chars[0]; i++) {
        if (out_len >= output_size) {
            free(temp);
            return -1;
        }
        *out_ptr++ = 0;
        out_len++;
    }
    
    for (size_t i = 0; i < out_len / 2; i++) {
        uint8_t temp_byte = output[i];
        output[i] = output[out_len - 1 - i];
        output[out_len - 1 - i] = temp_byte;
    }
    
    free(temp);
    return 0;
}

int calculate_checksum(const uint8_t *data, size_t data_len, uint8_t *checksum) {
    if (!data || !checksum) return -1;
    
    uint8_t hash[32];
    SHA256(SHA256(data, data_len, NULL), 32, hash);
    
    memcpy(checksum, hash, CHECKSUM_SIZE);
    return 0;
}

int verify_checksum(const uint8_t *data, size_t data_len, const uint8_t *checksum) {
    if (!data || !checksum) return -1;
    
    uint8_t calculated_checksum[CHECKSUM_SIZE];
    if (calculate_checksum(data, data_len, calculated_checksum) != 0) {
        return -1;
    }
    
    return memcmp(calculated_checksum, checksum, CHECKSUM_SIZE) == 0 ? 0 : -1;
}

int create_p2pkh_address(const public_key_t *public_key, bitcoin_address_t *address) {
    if (!public_key || !address) return -1;
    
    uint8_t sha256_hash[32];
    uint8_t ripemd160_hash[20];
    
    SHA256(public_key->data, public_key->length, sha256_hash);
    RIPEMD160(sha256_hash, 32, ripemd160_hash);
    
    address->data[0] = VERSION_BYTE_MAINNET;
    memcpy(address->data + 1, ripemd160_hash, 20);
    
    uint8_t checksum[CHECKSUM_SIZE];
    if (calculate_checksum(address->data, 21, checksum) != 0) {
        return -1;
    }
    
    memcpy(address->data + 21, checksum, CHECKSUM_SIZE);
    address->length = 25;
    
    return 0;
}

int create_p2sh_address(const public_key_t *public_key, bitcoin_address_t *address) {
    if (!public_key || !address) return -1;
    
    uint8_t sha256_hash[32];
    uint8_t ripemd160_hash[20];
    
    SHA256(public_key->data, public_key->length, sha256_hash);
    RIPEMD160(sha256_hash, 32, ripemd160_hash);
    
    address->data[0] = 0x05;
    memcpy(address->data + 1, ripemd160_hash, 20);
    
    uint8_t checksum[CHECKSUM_SIZE];
    if (calculate_checksum(address->data, 21, checksum) != 0) {
        return -1;
    }
    
    memcpy(address->data + 21, checksum, CHECKSUM_SIZE);
    address->length = 25;
    
    return 0;
}

int validate_bitcoin_address(const char *address) {
    if (!address) return -1;
    
    uint8_t decoded[25];
    if (base58_decode(address, decoded, sizeof(decoded)) != 0) {
        return -1;
    }
    
    if (decoded[0] != VERSION_BYTE_MAINNET && decoded[0] != 0x05) {
        return -1;
    }
    
    return verify_checksum(decoded, 21, decoded + 21);
}

int address_to_bytes(const char *address, address_data_t *data) {
    if (!address || !data) return -1;
    
    uint8_t decoded[25];
    if (base58_decode(address, decoded, sizeof(decoded)) != 0) {
        return -1;
    }
    
    data->version = decoded[0];
    memcpy(data->payload, decoded + 1, 20);
    memcpy(data->checksum, decoded + 21, CHECKSUM_SIZE);
    
    return 0;
} 