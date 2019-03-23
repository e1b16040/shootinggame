#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <assert.h>
#include <termios.h>
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include "Window.h"
#include "FPS.h"

#define DEFAULT_COLOR 0
#define PLAYER_COLOR 1
#define ENEMY_COLOR 2
#define SCORE_NUM 10
#define SCORE_TYPE 3

/*プロトタイプ宣言*/
static void initGame();                      /*ゲームの初期化*/
static void initColor();                      /*カラーの初期化*/
/*
static void *input_thread();               入力処理スレッド*/

static void updateGame();                 /*ゲームシステムの更新統括 引数はstatus*/
static void updateGameState(int);      /*ゲームの状態を更新 引数はstatus, 入力key*/
static void drawScr();                   /*ゲームの状態毎に描画処理 引数はstatus*/
static void drawTitle();                    /*タイトル画面描画*/
static void drawGame();                     /*ゲーム画面描画*/
static void drawHelp();                     /*ヘルプ画面描画*/
static void drawPause();                  /*一時停止画面描画*/
static void drawScore();                    /*スコア画面描画*/
int scoreSort(const void *, const void *);
static int readScore();
static void writeScore(const struct tm * const);

enum Status {
    TITLE,
    GAME,
    HELP,
    PAUSE,
    SCORE,
    FIN
};

static enum Status status;
/*
static int inputKey;
static pthread_mutex_t imutex;
*/
static time_t startTime;
static time_t pausedTime;
static struct tm *gameTime;
static int loop;
static FILE *score;
static int scoreArray[SCORE_NUM +1][SCORE_TYPE]; /* kills, hour, min, sec */

#ifdef DEBUG
static int playerBulletNum;
static int enemyBulletNum;
#endif


int main(int argc, char const *argv[]) {
    /*
    struct termios CookedTermIos;
    struct termios RawTermIos;
    pthread_t ipthread;
    */


    /*
       初期化
    */
    /*
    tcgetattr(STDIN_FILENO, &CookedTermIos);
    RawTermIos = CookedTermIos;
    cfmakeraw(&RawTermIos);
    tcsetattr(STDIN_FILENO, 0, &RawTermIos);
    */
    initGame();
    /*
    pthread_create(&ipthread, NULL, input_thread, NULL);
    */

    /*
        メインループ
    */
    while (status != FIN) {
        loop++;

        updateFrame();
        updateGame();
        drawScr();
        wait();
    }

    printw("See you again!");
    /*
    pthread_join(ipthread, NULL);
    */
    endwin();
    /*
    tcsetattr(STDIN_FILENO, 0, &CookedTermIos);
    */


    return 0;
}

static void initGame() {

    status = TITLE;
    loop = 0;
    /*
    inputKey = -1;
    */
    if (initWindows() == -1) {
        
    }

    srand((unsigned)time(NULL));

    keypad(stdscr, TRUE);
    curs_set(0);
    crmode();
    noecho();
    timeout(1);

    initColor();
    initPlayer();
    initEnemy();
    initBullets();

}

