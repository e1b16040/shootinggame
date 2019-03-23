# DDEGBUG
ShootingGame: ShootingGame.o Player.o Enemy.o Bullet.o Window.o FPS.o
	echo 'Starts linking ShootingGame'
	cc -o ShootingGame ShootingGame.o Player.o Enemy.o Bullet.o Window.o FPS.o -lncurses -lpthread -lm -DDEBUG
ShootingGame.o: ShootingGame.c Player.c Enemy.o Bullet.o Window.o FPS.o
	echo 'Starts compling ShootingGame.c'
	cc -c -Wall ShootingGame.c -lncurses -lpthread -DDEBUG
Player.o: Player.c Bullet.o Enemy.o Window.o FPS.o Player.h 
	echo 'Starts compling Player.c'
	cc -c -Wall Player.c -lncurses -DDEBUG
Enemy.o: Enemy.c Bullet.o FPS.o Enemy.h
	echo 'Starts compling Enemy.c'
	cc -c -Wall Enemy.c -lncurses -lm -DDEBUG
Bullet.o: Bullet.c Window.o FPS.o Bullet.h
	echo 'Starts compling Bullet.c'
	cc -c -Wall Bullet.c -lncurses -DDEBUG
Window.o: Window.c Window.h
	echo 'Starts compling Window.c'
	cc -c -Wall Window.c -lncurses -DDEBUG
FPS.o: FPS.c FPS.h
	echo 'Starts compling FPS.c'
	cc -c -Wall FPS.c -lncurses -DDEBUG
#
clean:
	rm *.o
