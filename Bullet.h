#ifndef INCLUDED_Bullet_h_
#define INCLUDED_Bullet_h_

typedef struct Bullet{
    char appr;
    int x;      /* 弾の現在位置 */
    int y;
    int vx;      /* 弾の速度 */
    int vy;
    struct Bullet *next;    
} Bullet;

typedef struct bulletlist {
    Bullet *head;
} BulletList;

void initBullets();
void deleteBullet(BulletList * const, const int , const int);
int deleteBulletWithinRange(BulletList * const, const int, const int, const int, const int);
void shootBullet(BulletList * const , const char, const int, const int, const int, const int); 
void updateBullets(BulletList * const, const int);
void drawBullets(const BulletList * const, const int);

#ifdef DEBUG
int bulletNum;
#endif


#endif /*INCLUDED_Bullet_h_*/
