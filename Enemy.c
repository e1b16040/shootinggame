#include <ncurses.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "Window.h"
#include "Enemy.h"
#include "FPS.h"

/* 全エネミー共通の値を定義する */
#define ENEMY_TYPE_NUM 2
#define ENEMY_NUM_MAX 30
#define ENEMY_HEIGHT 3
#define ENEMY_WIDTH 5
#define ENEMY_SIZE ENEMY_HEIGHT * ENEMY_WIDTH
#define ENEMY_BULLET_OFFSET_Y 2
#define SPAWN_POINTS_NUM 30
#define SPAWN_POINT_GAP 6
#define SPAWN_AREA_BOTTOM 20


/* エネミータイプごとの値をconst配列として宣言 */
const char eAppr[ENEMY_TYPE_NUM][ENEMY_HEIGHT][ENEMY_WIDTH + 1] = {{" * * \0", "* * *\0", " * *\0"}, {"+ + +\0", " + + \0", "+ + +\0"}}; /*height 3 width 5*/
const int eBv[ENEMY_TYPE_NUM] = {6, 2};
const int eLife[ENEMY_TYPE_NUM] = {5, 8};
const int eBsr[ENEMY_TYPE_NUM] = {FRAMERATE * 2 , FRAMERATE * 4};

int spawnPoints[SPAWN_POINTS_NUM][2]; /* x,y  */
const int spawnRate = FRAMERATE * 2; /* spawnRate秒毎にspawnする */

Enemy enemy[ENEMY_NUM_MAX];
BulletList *eBullets;
int kills;

/* エネミー構造体の操作関数  */

static int isAlive(const int index) {
    assert(0 <= index && index < ENEMY_NUM_MAX);
    return enemy[index].life > 0;
}

static int getNewIndex() {
    int i;

    for (i = 0; i < ENEMY_NUM_MAX; i++) {
        if (!isAlive(i)) {
            return i;
        }
    }
    return -1;
}

static void setParam(const int index, const int _type, const int _x, const int _y, const int birth) {
    assert(0 <= index && index < ENEMY_NUM_MAX);

    enemy[index].type = _type;
    enemy[index].x = _x;
    enemy[index].y = _y;
    enemy[index].life = eLife[enemy[index].type];
    enemy[index].birth = birth;
}

static int isSamePos(const int index, const int _x, const int _y) {
    assert(0 <= index && index < ENEMY_NUM_MAX);
    assert(isAlive(index));
    return (enemy[index].x == _x && enemy[index].y == _y);
}

/* エネミー全体の操作関数  */

void initEnemy() {
    int i;

    kills = 0;
    for (i = 0; i < ENEMY_NUM_MAX; i++) {
        enemy[i].life = 0;
    }
    for (i = 0; i < SPAWN_POINTS_NUM; i++) {
        spawnPoints[i][0] = 4 + (ENEMY_WIDTH + 7) * (i % 10);
        spawnPoints[i][1] = 4 + (ENEMY_HEIGHT + 4) * (i / 10);
    }
}
 

static void shoot(const int index, const int loop, const int px, const int py) {
#ifdef DEBUG
    mvwprintw(debug, 30, 1, "shoot:%d", (loop % (FRAMERATE * eBsr[enemy[index].type])));
    mvwprintw(debug, 30, 1, "%d:%d:%d", loop, FRAMERATE , eBsr[enemy[index].type]);
#endif
    assert(0 <= index && index < ENEMY_NUM_MAX);
    int dx = px - enemy[index].x;
    int dy = py - enemy[index].y;
    double dr = sqrt(dx * dx + dy * dy);
    int shootTimeCounter = ((loop - enemy[index].birth) % eBsr[enemy[index].type]);

    if (!isAlive(index)) {
        return;
    }
    /* エネミーが生成されてから一定間隔で弾を発射 */
    if (enemy[index].type == 0) {
        if (shootTimeCounter == 0 || shootTimeCounter == 2) {
            shootBullet(eBullets,
                        '*',
                        enemy[index].x + ENEMY_WIDTH / 2,
                        enemy[index].y + ENEMY_BULLET_OFFSET_Y,
                        (int)(eBv[enemy[index].type] / dr * dx),
                        (int)(eBv[enemy[index].type] / dr * dy));
        }
    } else if (enemy[index].type == 1) {
        if (shootTimeCounter == 0) {
            shootBullet(eBullets,
                        '|',
                        enemy[index].x + ENEMY_WIDTH / 2,
                        enemy[index].y + ENEMY_BULLET_OFFSET_Y,
                        (int)(eBv[enemy[index].type] * 0),
                        (int)(eBv[enemy[index].type]));
            shootBullet(eBullets,
                        '|',
                        enemy[index].x + ENEMY_WIDTH / 2,
                        enemy[index].y + ENEMY_BULLET_OFFSET_Y,
                        (int)(eBv[enemy[index].type] / 2),
                        (int)(eBv[enemy[index].type]));
            shootBullet(eBullets,
                        '|',
                        enemy[index].x + ENEMY_WIDTH / 2,
                        enemy[index].y + ENEMY_BULLET_OFFSET_Y,
                        (int)(eBv[enemy[index].type] / -2),
                        (int)(eBv[enemy[index].type]));
        }
    }
}

static void spawn(const int index, const int _type, const int _x, const int _y, const int birth) {
    assert(0 <= index && index < ENEMY_NUM_MAX);
    setParam(   index, 
                _type,
                _x, 
                _y, 
                birth
    );
}


void updateEnemy(const int loop, const int px, const int py) {
    int index, type, SPIndex;
    int i, canSpawn = 1;

    /* spawnRate秒ごとにspawn */
    if (loop % spawnRate == 0) {
        type = rand() % ENEMY_TYPE_NUM;
        index = getNewIndex();
        SPIndex = rand() % SPAWN_POINTS_NUM;
        for (i = 0; i < ENEMY_NUM_MAX; i++) {
            if (isAlive(i) && isSamePos(i, spawnPoints[SPIndex][0], spawnPoints[SPIndex][1])) {
                canSpawn = 0;
            }
        }
        if (index >= 0 && canSpawn) {
            spawn(index, type, spawnPoints[SPIndex][0], spawnPoints[SPIndex][1], loop);
        }
    }

    for (i = 0; i < ENEMY_NUM_MAX; i++) {
        shoot(i, loop, px, py);
    }
}

void drawEnemy() {
    int i, j;

    wattron(field, COLOR_PAIR(2));
    for (i = 0; i < ENEMY_NUM_MAX; i++) {
        if (isAlive(i)) {
            for (j = 0; j < ENEMY_HEIGHT; j++) {
                mvwprintw(field, enemy[i].y + j, enemy[i].x, "%s", eAppr[enemy[i].type][j]);
            }
        }
    }
    wattroff(field, COLOR_PAIR(2));
}


static void collisionDetection(const int index, BulletList * const list) {
    assert(0 <= index && index < ENEMY_NUM_MAX);
    assert(list != NULL);
    int isDeleted;

    if (!isAlive(index)) {
        return;
    }

    isDeleted = deleteBulletWithinRange(list, enemy[index].x, enemy[index].y, ENEMY_WIDTH, ENEMY_HEIGHT);
    if (isDeleted) {
        enemy[index].life--;
        if (enemy[index].life <= 0) {
            kills++;
        }
    }
}

void collisionDetectionEnemy(BulletList * const list) {
    assert(list != NULL);
    int i;

    for (i = 0; i < ENEMY_NUM_MAX; i++) {
        collisionDetection(i, list);
    }
}