static void initColor() {
    start_color();

    init_pair(DEFAULT_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(PLAYER_COLOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(ENEMY_COLOR, COLOR_MAGENTA, COLOR_BLACK);
}


/*
static void *input_thread() {
#ifdef debug
    mvwprintw(info, 2, 1, "input_thread");
#endif
    int key;
    while (status != FIN) {
        key = getchar();

        pthread_mutex_lock(&imutex);
        inputKey = key;
        pthread_mutex_unlock(&imutex);
    }
    endwin();
    return 0;
}
*/

static void updateGame() {
    int key;
    time_t passedtime;

/*
    if (inputKey != -1) {
        pthread_mutex_lock(&imutex);
        key = inputKey;
        inputKey = -1;
        pthread_mutex_unlock(&imutex);
    }
    */
    key = getch();

    updateGameState(key); /*入力と現在のSTATEからSTATEを更新*/

    if (status == GAME) {

        passedtime = time(NULL) - startTime;
        gameTime = localtime(&passedtime);

        updatePlayer(key, loop);    /*プレイヤー更新 移動・発射*/
        updateEnemy(loop, getPlayerX(), getPlayerY());     /*エネミー更新 発射・湧き*/

        updateBullets(pBullets, loop);  /*プレイヤーの発射した弾を更新*/
#ifdef DEBUG
        playerBulletNum = bulletNum;
#endif
        updateBullets(eBullets, loop);   /*エネミーの発射した弾を更新*/
#ifdef DEBUG
        enemyBulletNum = bulletNum;
#endif
        collisionDetectionEnemy(pBullets); /*エネミーにプレイヤーの弾が当たったか判定*/
        /*エネミーの弾がプレイヤーに当たったか判定し，残機処理．戻り値がGAME_OVERならスコア画面に遷移*/
        if (collisionDetectionPlayer(eBullets) == GAME_OVER) {
            status = SCORE;
            writeScore(gameTime);
            return;
        }
    }
}

static void updateGameState(int key) {

    if (status == TITLE) {
        if (key == '1') {
            status = GAME;
            time(&startTime);
            clear();
            refresh();
            return;
        }
        if (key == '2') {
            status = HELP;
            return;
        }
        if (key == '3') {
            status = SCORE;
            readScore();
            return;
        }
        if (key == '0') {
            status = FIN;
            return;
        }
    }

    if (status == GAME) {
        if (key == '0') {
            status = PAUSE;
            time(&pausedTime);
            return;
        }

    }

    if (status == HELP) {
        if (key == '0') {
            status = TITLE;
            return;
        }
    }

    if (status == PAUSE) {
        if (key == '1') {
            status = GAME;
            startTime += (time(NULL) - pausedTime);
            clear();
            refresh();
            return;
        }
        if (key == '0') {
            status = TITLE;
            initPlayer();
            initEnemy();
            initBullets(pBullets);
            initBullets(eBullets);
            return;
        }
    }

    if (status == SCORE) {
        if (key == '0') {
            status = TITLE;
            initPlayer();
            initEnemy();
            initBullets(pBullets);
            initBullets(eBullets);
            return;
        }
    }
    return;
}

static void drawScr() {
    if (status == GAME) {
        drawGame();
    }

    if (status == TITLE) {
        drawTitle();
    }
    if (status == HELP) {
        drawHelp();
    }
    if (status == PAUSE) {
        drawPause();
    }
    if (status == SCORE) {
        drawScore();
    }
}


static void drawTitle() {

    int sx = 0, sy = 0;
    clear();

    mvprintw(sx, sy, "SHOOTING GAME\n");

    printw("1.START\n");
    printw("2.HELP\n");
    printw("3.SCORE\n");
    printw("0.EXIT\n");

#ifdef DEBUG
    mvwprintw(debug, 1, 1, "DEBUG");
    /*
    mvwprintw(debug, 2, 1, "key:%d", inputKey);
    */
    mvwprintw(debug, 3, 1, "loop:%d", loop);
    mvwprintw(debug, 4, 1, "waitNTime:%3ds:%10ldns", waitNTime.tv_sec, waitNTime.tv_nsec);
    wnoutrefresh(stdscr);
    wnoutrefresh(debug);
    doupdate();
#else
    refresh();
#endif
}

static void drawGame() {

    clearWindows();

    drawPlayer();
    drawEnemy();
    drawBullets(pBullets, PLAYER_COLOR);
    drawBullets(eBullets, ENEMY_COLOR);

    wattron(info, A_BOLD);
    mvwprintw(info, 1, 1, "InfomationArea");
    mvwprintw(info, 2, 1, "survival time : %d:%d", gameTime->tm_min, gameTime->tm_sec);
    mvwprintw(info, 3, 1, "player remaining : %d", getPlayerRemaining());
    mvwprintw(info, 4, 1, "kills : %d", kills);
    mvwprintw(info, 9, 1, "FPS : %d", actFPS);
    wattroff(info, A_BOLD);

#ifdef DEBUG
    wnoutrefresh(debug);
    mvwprintw(debug, 1, 1, "DEBUG");
    /*
    mvwprintw(debug, 2, 1, "key:%d", inputKey);
    */
    mvwprintw(debug, 3, 1, "loop:%d", loop);
    mvwprintw(debug, 4, 1, "waitNTime:%3ds:%10ldns", waitNTime.tv_sec, waitNTime.tv_nsec);
    mvwprintw(debug, 5, 1, "PlayerBulletNum:%d", playerBulletNum);
    mvwprintw(debug, 6, 1, "EnemyBulletNum:%d", enemyBulletNum);
#endif

    refreshWindows();

}

static void drawHelp() {

    int sx = 0, sy = 0;
    clear();

    mvprintw(sx, sy, "HELP\n");

    printw("GAME RULE\n");

    printw("\nOPERATION\n");
    printw("ACTION\t\tKEY\n\n");
    printw("MOVE PLAYER\tCURSOR KEY\n");

    printw("\nplease press 0 key and go to title.");

#ifdef DEBUG
    mvwprintw(debug, 1, 1, "DEBUG");
    /*
    mvwprintw(debug, 2, 1, "key:%d", inputKey);
    */
    mvwprintw(debug, 3, 1, "loop:%d", loop);
    mvwprintw(debug, 4, 1, "waitNTime:%3ds:%10ldns", waitNTime.tv_sec, waitNTime.tv_nsec);
    wnoutrefresh(stdscr);
    wnoutrefresh(debug);
    doupdate();
#else
    refresh();
#endif
}

static void drawPause() {

    int sx = 0, sy = 0;
    clear();

    mvprintw(sx, sy, "Paused now\n");

    printw("1.GAME\n");
    printw("0.TITLE\n");

#ifdef DEBUG
    mvwprintw(debug, 1, 1, "DEBUG");
    /*
    mvwprintw(debug, 2, 1, "key:%d", inputKey);
    */
    mvwprintw(debug, 3, 1, "loop:%d", loop);
    mvwprintw(debug, 4, 1, "waitNTime:%3ds:%10ldns", waitNTime.tv_sec, waitNTime.tv_nsec);
    wnoutrefresh(stdscr);
    wnoutrefresh(debug);
    doupdate();
#else
    refresh();
#endif
}

static void drawScore() {
    int sx = 0, sy = 0, i;
    clear();

    mvprintw(sx, sy, "SCORE\n");

    printw("Kill Count\tSurvival Time\n");
    for (i = 0; i < SCORE_NUM; i++) {
        if (scoreArray[i][0] == -1) {
            break;
        }
        printw("%2d:\t%d\t\t%2d:%2ds\n", i+1, scoreArray[i][0], scoreArray[i][1], scoreArray[i][2]);
    }

    printw("\nplease press 0 key and go to title.");
#ifdef DEBUG
    mvwprintw(debug, 1, 1, "DEBUG");
    /*
    mvwprintw(debug, 2, 1, "key:%d", inputKey);
    */
    mvwprintw(debug, 3, 1, "loop:%d", loop);
    mvwprintw(debug, 4, 1, "waitNTime:%3ds:%10ldns", waitNTime.tv_sec, waitNTime.tv_nsec);
    wnoutrefresh(stdscr);
    wnoutrefresh(debug);
    doupdate();
#else
    refresh();
#endif
}
int scoreSort(const void *a, const void *b) {
    assert(a != NULL && b != NULL);
    return ((int *)b)[0] - ((int *)a)[0];
}

static int readScore() {
    int i = 0;
    char scoreLine[21];
    char *tmpChar[3];

    for (i = 0; i < SCORE_NUM +1; i++) {
        assert(scoreArray[i] != NULL);
        scoreArray[i][0] = -1;
    }
    i = 0;

    if ((score = fopen("score.txt", "r")) != NULL) {
        for (i = 0; i < SCORE_NUM; i++) {
            if (fgets(scoreLine, 20, score) == NULL) {
                break;
            }
            assert(scoreArray[i] != NULL);
            tmpChar[0] = strtok(scoreLine, ",");
            tmpChar[1] = strtok(NULL, ",");
            tmpChar[2] = strtok(NULL, ",");
            scoreArray[i][0] = atoi(tmpChar[0]);
            scoreArray[i][1] = atoi(tmpChar[1]);
            scoreArray[i][2] = atoi(tmpChar[2]);
        }
        fclose(score);
    }
    return i;
}

static void writeScore(const struct tm * const gameTime) {
    int i = 0;
    i = readScore();
    scoreArray[i][0] = kills;
    scoreArray[i][1] = gameTime->tm_min;
    scoreArray[i][2] = gameTime->tm_sec;


    qsort((void *)scoreArray, sizeof(scoreArray)/sizeof(scoreArray[0]), sizeof(scoreArray[0]), scoreSort);
    /* fprintf(score, "%d,%d,%d,%d\n", kills, gameTime->tm_hour, gameTime->tm_min, gameTime->tm_sec);*/

    if ((score = fopen("score.txt", "w")) == NULL) {
        return;
    }
    for (i = 0; i < SCORE_NUM; i++) {
        if (scoreArray[i][0] == -1) {
            break;
        }
        fprintf(score, "%d,%d,%d\n", scoreArray[i][0], scoreArray[i][1], scoreArray[i][2]);
    }
    fclose(score);
}
