#ifndef INCLUDED_Player_h_
#define INCLUDED_Player_h_

#include "Bullet.h"

#define GAME_OVER 0
#define GAME_CONTINUE 1

void initPlayer();
void drawPlayer();
void updatePlayer(const int, const int);
int getPlayerRemaining();
int getPlayerX();
int getPlayerY();
int collisionDetectionPlayer(BulletList * const);

extern BulletList *pBullets;


#endif /*INCLUDED_Player_h_*/
