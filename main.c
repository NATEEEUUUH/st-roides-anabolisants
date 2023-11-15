#include <SFML/Window.h>
#include <SFML/Graphics.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <time.h>
#include "player.h"
#include "enemies.h"

#define ARROW_SPEED 10.0f
#define NUM_SLIMES 10
#define PLAYER_WIDTH 64
#define PLAYER_HEIGHT 64
#define SLIME_WIDTH 64
#define SLIME_HEIGHT 64 

sfRenderWindow* gameWindow;
Player player;
Arrow arrow;
Slime slime[NUM_SLIMES];
sfSprite* playerSprite;
sfSprite* cursorSprite;
sfSprite* arrowSprite;
sfSprite* slimeSprite[NUM_SLIMES];
sfText* scoreText;
sfText* gameOverText;

int gameOver = 0;

void initializeSlimes()
{
    for (int i = 0; i < NUM_SLIMES; ++i)
    {
        slime[i] = slimeInit();
        while (slimeOverlapsPlayer(&slime[i], &player))
        {
            slime[i] = slimeInit(slime, i);
        }
        slimeSprite[i] = slimeSpriteInit(gameWindow, &slime[i]);
    }
}

void respawnSlime(int index)
{
    slime[index] = slimeInit();

    while (slimeOverlapsPlayer(&slime[index], &player))
    {
        slime[index] = slimeInit();
    }
    slimeSprite[index] = slimeSpriteInit(gameWindow, &slime[index]);
}

int slimeOverlapsPlayer(const Slime* slime, const Player* player)
{
    return (
        slime->x < player->x + PLAYER_WIDTH &&
        slime->x + SLIME_WIDTH > player->x &&
        slime->y < player->y + PLAYER_HEIGHT &&
        slime->y + SLIME_HEIGHT > player->y
        );
}

