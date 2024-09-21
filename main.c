#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "keygen.h"

int main() {
    // this will set random seed
    srand(time(NULL));

    // determine the length of the private this will generate
    int key_length = 64;
        char priv_key[65];  // give an extra space for the null terminator 

}