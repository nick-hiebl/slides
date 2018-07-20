#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include "slide-input.h"

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
    if (starts_with(s, "###")) {
        printf(BOLD "%s" KNRM, s + after(s, "###"));
    } else if (starts_with(s, "##")) {
        printf(BOLD "%s" KNRM, s + after(s, "##"));
    } else if (starts_with(s, "#")) {
        printf(BOLD "%s" KNRM, s + after(s, "#"));
    } else {
        printf("%s", s);
    }
}

void print_slide(struct slide *slide, struct winsize w) {
    printf("%dx%d\n", slide->width, slide->lines);
    int j = 0;
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
    while (j < w.ws_row - 2) {
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


    struct action action;
    action.action = NOOP;
    while (action.action != QUIT) {
        if (!next_slide(f, &slide)) break;
        print_slide(&slide, w);

        action = get_input();

    }


    return 0;
}