int main()
{
    sfVideoMode VideoMode = { 1920, 1080, 64 };
    gameWindow = sfRenderWindow_create(VideoMode, "Danazol", sfClose, NULL);
    sfRenderWindow_setFramerateLimit(gameWindow, 60);
    sfEvent event;
    sfColor blue = sfColor_fromRGBA(130, 130, 255, 0);
    sfFont* font = sfFont_createFromFile("alagard.ttf");

#pragma region Score Text
    scoreText = sfText_create();
    sfText_setFont(scoreText, font);
    sfText_setCharacterSize(scoreText, 40);
    sfVector2f textPosition = { 10.0f, 10.0f };
    sfText_setPosition(scoreText, textPosition);
#pragma endregion

#pragma region gameOver Text
    gameOverText = sfText_create();
    sfText_setFont(gameOverText, font);
    sfText_setCharacterSize(gameOverText, 120);
    sfVector2f gameOverPosition = { 650.0f, 200.0f };
    sfText_setPosition(gameOverText, gameOverPosition);
#pragma endregion

    player = playerInit();
    arrow = arrowInit();
    playerSprite = playerSpriteInit(gameWindow, &player);
    arrowSprite = arrowSpriteInit(gameWindow, &arrow);
    cursorSprite = cursorSpriteInit(gameWindow);
    sfVector2f cursorPosition = { 0.0f, 0.0f };

    while (sfRenderWindow_isOpen(gameWindow))
    {
        sfRenderWindow_clear(gameWindow, blue);
        if (sfRenderWindow_pollEvent(gameWindow, &event))
        {
            if (event.type == sfEvtClosed)
            {
                sfRenderWindow_close(gameWindow);  
            }
        }

        sfVector2i mousePosition = sfMouse_getPositionRenderWindow(gameWindow);

#pragma region Player Rotation Calculation
        sfVector2f playerPosition = sfSprite_getPosition(playerSprite);
        sfVector2f direction = { mousePosition.x - playerPosition.x, mousePosition.y - playerPosition.y };
        float angle = atan2f(direction.y, direction.x) * 180 / 3.14159276f;
        sfVector2f playerVelocity = { cos(angle * 3.14159276f / 180) * player.speed, sin(angle * 3.14159276f / 180) * player.speed };
        player.x += playerVelocity.x;
        player.y += playerVelocity.y;
        sfSprite_setRotation(playerSprite, angle); 
#pragma endregion

#pragma region Cursor Position Calculation
        cursorPosition.x = (float)mousePosition.x; 
        cursorPosition.y = (float)mousePosition.y; 
        sfSprite_setPosition(cursorSprite, cursorPosition); 
#pragma endregion

#pragma region Arrow Position Calculation
        if (arrow.IsShot && gameOver == 0)
        {
            sfVector2f arrowPosition = sfSprite_getPosition(arrowSprite);
            sfVector2f arrowDirection = { mousePosition.x - arrowPosition.x, mousePosition.y - arrowPosition.y };
            float length = sqrt(arrowDirection.x * arrowDirection.x + arrowDirection.y * arrowDirection.y);
            float arrowAngle = atan2f(arrowDirection.y, arrowDirection.x) * 180 / 3.14159276f;
            sfVector2f normalizedDirection = { arrowDirection.x / length, arrowDirection.y / length };
            sfVector2f arrowVelocity = { normalizedDirection.x * ARROW_SPEED, normalizedDirection.y * ARROW_SPEED };
            sfSprite_move(arrowSprite, arrowVelocity);
            sfSprite_setRotation(arrowSprite, arrowAngle);
            sfRenderWindow_drawSprite(gameWindow, arrowSprite, NULL);
        }
#pragma endregion

#pragma region Slime Position Calculation 
    if (gameOver == 0)
    {
        for (int i = 0; i < NUM_SLIMES; ++i)
        {
            if (slime[i].size > 0)
            {
                sfRenderWindow_drawSprite(gameWindow, slimeSprite[i], NULL);
                handleSlimeMovementsCollisions(gameWindow, &slime[i], &player, &arrow, playerSprite, arrowSprite, slimeSprite[i]);
            }
            else
            {
                respawnSlime(i);
            }
        }
    }
#pragma endregion

        sfRenderWindow_drawSprite(gameWindow, playerSprite, NULL);
        handlePlayerInput(gameWindow, &player, playerSprite);
        sfSprite_move(playerSprite, playerVelocity);
        sfRenderWindow_drawSprite(gameWindow, cursorSprite, NULL);
        handleShootingArrow(gameWindow, &player, &arrow, playerSprite, arrowSprite, cursorSprite);
        HUD(gameWindow, &player);
        sfRenderWindow_display(gameWindow);  
    }

    for (int i = 0; i < NUM_SLIMES; ++i)
    {
        sfSprite_destroy(slimeSprite[i]);
    }

    sfSprite_destroy(playerSprite); 
    sfSprite_destroy(cursorSprite);  
    sfSprite_destroy(arrowSprite); 
    sfText_destroy(scoreText); 
    sfText_destroy(gameOverText);
    sfRenderWindow_destroy(gameWindow); 
    return 0; 
}


int HUD(sfRenderWindow* gameWindow, Player* player)
{
    char scoreString[50];
    snprintf(scoreString, sizeof(scoreString), "     Score: %d", player->score);
    sfText_setString(scoreText, scoreString);
    sfRenderWindow_drawText(gameWindow, scoreText, NULL);

    if (player->IsAlive == 0)
    {
        gameOver = 1;

        char GOString[50];
        snprintf(GOString, sizeof(GOString), "GAME OVER");
        sfText_setString(gameOverText, GOString);
        sfRenderWindow_clear(gameWindow, sfBlack);
        sfText_setColor(gameOverText, sfRed);
        sfRenderWindow_drawText(gameWindow, gameOverText, NULL);

        char scoreString[50];
        snprintf(scoreString, sizeof(scoreString), "     Score: %d", player->score); 
        sfText_setString(scoreText, scoreString); 
        sfRenderWindow_drawText(gameWindow, scoreText, NULL); 
        sfText_setCharacterSize(scoreText, 80);
        sfVector2f scorePosition = { 680.0f, 320.0f };
        sfText_setPosition(scoreText, scorePosition);  
    }
    return 0;
}