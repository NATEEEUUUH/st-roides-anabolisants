#include <SFML/Window.h>
#include <SFML/Graphics.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <time.h>
#include "player.h"
#include "enemies.h"

#define ARROW_SPEED 10.0f
#define NUM_SLIMES 12

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

void initializeSlimes()
{
    for (int i = 0; i < NUM_SLIMES; ++i)
    {
        slime[i] = slimeInit();
        slimeSprite[i] = slimeSpriteInit(gameWindow, &slime[i]);
    }
}

void respawnSlime(int index)
{
    slime[index] = slimeInit();
    slimeSprite[index] = slimeSpriteInit(gameWindow, &slime[index]);
}

int main()
{
    sfVideoMode VideoMode = { 1920, 1080, 64 };
    gameWindow = sfRenderWindow_create(VideoMode, "Danazol", sfClose, NULL);
    sfRenderWindow_setFramerateLimit(gameWindow, 60);
    sfEvent event;
    sfColor blue = sfColor_fromRGBA(150, 150, 255, 0);

    sfFont* font = sfFont_createFromFile("alagard.ttf");

    scoreText = sfText_create();
    sfText_setFont(scoreText, font);
    sfText_setCharacterSize(scoreText, 40);
    sfVector2f textPosition = { 10.0f, 10.0f };
    sfText_setPosition(scoreText, textPosition);

    gameOverText = sfText_create();
    sfText_setFont(gameOverText, font);
    sfText_setCharacterSize(gameOverText, 120);

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
                sfRenderWindow_close(gameWindow);
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
    if (player.IsAlive == 1)
    {
        if (arrow.IsShot)
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
    }
#pragma endregion

#pragma region Slime Position Calculation 
    if (player.IsAlive == 1)
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
        printScore(gameWindow, &player);
        gameOver(gameWindow, &player);
        sfRenderWindow_display(gameWindow);
    }
    for (int i = 0; i < NUM_SLIMES; ++i)
    {
        sfSprite_destroy(slimeSprite[i]);
    }

    sfSprite_destroy(playerSprite);
    sfSprite_destroy(cursorSprite);
    sfSprite_destroy(arrowSprite);
    sfRenderWindow_destroy(gameWindow);
    sfText_destroy(gameOverText);
    return 0;
}

int printScore(sfRenderWindow* gameWindow, Player* player)
{
    char scoreString[50];
    snprintf(scoreString, sizeof(scoreString), "     Score: %d", player->score - 0);
    sfText_setString(scoreText, scoreString);
    sfRenderWindow_drawText(gameWindow, scoreText, NULL);
}

int gameOver(sfRenderWindow* gameWindow, Player* player)
{
    if (player->IsAlive == 0)
    {
        char GOString[50];
        snprintf(GOString, sizeof(GOString), "GAME OVER");
        sfText_setString(gameOverText, GOString);

        sfText_setCharacterSize(scoreText, 100);
        sfVector2f textPosition = { 640.0f, 500.0f };
        sfText_setPosition(scoreText, textPosition);

        sfVector2f gameOverPosition = { 650.0f, 400.0f };
        sfText_setPosition(gameOverText, gameOverPosition);

        sfRenderWindow_clear(gameWindow, sfBlack);
        sfText_setColor(gameOverText, sfRed);
        sfRenderWindow_drawText(gameWindow, gameOverText, NULL);
        sfRenderWindow_drawText(gameWindow, scoreText, NULL);
    }
    return 0;
}