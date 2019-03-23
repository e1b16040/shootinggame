#include <ncurses.h>
#include <stdlib.h>
#include "Window.h"


WINDOW *field;
WINDOW *info;

int initWindows() {
    int scrHeight;
    int scrWidth;
    char cmd[20];

    sprintf(cmd, "resize -s %d %d", TERMINAL_HEIGHT, TERMINAL_WIDTH);


    system(cmd);

    initscr();
    getmaxyx(stdscr, scrHeight, scrWidth);
    if (scrHeight < TERMINAL_HEIGHT || scrWidth < TERMINAL_WIDTH) {
        fprintf(stderr, "画面サイズを縦60文字,横120文字以上にしてください\n");
        return -1;
    }

    field = newwin(FIELD_HEIGHT, FIELD_WIDTH, FIELD_START_Y, FIELD_START_X);
    info = newwin(INFO_HEIGHT, INFO_WIDTH, INFO_START_Y, INFO_START_X);

#ifdef DEBUG
    debug = newwin(DEBUG_HEIGHT, DEBUG_WIDTH, DEBUG_START_Y, DEBUG_START_X);
#endif

    return 0;
}

int contains(const int x, const int y) {
    return (FIELD_START_X +1 <= x && x < FIELD_WIDTH -1 && FIELD_START_Y +1 <= y && y < FIELD_HEIGHT
    -1);
}

void clearWindows() {
    wclear(field);
    wclear(info);
    box(field, 0, 0);
    box(info, 0, 0);
}

void refreshWindows() {
    wnoutrefresh(field);
    wnoutrefresh(info);
    doupdate();
}