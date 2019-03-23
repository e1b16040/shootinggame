#ifndef INCLUDED_FPS_h_
#define INCLUDED_FPS_h_

#include <sys/time.h>
#include <time.h>

#define FRAMERATE 30  /* フレームレート */

void updateFrame();
void wait();

extern int actFPS;
extern struct timespec waitNTime;


#endif /* INCLUDED_FPS_h_*/
