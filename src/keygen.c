#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "keygen.h"
#include "crypto.h"
#include "address.h"
#include "utils.h"

#define VERSION "2.0.0"

int generate_bitcoin_key_pair(private_key_t *private_key, public_key_t *public_key, const keygen_options_t *options) {
    if (!private_key || !public_key || !options) return -1;
    
    if (generate_secure_private_key(private_key) != 0) {
        return -1;
    }
    
    if (validate_private_key(private_key) != 0) {
        return -1;
    }
    
    if (options->compressed) {
        if (derive_compressed_public_key(private_key, public_key) != 0) {
            return -1;
        }
    } else {
        if (derive_public_key(private_key, public_key) != 0) {
            return -1;
        }
    }
    
    return 0;
}

int generate_multiple_keys(int count, const keygen_options_t *options) {
    if (count <= 0 || !options) return -1;
    
    for (int i = 0; i < count; i++) {
        private_key_t private_key;
        public_key_t public_key;
        bitcoin_address_t address;
        
        if (generate_bitcoin_key_pair(&private_key, &public_key, options) != 0) {
            if (!options->quiet) {
                fprintf(stderr, "Failed to generate key pair %d\n", i + 1);
            }
            continue;
        }
        
        if (options->with_address) {
            if (create_p2pkh_address(&public_key, &address) != 0) {
                if (!options->quiet) {
                    fprintf(stderr, "Failed to generate address for key pair %d\n", i + 1);
                }
                continue;
            }
        }
        
        if (print_key_information(&private_key, &public_key, 
                                options->with_address ? &address : NULL, options) != 0) {
            if (!options->quiet) {
                fprintf(stderr, "Failed to print key information for key pair %d\n", i + 1);
            }
        }
        
        secure_zero_memory(&private_key, sizeof(private_key));
        secure_zero_memory(&public_key, sizeof(public_key));
        if (options->with_address) {
            secure_zero_memory(&address, sizeof(address));
        }
    }
    
    return 0;
}

int print_key_information(const private_key_t *private_key, const public_key_t *public_key, 
                         const bitcoin_address_t *address, const keygen_options_t *options) {
    if (!private_key || !public_key || !options) return -1;
    
    char hex_private_key[MAX_HEX_STRING_SIZE];
    char wif_private_key[MAX_WIF_STRING_SIZE];
    char hex_public_key[MAX_HEX_STRING_SIZE];
    char address_string[MAX_ADDRESS_STRING_SIZE];
    
    if (bytes_to_hex(private_key->data, PRIVATE_KEY_SIZE, hex_private_key, sizeof(hex_private_key)) != 0) {
        fprintf(stderr, "Failed to convert private key to hex\n");
        return -1;
    }
    
    if (private_key_to_wif(private_key, wif_private_key, sizeof(wif_private_key)) != 0) {
        fprintf(stderr, "Failed to convert private key to WIF\n");
        return -1;
    }
    
    fprintf(stderr, "Debug: public_key->length = %zu\n", public_key->length);
    size_t pub_key_len = public_key->length;
    if (pub_key_len == 0) {
        fprintf(stderr, "Public key length is 0, setting to default\n");
        pub_key_len = PUBLIC_KEY_SIZE;
    }
    
    if (bytes_to_hex(public_key->data, pub_key_len, hex_public_key, sizeof(hex_public_key)) != 0) {
        fprintf(stderr, "Failed to convert public key to hex\n");
        return -1;
    }
    
    if (address && base58_encode(address->data, address->length, address_string, sizeof(address_string)) != 0) {
        fprintf(stderr, "Failed to encode address\n");
        return -1;
    }
    
    if (options->verbose) {
        printf("Private Key (Hex): %s\n", hex_private_key);
        printf("Private Key (WIF): %s\n", wif_private_key);
        printf("Public Key (Hex): %s\n", hex_public_key);
        if (address) {
            printf("Bitcoin Address: %s\n", address_string);
        }
        printf("---\n");
    } else {
        switch (options->format) {
            case OUTPUT_FORMAT_HEX:
                printf("%s", hex_private_key);
                break;
            case OUTPUT_FORMAT_WIF:
                printf("%s", wif_private_key);
                break;
            case OUTPUT_FORMAT_BINARY:
                print_hex(private_key->data, PRIVATE_KEY_SIZE);
                break;
        }
        
        if (options->with_address && address) {
            printf(" %s", address_string);
        }
        printf("\n");
    }
    
    return 0;
}

int parse_command_line_args(int argc, char *argv[], keygen_options_t *options) {
    if (!options) return -1;
    
    memset(options, 0, sizeof(keygen_options_t));
    options->count = 1;
    options->format = OUTPUT_FORMAT_HEX;
    
    static struct option long_options[] = {
        {"count", required_argument, 0, 'c'},
        {"format", required_argument, 0, 'f'},
        {"with-address", no_argument, 0, 'a'},
        {"compressed", no_argument, 0, 'p'},
        {"testnet", no_argument, 0, 't'},
        {"verbose", no_argument, 0, 'v'},
        {"quiet", no_argument, 0, 'q'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, "c:f:aptvqhV", long_options, NULL)) != -1) {
        switch (opt) {
            case 'c':
                options->count = atoi(optarg);
                if (options->count <= 0) {
                    fprintf(stderr, "Invalid count: %s\n", optarg);
                    return -1;
                }
                break;
            case 'f':
                if (string_equals(optarg, "hex")) {
                    options->format = OUTPUT_FORMAT_HEX;
                } else if (string_equals(optarg, "wif")) {
                    options->format = OUTPUT_FORMAT_WIF;
                } else if (string_equals(optarg, "binary")) {
                    options->format = OUTPUT_FORMAT_BINARY;
                } else {
                    fprintf(stderr, "Invalid format: %s\n", optarg);
                    return -1;
                }
                break;
            case 'a':
                options->with_address = 1;
                break;
            case 'p':
                options->compressed = 1;
                break;
            case 't':
                options->testnet = 1;
                break;
            case 'v':
                options->verbose = 1;
                break;
            case 'q':
                options->quiet = 1;
                break;
            case 'h':
                print_usage(argv[0]);
                exit(0);
            case 'V':
                print_version();
                exit(0);
            default:
                print_usage(argv[0]);
                return -1;
        }
    }
    
    return 0;
}

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("Generate Bitcoin private keys and addresses\n\n");
    printf("Options:\n");
    printf("  -c, --count NUM        Generate NUM keys (default: 1)\n");
    printf("  -f, --format FORMAT    Output format: hex, wif, binary (default: hex)\n");
    printf("  -a, --with-address     Include Bitcoin address in output\n");
    printf("  -p, --compressed       Use compressed public key format\n");
    printf("  -t, --testnet          Generate testnet addresses\n");
    printf("  -v, --verbose          Verbose output\n");
    printf("  -q, --quiet            Suppress error messages\n");
    printf("  -h, --help             Show this help message\n");
    printf("  -V, --version          Show version information\n\n");
    printf("Examples:\n");
    printf("  %s                      Generate one key in hex format\n", program_name);
    printf("  %s -c 10               Generate 10 keys\n", program_name);
    printf("  %s -f wif -a           Generate WIF format with address\n", program_name);
    printf("  %s -v -p               Verbose output with compressed key\n", program_name);
}

void print_version(void) {
    printf("Bitcoin Key Generator v%s\n", VERSION);
    printf("Cryptographically secure Bitcoin private key generation\n");
}