#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include "slide-input.h"

// http://ascii-table.com/ansi-escape-sequences.php

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define BOLD  "\x1B[1m"

#define MAX_LINE_LENGTH 128
#define MAX_LINES 64

#define FALSE 0
#define TRUE !FALSE

struct slide {
    int width;
    int lines;
    char content[MAX_LINES][MAX_LINE_LENGTH];
};

int starts_with(char *s1, char *s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return FALSE;
        }
        i++;
    }
    return TRUE;
}

int after(char *s, char *prefix) {
    int i;
    while (prefix[i] != '\0' && s[i] != '\0') {
        i++;
    }
    while (s[i] != '\0' && (s[i] == ' ' || s[i] == '\t')) {
        i++;
    }
    return i;
}

int next_slide(FILE *fin, struct slide *slide) {
    int n_read = 0, width = 0;
    while (n_read < MAX_LINES &&
            fgets(slide->content[n_read], MAX_LINE_LENGTH, fin)) {
        if (starts_with(slide->content[n_read], "!!")) {
            break;
        }
        int len = strlen(slide->content[n_read]);
        if (len > width) {
            width = len;
        }
        n_read++;
    }
    slide->width = width;
    slide->lines = n_read;
    if (n_read == 0) {
        return FALSE;
    } else {
        return TRUE;
    }
}

void print_line(char *s) {
    if (starts_with(s, "#red")) {
        printf(KRED "%s" KNRM, s + after(s, "#red"));
    } else if (starts_with(s, "#green")) {
        printf(KGRN "%s" KNRM, s + after(s, "#green"));
    } else if (starts_with(s, "#yellow")) {
        printf(KYEL "%s" KNRM, s + after(s, "#yellow"));
    } else if (starts_with(s, "#blue")) {
        printf(KBLU "%s" KNRM, s + after(s, "#blue"));
    } else if (starts_with(s, "#magenta")) {
        printf(KMAG "%s" KNRM, s + after(s, "#magenta"));
    } else if (starts_with(s, "#cyan")) {
        printf(KCYN "%s" KNRM, s + after(s, "#cyan"));
    } else if (starts_with(s, "#white")) {
        printf(KWHT "%s" KNRM, s + after(s, "#white"));
    } else if (starts_with(s, "###")) {
        printf(BOLD "%s" KNRM, s + after(s, "###"));
    } else if (starts_with(s, "##")) {
        printf(BOLD "%s" KNRM, s + after(s, "##"));
    } else if (starts_with(s, "#")) {
        printf(BOLD "%s" KNRM, s + after(s, "#"));
    } else {
        printf("%s", s);
    }
}

void print_slide(struct slide *slide, struct winsize w, int number) {
    printf("Slide %d | %dx%d\n", number, slide->width, slide->lines);
    int j = 1;
    int pre_space = (w.ws_row - slide->lines) / 2 - 2;
    while (j < pre_space) {
        putchar('\n');
        j++;
    }
    int i;
    int left = (w.ws_col - slide->width) / 2 - 3;
    for (i = 0; i < slide->lines; i++) {
        int k;
        for (k = 0; k < left; k++) {
            putchar(' ');
        }
        print_line(slide->content[i]);
        j++;
    }
    while (j < w.ws_row - 1) {
        putchar('\n');
        j++;
    }
}

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Correct usage is: %s <filename>\n", argv[0]);
        exit(1);
    }

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    struct slide slide;
    FILE *f = fopen(argv[1], "r");

    int slide_number = 0;

    struct action action;
    action.action = NOOP;

    int slide_exists = next_slide(f, &slide);
    slide_number++;

    while (slide_exists && action.action != QUIT) {
        print_slide(&slide, w, slide_number);

        action = get_input();
        int k;
        switch (action.action) {
            case NEXT:
                k = action.value + slide_number;
                for (; slide_exists && slide_number < k;) {
                    slide_exists = next_slide(f, &slide);
                    slide_number++;
                }
                break;
            case BACK:
                action.action = JUMP;
                action.value = slide_number - action.value;
            case JUMP:
                if (action.value < slide_number) {
                    fclose(f);
                    f = fopen(argv[1], "r");
                    slide_number = 0;
                }
                for (;slide_exists && slide_number < action.value;) {
                    slide_exists = next_slide(f, &slide);
                    slide_number++;
                }
        }
    }

    fclose(f);

    return 0;
}
