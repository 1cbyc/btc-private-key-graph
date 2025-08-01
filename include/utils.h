#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_HEX_STRING_SIZE 131
#define MAX_WIF_STRING_SIZE 52
#define MAX_ADDRESS_STRING_SIZE 35

int hex_to_bytes(const char *hex, uint8_t *bytes, size_t bytes_size);
int bytes_to_hex(const uint8_t *bytes, size_t bytes_size, char *hex, size_t hex_size);
int validate_hex_string(const char *hex);
int validate_wif_string(const char *wif);
int reverse_bytes(uint8_t *data, size_t size);
int compare_bytes(const uint8_t *a, const uint8_t *b, size_t size);
bool is_zero_bytes(const uint8_t *data, size_t size);
int safe_memcpy(void *dest, size_t dest_size, const void *src, size_t src_size);
int safe_memset(void *dest, int value, size_t size);
void print_hex(const uint8_t *data, size_t size);
void print_binary(const uint8_t *data, size_t size);
int string_to_lower(char *str);
int string_to_upper(char *str);
int trim_string(char *str);
bool string_equals(const char *a, const char *b);
bool string_starts_with(const char *str, const char *prefix);
bool string_ends_with(const char *str, const char *suffix);

#endif
