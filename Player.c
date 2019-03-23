#include <ncurses.h>
#include <assert.h>
#include "Window.h"
#include "Player.h"
#include "Enemy.h"
#include "FPS.h"

#define PLAYER_HEIGHT 3
#define PLAYER_WIDTH 5 
#define START_X (FIELD_START_X + FIELD_WIDTH / 2)
#define START_Y (FIELD_START_Y + FIELD_HEIGHT / 3 * 2)
#define SHOOT_RATE (FRAMERATE / 4)
#define REMAINING 3
#define VX 2 
#define VY 1
#define INVINCIBLE_TIME (2 * FRAMERATE)


#define BULLET_APPR '|'
#define BULLET_VX 0
#define BULLET_VY -15
#define BULLET_OFFSET_Y -2

typedef struct Player{
    char appr[PLAYER_HEIGHT][PLAYER_WIDTH + 1];
    int x;
    int y;
    int remaining;
    int shootrate;
    int invincible;
} Player;


static Player player;
BulletList *pBullets;

void initPlayer() {
    int i, j;
    for (i = 0; i < PLAYER_HEIGHT; i++) {
        for (j = 0; j < PLAYER_WIDTH; j++) {
            player.appr[i][j] = '*';
        }
        player.appr[i][j] = '\0';
    }
    player.appr[0][0] = ' ';
    player.appr[0][PLAYER_WIDTH-1] = ' ';
    player.appr[PLAYER_HEIGHT-1][0] = ' ';
    player.appr[PLAYER_HEIGHT-1][PLAYER_WIDTH-1] = ' ';
    player.x = START_X;
    player.y = START_Y;
    player.remaining = REMAINING;
    player.shootrate = SHOOT_RATE;
    player.invincible = 0;
}


static void movePlayer(const int mx, const int my) {
   player.x += mx; 
   player.y += my;
}

void drawPlayer() {
    int i;

    wattron(field, COLOR_PAIR(1));
    for (i = 0; i < PLAYER_HEIGHT; i++) {
        mvwprintw(field, player.y + i, player.x, "%s", player.appr[i]);
    }
    wattroff(field, COLOR_PAIR(1));
}

static void shoot() {

#ifdef DEBUG
    mvwprintw(debug, 20, 1, "shoot\n");
#endif

if (kills >= 0) {
    shootBullet(pBullets, 
                'A', 
                player.x + PLAYER_WIDTH / 2, 
                player.y + BULLET_OFFSET_Y, 
                BULLET_VX,
                BULLET_VY
    );
}
if (kills >= 10) {
    shootBullet(pBullets, 
                BULLET_APPR, 
                (player.x + PLAYER_WIDTH / 2) -3, 
                (player.y + BULLET_OFFSET_Y) + 3, 
                BULLET_VX -3,
                BULLET_VY
    );
    shootBullet(pBullets, 
                BULLET_APPR, 
                (player.x + PLAYER_WIDTH / 2) +3, 
                (player.y + BULLET_OFFSET_Y) + 3, 
                BULLET_VX + 3,
                BULLET_VY
    );
}
if (kills >= 20) {
    shootBullet(pBullets, 
                'A', 
                (player.x + PLAYER_WIDTH / 2) + 1, 
                (player.y + BULLET_OFFSET_Y) +1, 
                BULLET_VX,
                BULLET_VY
    );
    shootBullet(pBullets, 
                'A', 
                player.x + PLAYER_WIDTH / 2 - 1, 
                player.y + BULLET_OFFSET_Y +1, 
                BULLET_VX,
                BULLET_VY
    );
    shootBullet(pBullets, 
                BULLET_APPR, 
                (player.x + PLAYER_WIDTH / 2) -4, 
                (player.y + BULLET_OFFSET_Y) + 5, 
                BULLET_VX -3,
                BULLET_VY
    );
    shootBullet(pBullets, 
                BULLET_APPR, 
                (player.x + PLAYER_WIDTH / 2) +4, 
                (player.y + BULLET_OFFSET_Y) + 5, 
                BULLET_VX + 3,
                BULLET_VY
    );
}
}

static int canMove(const int vx, const int vy) {
    return (contains(player.x + vx, player.y + vy) && contains(player.x + PLAYER_WIDTH -1 + vx,
    player.y + PLAYER_HEIGHT -1 + vy));
}

void updatePlayer(const int key, const int loop) {
    if (player.invincible > 0) {
        player.invincible--;
    }

    switch (key) {
        case KEY_UP:
        case 'w':
            if(canMove(0, -VY)) {
                movePlayer(0, -VY);
            }
            break;
        case KEY_DOWN:
        case 's':
            if(canMove(0, VY)) {
                movePlayer(0, VY);
            }
            break;
        case KEY_LEFT:
        case 'a':
            if(canMove(-VX, 0)) {
                movePlayer(-VX, 0);
            }
            break;
        case KEY_RIGHT:
        case 'd':
            if(canMove(VX, 0)) {
                movePlayer(VX, 0);
            }
            break;
        default :
            break;
    }

    if (loop % player.shootrate == 0) {
        shoot();
    }

}

int getPlayerRemaining() {
    return player.remaining;
}

int getPlayerX() {
    return player.x;
}

int getPlayerY() {
    return player.y;
}

int collisionDetectionPlayer(BulletList * const list) {
    int isDeleted;

    isDeleted = deleteBulletWithinRange(list, player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT);
    if (isDeleted && player.invincible == 0) {
        player.remaining--;
        player.invincible = INVINCIBLE_TIME;
        initBullets();
        player.x = START_X;
        player.y = START_Y;
        if (player.remaining < 0) {
            return GAME_OVER;
        }
    }

    return GAME_CONTINUE;
}
