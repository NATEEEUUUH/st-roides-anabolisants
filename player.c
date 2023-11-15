#include <SFML/Graphics.h>
#include "player.h"
#include "enemies.h"

#define ARROW_SPEED 10.0f

Player playerInit()
{
    Player player;
    player.IsAlive = 1;
    player.x = 960;
    player.y = 540;
    player.rotation = 0;
    player.speed = 0;
    player.score = 0;
    return player;
}

Arrow arrowInit()
{
    Arrow arrow;
    arrow.IsShot = 0;
    arrow.x = 0;
    arrow.y = 0;
    arrow.rotation = 0;
    return arrow;
}

sfSprite* playerSpriteInit(sfRenderWindow* gameWindow, Player* player)
{
    sfTexture* playerTexture = sfTexture_createFromFile("sprites/Knight.png", NULL);
    sfSprite* playerSprite = sfSprite_create();
    sfSprite_setTexture(playerSprite, playerTexture, sfFalse);
    sfVector2f pOrigin = { 32.0f, 32.0f };
    sfSprite_setOrigin(playerSprite, pOrigin);
    sfVector2f playerCord = { player->x, player->y };
    sfSprite_setPosition(playerSprite, playerCord);
    return playerSprite;
}

void handlePlayerInput(sfRenderWindow* gameWindow, Player* player, sfSprite* playerSprite)
{
#pragma region Player Rotation Calculation
    sfVector2i mousePosition = sfMouse_getPositionRenderWindow(gameWindow);
    sfVector2f playerPosition = sfSprite_getPosition(playerSprite);
    sfVector2f direction = { mousePosition.x - playerPosition.x, mousePosition.y - playerPosition.y };
    float angle = atan2f(direction.y, direction.x) * 180 / 3.14159276f;
#pragma endregion

    if (player->IsAlive == 1)
    {
        if (sfKeyboard_isKeyPressed(sfKeyZ))
        {
            player->speed = 4;
        }
        else if (sfKeyboard_isKeyPressed(sfKeyS))
        {
            player->speed = -4;
        }
        else
        {
            player->speed = player->speed * 0.99;
        }
    }  
}

sfSprite* cursorSpriteInit(sfRenderWindow* gameWindow)
{
    sfTexture* cursorTexture = sfTexture_createFromFile("sprites/Target.png", NULL);
    sfSprite* cursorSprite = sfSprite_create();
    sfSprite_setTexture(cursorSprite, cursorTexture, sfFalse);
    sfVector2f cOrigin = { 32.0f, 32.0f };
    sfSprite_setOrigin(cursorSprite, cOrigin);
    sfSprite_setScale(cursorSprite, (sfVector2f) { 0.6f, 0.6f });
    return cursorSprite;
}

sfSprite* arrowSpriteInit(sfRenderWindow* gameWindow, Arrow* arrow)
{
    sfTexture* arrowTexture = sfTexture_createFromFile("sprites/Arrow.png", NULL);
    sfSprite* arrowSprite = sfSprite_create(); 
    sfSprite_setTexture(arrowSprite, arrowTexture, sfTrue);
    sfVector2f aOrigin = { 25.0f, 6.0f };
    sfSprite_setOrigin(arrowSprite, aOrigin);
    sfSprite_setScale(arrowSprite, (sfVector2f) { 0.6f, 0.6f });
    return arrowSprite;
}

void handleShootingArrow(sfRenderWindow* gameWindow, Player* player, Arrow* arrow, sfSprite* playerSprite, sfSprite* arrowSprite, sfSprite* cursorSprite)
{
    if (player->IsAlive == 1)
    {
         if (arrow->IsShot == 0)
         {
            arrow->IsShot = 1;
            sfVector2i mousePosition = sfMouse_getPositionRenderWindow(gameWindow);
            sfVector2f playerPosition = sfSprite_getPosition(playerSprite);
            sfVector2f direction = { mousePosition.x - playerPosition.x, mousePosition.y - playerPosition.y };
            float length = sqrt(direction.x * direction.x + direction.y * direction.y); 
            sfVector2f normalizedDirection = { direction.x / length, direction.y / length }; 
            sfVector2f arrowVelocity = { normalizedDirection.x * ARROW_SPEED, normalizedDirection.y * ARROW_SPEED }; 
            arrow->x = playerPosition.x;
            arrow->y = playerPosition.y;
            sfVector2f arrowPosition = { arrow->x, arrow->y};
            sfSprite_setPosition(arrowSprite, arrowPosition);
         }
        
    }
#pragma region Arrow Collisions Cursor
    sfFloatRect arrowBounds = sfSprite_getGlobalBounds(arrowSprite);
    sfFloatRect cursorBounds = sfSprite_getGlobalBounds(cursorSprite);
    if (sfFloatRect_intersects(&arrowBounds, &cursorBounds, NULL))
    {
        arrow->IsShot = 0;
    }
#pragma endregion
}