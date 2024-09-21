#include <stdlib.h>
#include <time.h>
#include "utils.h"

// this way, we get a ra
int get_random_number(int limit) {
    return rand() % limit;
}
