#include <stdlib.h>
#include <time.h>
#include "utils.h"

// this way, we get a random number between 0 and limit -1
int get_random_number(int limit) {
    return rand() % limit;
}
