/* 
忘れないように　仕様
Enemy構造体はインスタンスに必要な情報のみもたせる．例えばx, y, life, type
Enemyのインスタンスはindexによって区別する
typeによって，appr(appearance), eBvx eBvy(enemy Bullet velocity x, y) 初期life
eBsr(enemy Bullet shoot rate)などを操作関数内でconst配列から得る

*/

#ifndef INCLUDED_Enemy_h_
#define INCLUDED_Enemy_h_

#include "Bullet.h"

typedef struct Enemy{
    int type;
    int x;
    int y;
    int life;
    int birth;
} Enemy;


void initEnemy(); /*敵の初期化*/
void drawEnemy(); /*敵描画*/
void updateEnemy(const int, const int, const int); /*敵の弾発射処理*/
void collisionDetectionEnemy(BulletList * const); /*プレイヤーの弾と敵との当たり判定計算*/

extern BulletList *eBullets;
extern int kills;

#endif /*INCLUDED_Enemy_h_*/
