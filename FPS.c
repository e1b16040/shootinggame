#include <unistd.h>
#include <ncurses.h>
#include "FPS.h"

#define FRAME 1e9 / FRAMERATE
#define AVE_PRECISION FRAMERATE

static int calcFPS(struct timespec * const, const struct timespec * const);
static void copyTimespec(struct timespec * const, const struct timespec * const);
static void subtractTimespec(struct timespec * const, const struct timespec * const);


static struct timespec startNTime;
static int count;
int actFPS;
struct timespec waitNTime;

void updateFrame() {
    struct timespec tmpNTime;
    if (count == 0) {
        clock_gettime(CLOCK_MONOTONIC, &startNTime);
    }
    if (count == AVE_PRECISION) {
        clock_gettime(CLOCK_MONOTONIC, &tmpNTime);
        actFPS = calcFPS(&tmpNTime, &startNTime);
        count = 0;
        copyTimespec(&startNTime, &tmpNTime);
    }
    count++;
}

void wait() {
    struct timespec passedNTime;
    clock_gettime(CLOCK_MONOTONIC, &passedNTime);
    subtractTimespec(&passedNTime, &startNTime);

    waitNTime.tv_sec = (count * FRAME) / 1e10;
    waitNTime.tv_nsec = (int)(count * FRAME) % (int)1e10;

    subtractTimespec(&waitNTime, &passedNTime);

/*
    printw("fps:%d\n", actfps);
    printw("startNTime\t:%10ds:%10ld\n", startNTime.tv_sec, startNTime.tv_nsec);
    printw("passedNTime\t:%10ds:%10ld\n", passedNTime.tv_sec, passedNTime.tv_nsec);
    printw("waitNTime\t:%10ds:%10ld\n", waitNTime.tv_sec, waitNTime.tv_nsec);
*/

    if (waitNTime.tv_nsec > 0) {
        nanosleep(&waitNTime, NULL);
    }
}

static int calcFPS(struct timespec * const b, const struct timespec * const a) {
    int fps;
    struct timespec elapsedNTime;
    copyTimespec(&elapsedNTime, b);
    subtractTimespec(&elapsedNTime, a);

    /* startからの経過時間をマイクロ秒単位で計算し，int型で代入 */
    fps = (elapsedNTime.tv_sec % 10) * 1e7 + (int)(elapsedNTime.tv_nsec / 1e3);
    fps = 1e7 / (fps / AVE_PRECISION);

    return fps;
}

static void copyTimespec(struct timespec * const b, const struct timespec * const a) {
    b->tv_sec = a->tv_sec;
    b->tv_nsec = a->tv_nsec;
}

/* b = b - a */
static void subtractTimespec(struct timespec * const b, const struct timespec * const a) {
    if ((b->tv_sec < a->tv_sec) || ((b->tv_sec == a->tv_sec) && (b->tv_nsec <= a->tv_nsec))) {
        b->tv_sec = b->tv_nsec = 0;
    } else {
        b->tv_sec -= a->tv_sec;
        if (b->tv_nsec < a->tv_nsec) {
            b->tv_nsec += 1e10 - a->tv_nsec;
            b->tv_sec--;
        } else {
            b->tv_nsec -= a->tv_nsec;
        }
    }
    return;
}
