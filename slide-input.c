#include "slide-input.h"
#include <stdio.h>

#define INPUT_LEN 64

#define UPPER_MASK 0xDF

struct action get_input() {
    struct action in;
    in.value = 0;

    char action = '-';
    int num = 1;

    char buf[INPUT_LEN];
    fgets(buf, INPUT_LEN, stdin);
    sscanf(buf, "%c %d", &action, &num);

    action &= UPPER_MASK;

    switch (action) {
        case NOOP:
            in.action = NOOP;
            break;
        case BACK:
            in.action = BACK;
            break;
        case NEXT:
            in.action = NEXT;
            break;
        case QUIT:
            in.action = QUIT;
            break;
        case JUMP:
            in.action = JUMP;
            break;
        default:
            in.action = NEXT;
            num = 1;
            break;
    }
    in.value = num;

    return in;
}
