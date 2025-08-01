#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    while (1) {
        system("git add .");

        system("git commit -m 'refactoring from c# to c'");

        system("git push");

        sleep(8);
    }

    return 0;
}
