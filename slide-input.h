#ifndef SLIDE_INPUT_H
#define SLIDE_INPUT_H

#define NOOP '-'
#define NEXT 'N'
#define BACK 'B'
#define QUIT 'Q'

struct action {
    int action;
    int value;
};

struct action get_input();

#endif
