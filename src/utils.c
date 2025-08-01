#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

// this way, we get a random number between 0 and limit -1
int get_random_number(int limit) {
    return rand() % limit;
}

int hex_to_bytes(const char *hex, uint8_t *bytes, size_t bytes_size) {
    if (!hex || !bytes || bytes_size == 0) return -1;
    
    size_t hex_len = strlen(hex);
    if (hex_len % 2 != 0) return -1;
    
    size_t bytes_len = hex_len / 2;
    if (bytes_len > bytes_size) return -1;
    
    for (size_t i = 0; i < hex_len; i += 2) {
        char hex_byte[3] = {hex[i], hex[i + 1], '\0'};
        char *endptr;
        unsigned long value = strtoul(hex_byte, &endptr, 16);
        
        if (*endptr != '\0') return -1;
        
        bytes[i / 2] = (uint8_t)value;
    }
    
    return 0;
}

int bytes_to_hex(const uint8_t *bytes, size_t bytes_size, char *hex, size_t hex_size) {
    if (!bytes || !hex || hex_size == 0) return -1;
    
    if (hex_size < bytes_size * 2 + 1) return -1;
    
    for (size_t i = 0; i < bytes_size; i++) {
        snprintf(hex + i * 2, 3, "%02x", bytes[i]);
    }
    
    hex[bytes_size * 2] = '\0';
    return 0;
}

int validate_hex_string(const char *hex) {
    if (!hex) return -1;
    
    size_t len = strlen(hex);
    if (len == 0 || len % 2 != 0) return -1;
    
    for (size_t i = 0; i < len; i++) {
        if (!isxdigit((unsigned char)hex[i])) return -1;
    }
    
    return 0;
}

int validate_wif_string(const char *wif) {
    if (!wif) return -1;
    
    size_t len = strlen(wif);
    if (len < 26 || len > 35) return -1;
    
    for (size_t i = 0; i < len; i++) {
        if (!strchr(BASE58_ALPHABET, wif[i])) return -1;
    }
    
    return 0;
}

int reverse_bytes(uint8_t *data, size_t size) {
    if (!data || size == 0) return -1;
    
    for (size_t i = 0; i < size / 2; i++) {
        uint8_t temp = data[i];
        data[i] = data[size - 1 - i];
        data[size - 1 - i] = temp;
    }
    
    return 0;
}

int compare_bytes(const uint8_t *a, const uint8_t *b, size_t size) {
    if (!a || !b) return -1;
    
    return memcmp(a, b, size);
}

bool is_zero_bytes(const uint8_t *data, size_t size) {
    if (!data) return false;
    
    for (size_t i = 0; i < size; i++) {
        if (data[i] != 0) return false;
    }
    
    return true;
}

int safe_memcpy(void *dest, size_t dest_size, const void *src, size_t src_size) {
    if (!dest || !src) return -1;
    
    if (src_size > dest_size) return -1;
    
    memcpy(dest, src, src_size);
    return 0;
}

int safe_memset(void *dest, int value, size_t size) {
    if (!dest) return -1;
    
    memset(dest, value, size);
    return 0;
}

void print_hex(const uint8_t *data, size_t size) {
    if (!data || size == 0) return;
    
    for (size_t i = 0; i < size; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

void print_binary(const uint8_t *data, size_t size) {
    if (!data || size == 0) return;
    
    for (size_t i = 0; i < size; i++) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (data[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

int string_to_lower(char *str) {
    if (!str) return -1;
    
    for (size_t i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
    
    return 0;
}

int string_to_upper(char *str) {
    if (!str) return -1;
    
    for (size_t i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
    
    return 0;
}

int trim_string(char *str) {
    if (!str) return -1;
    
    size_t len = strlen(str);
    if (len == 0) return 0;
    
    size_t start = 0;
    while (start < len && isspace((unsigned char)str[start])) {
        start++;
    }
    
    size_t end = len;
    while (end > start && isspace((unsigned char)str[end - 1])) {
        end--;
    }
    
    if (start > 0) {
        memmove(str, str + start, end - start);
    }
    
    str[end - start] = '\0';
    
    return 0;
}

bool string_equals(const char *a, const char *b) {
    if (!a || !b) return false;
    return strcmp(a, b) == 0;
}

bool string_starts_with(const char *str, const char *prefix) {
    if (!str || !prefix) return false;
    
    size_t str_len = strlen(str);
    size_t prefix_len = strlen(prefix);
    
    if (prefix_len > str_len) return false;
    
    return strncmp(str, prefix, prefix_len) == 0;
}

bool string_ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) return false;
    
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) return false;
    
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}
