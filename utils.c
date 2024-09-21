#include <stdlib.h>
#include <time.h>
#include "utils.h"

//
int get_random_number(int limit) {
    return rand() % limit;
}
