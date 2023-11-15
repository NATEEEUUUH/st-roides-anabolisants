#include <SFML/Graphics.h>

#ifndef PLAYER_H
#define PLAYER_H

typedef struct Player
{
    int IsAlive;
    float x;
    float y;
    float speed;
    float rotation;
    int score;
} Player;

typedef struct Arrow
{
    int IsShot;
    float x;
    float y;
    float rotation;
    sfVector2f velocity;
} Arrow;

Player playerInit();
Arrow arrowInit();
sfSprite* playerSpriteInit(sfRenderWindow* gameWindow, Player* player);
void handlePlayerInput(sfRenderWindow* gameWindow, Player* player, sfSprite* playerSprite);
sfSprite* cursorSpriteInit(sfRenderWindow* gameWindow);
sfSprite* arrowSpriteInit(sfRenderWindow* gameWindow, Arrow* arrow);
void handleShootingArrow(sfRenderWindow* gameWindow, Player* player, Arrow* arrow, sfSprite* playerSprite, sfSprite* arrowSprite, sfSprite* cursorSprite);

#endif