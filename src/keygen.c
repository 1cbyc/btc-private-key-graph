#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "keygen.h"
#include "utils.h"

// my intent is to write the logic for the priate key generation here:
void generate_priv_key(char *output, int length) {
    const char validChars[] = "QWERTYUIOPASDFGHJKLZXCVBNM1234567890qwertyuiopasdfghjklzxcvbnm";
    int validCharsLen = sizeof(validChars) - 1;

    for (int i = 0; i < length; i++) {
        int randomIndex = get_random_number(validCharsLen);
        output[i] = validChars[randomIndex];
    }

    output[length] = '\0'; // i get to null-terminate the string
}