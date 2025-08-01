#ifndef KEYGEN_H
#define KEYGEN_H

#include <stdint.h>
#include <stddef.h>
#include "crypto.h"

typedef enum {
    OUTPUT_FORMAT_HEX,
    OUTPUT_FORMAT_WIF,
    OUTPUT_FORMAT_BINARY
} output_format_t;

typedef struct {
    int count;
    output_format_t format;
    int with_address;
    int compressed;
    int testnet;
    int verbose;
    int quiet;
} keygen_options_t;

int generate_bitcoin_key_pair(private_key_t *private_key, public_key_t *public_key, const keygen_options_t *options);
int generate_multiple_keys(int count, const keygen_options_t *options);
int print_key_information(const private_key_t *private_key, const public_key_t *public_key, const bitcoin_address_t *address, const keygen_options_t *options);
int parse_command_line_args(int argc, char *argv[], keygen_options_t *options);
void print_usage(const char *program_name);
void print_version(void);

#endif
