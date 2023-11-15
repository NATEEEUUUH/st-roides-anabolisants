#include <SFML/Window.h>
#include <SFML/Graphics.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>  
#include <time.h>


#ifndef ENEMIES_H
#define ENEMIES_H

typedef struct Slime
{
	float x;
	float y;
	float speed;
	int size;
	int hitCount;
	float rotation;
	int isAlive;
} Slime;

Slime slimeInit();
sfSprite* slimeSpriteInit(sfRenderWindow* gameWindow, Slime* slime);
void handleSlimeMovementsCollisions(sfRenderWindow* gameWindow, Slime* slime, Player* player, Arrow* arrow, sfSprite* playerSprite, sfSprite* arrowSprite, sfSprite* slimeSprite);
#endif 