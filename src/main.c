#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "keygen.h"
#include "crypto.h"
#include "utils.h"

static volatile int running = 1;

void signal_handler(int sig) {
    running = 0;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    if (crypto_init() != 0) {
        fprintf(stderr, "Failed to initialize cryptographic system\n");
        return 1;
    }
    
    keygen_options_t options;
    if (parse_command_line_args(argc, argv, &options) != 0) {
        crypto_cleanup();
        return 1;
    }
    
    if (options.count <= 0) {
        fprintf(stderr, "Invalid count: must be greater than 0\n");
        crypto_cleanup();
        return 1;
    }
    
    if (options.count == 1) {
        private_key_t private_key;
        public_key_t public_key;
        bitcoin_address_t address;
        
        if (generate_bitcoin_key_pair(&private_key, &public_key, &options) != 0) {
            if (!options.quiet) {
                fprintf(stderr, "Failed to generate key pair\n");
            }
            crypto_cleanup();
            return 1;
        }
        
        if (options.with_address) {
            if (create_p2pkh_address(&public_key, &address) != 0) {
                if (!options.quiet) {
                    fprintf(stderr, "Failed to generate address\n");
                }
                secure_zero_memory(&private_key, sizeof(private_key));
                secure_zero_memory(&public_key, sizeof(public_key));
                crypto_cleanup();
                return 1;
            }
        }
        
        if (print_key_information(&private_key, &public_key, 
                                options.with_address ? &address : NULL, &options) != 0) {
            if (!options.quiet) {
                fprintf(stderr, "Failed to print key information\n");
            }
            secure_zero_memory(&private_key, sizeof(private_key));
            secure_zero_memory(&public_key, sizeof(public_key));
            if (options.with_address) {
                secure_zero_memory(&address, sizeof(address));
            }
            crypto_cleanup();
            return 1;
        }
        
        secure_zero_memory(&private_key, sizeof(private_key));
        secure_zero_memory(&public_key, sizeof(public_key));
        if (options.with_address) {
            secure_zero_memory(&address, sizeof(address));
        }
    } else {
        if (generate_multiple_keys(options.count, &options) != 0) {
            if (!options.quiet) {
                fprintf(stderr, "Failed to generate multiple keys\n");
            }
            crypto_cleanup();
            return 1;
        }
    }
    
    crypto_cleanup();
    return 0;
}