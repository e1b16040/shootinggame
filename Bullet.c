#include <ncurses.h>
#include <stdlib.h>
#include <assert.h>
#include "Window.h"
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include "FPS.h"


static int isSamePos(const Bullet * const bullet, const int x, const int y) {
    assert(bullet != NULL);
    return (bullet->x == x && bullet->y == y);
}

static int containsBullet(const Bullet * const bullet, const int x, const int y, const int width, const int height) {
    assert(bullet != NULL);
    return (x <= bullet->x && x + width > bullet->x && y <= bullet->y && y + height > bullet->y);
}

static void moveBullet(Bullet * const bullet, int loop) {
    assert(bullet != NULL);
    if (bullet->vx != 0 && loop % (FRAMERATE / abs(bullet->vx)) == 0) {
        if (bullet->vx > 0) {
            bullet->x++;
        } else {
            bullet->x--;
        }
    }
    if (bullet->vy != 0 && loop % (FRAMERATE / abs(bullet->vy)) == 0){
        if (bullet->vy > 0) {
            bullet->y++;
        } else {
            bullet->y--;
        }
    }
}

static Bullet *createBullet(const char _appr, const int _x, const int _y, const int _vx, const int _vy) {
    Bullet *newBullet = malloc(sizeof(Bullet));
    if (!newBullet) {
        return NULL;
    }
    newBullet->appr = _appr;
    newBullet->x = _x;
    newBullet->y = _y;
    newBullet->vx = _vx;
    newBullet->vy = _vy;
    newBullet->next = NULL;
    return newBullet;
}

static BulletList *emptyList() {
    BulletList *list = malloc(sizeof(BulletList));
    list->head = NULL;
    return list;
}

static void appendBullet(BulletList * const list, const char _appr, const int _x, const int _y, const int _vx, const int _vy) {
    assert(list != NULL);
    Bullet * current = NULL;
    if (list->head == NULL) {
        list->head = createBullet(_appr, _x, _y, _vx, _vy);

#ifdef DEBUG
    mvwprintw(debug, 11, 1, "%c:(%3d, %3d), v(%3d, %3d)", list->head->appr, list->head->x, list->head->y, list->head->vx, list->head->vy);
#endif
    assert(list != NULL);
        return;
    }

    current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = createBullet(_appr, _x, _y, _vx, _vy);
    assert(list != NULL);

#ifdef DEBUG
    mvwprintw(debug, 11, 1, "%c:(%3d, %3d), v(%3d, %3d)", current->next->appr, current->next->x, current->next->y, current->next->vx, current->next->vy);
#endif
}

static void destroyBulletList(BulletList *list) {
    if (list == NULL) {
        return;
    }
    Bullet *current = list->head;
    Bullet *next = current;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

void initBullets() {
    destroyBulletList(pBullets);
    destroyBulletList(eBullets);
    pBullets = emptyList();
    eBullets = emptyList();
}

void deleteBullet(BulletList * const list, const int x, const int y) {
    assert(list != NULL);
    Bullet *current = list->head;
    Bullet *previous = current;

    while (current != NULL) {
        if (isSamePos(current, x, y)) {
            previous->next = current->next;
            if (current == list->head) {
                list->head = current->next;
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

int deleteBulletWithinRange(BulletList * const list, const int x, const int y, const int width, const int height) {
    assert(list != NULL);
    int isDeleted = 0;
    Bullet *current = list->head;
    Bullet *tmp;

    /* 先頭のBulletが範囲内にあるならば削除、以降範囲内にあるBulletを連続して削除 */
    while (current != NULL && containsBullet(current, x, y, width, height)) {
        tmp = current;
        current = current->next;
        list->head = current;
        free(tmp);
        isDeleted = 1;
    }

    /* リストの先頭以降のBulletが範囲内にあるならば連続して削除、範囲外のBulletが来たら次の範囲内のBulletを探索 */
    for (current = list->head; current != NULL; current = current->next) {
        /* リスト内の連続して範囲内にあるBulletを削除 */
        while (current->next != NULL && containsBullet(current->next, x, y, width, height)) {
            tmp = current->next;
            current->next = tmp->next;
            free(tmp);
            isDeleted = 1;
        }
    }

    return isDeleted;
}


void shootBullet(BulletList * const list, const char _appr, const int _x, const int _y, const int _vx, const int _vy) {
    assert(list != NULL);
#ifdef DEBUG
    mvwprintw(debug,  10, 1, "shootBullet:%p", list);
#endif
    appendBullet(list, _appr, _x, _y, _vx, _vy);
    assert(list != NULL);
}

void updateBullets(BulletList * const list, const int loop) {
    assert(list != NULL);
    assert(loop >= 0);
#ifdef DEBUG
    bulletNum = 0;
#endif
    /* deleteを埋め込む、previousとcurrentを保持して、deleteする */
    Bullet *current = list->head;
    Bullet *tmp;

    /* moveして、先頭のBulletが範囲外にでたならば削除、以降範囲外に出たBulletを連続して削除 */
    while (current != NULL) {
#ifdef DEBUG
    bulletNum++;
#endif
        moveBullet(current, loop);
        if (!contains(current->x, current->y)) {
            tmp = current;
            current = current->next;
            list->head = current;
            free(tmp);
        } else {
            break;
        }
    }

    /* moveして、リストの先頭以降のBulletが範囲外に出たならば連続して削除、範囲内ののBulletが来たら次の範囲外のBulletを探索 */
    for (current = list->head; current != NULL; current = current->next) {
        /* リスト内の連続して範囲外に出たBulletを削除 */
        while (current->next != NULL) {
            moveBullet(current->next, loop);

#ifdef DEBUG
    bulletNum++;
#endif
            if (!contains(current->next->x, current->next->y)) {
                tmp = current->next;
                current->next = tmp->next;
                free(tmp);
            } else {
                break;
            }
        }
    }
}

void drawBullets(const BulletList * const list , const int colorNum) {
    assert(list != NULL);
    Bullet * current = list->head;

#ifdef DEBUG
    mvwprintw(debug, 12, 1, "drawBullets\n");
#endif

    if (list->head == NULL) {
        return;
    }

    wattron(field, COLOR_PAIR(colorNum));
    while (current->next != NULL) {
        mvwprintw(field, current->y, current->x, "%c", current->appr);
        current = current->next;
    }
    mvwprintw(field, current->y, current->x, "%c", current->appr);
    wattroff(field, COLOR_PAIR(colorNum));
}
