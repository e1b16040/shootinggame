#ifndef INCLUDED_Window_h_
#define INCLUDED_Window_h_

#define _BSD_SOURCE || (_XOPEN_SOURCE) >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED) && !(_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)

#define TERMINAL_HEIGHT 60
#define TERMINAL_WIDTH 160
#define FIELD_START_X 0
#define FIELD_START_Y 0
#define FIELD_WIDTH 120
#define FIELD_HEIGHT TERMINAL_HEIGHT

#ifndef DEBUG

#define INFO_START_X FIELD_START_X + FIELD_WIDTH
#define INFO_START_Y FIELD_START_Y
#define INFO_WIDTH TERMINAL_WIDTH - FIELD_WIDTH
#define INFO_HEIGHT FIELD_HEIGHT

#else

#define INFO_START_X FIELD_START_X + FIELD_WIDTH
#define INFO_START_Y FIELD_START_Y
#define INFO_WIDTH TERMINAL_WIDTH - FIELD_WIDTH
#define INFO_HEIGHT 20

#define DEBUG_START_X INFO_START_X
#define DEBUG_START_Y INFO_START_Y + INFO_HEIGHT
#define DEBUG_WIDTH INFO_WIDTH
#define DEBUG_HEIGHT TERMINAL_HEIGHT - DEBUG_START_Y

WINDOW *debug;

#endif /* DEBUG */

int initWindows();
int contains(const int, const int);
void clearWindows();
void refreshWindows();

#ifdef DEBUG
extern WINDOW *debug;
#endif /* DEBUG */

extern WINDOW *field;
extern WINDOW *info;

#endif /*INCLUDED_Window_h_ */
