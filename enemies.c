#include <SFML/Graphics.h>
#include "player.h"
#include "enemies.h"

float random(int min, int max)
{
	return (float)(rand() % (max + 1 - min)) + min;
}

Slime slimeInit() 
{
	Slime slime;
	slime.x = rand() % 1920;
	slime.y = rand() % 1080;
	slime.speed = 3;
	slime.size = 2;
	slime.hitCount = 2;
	slime.rotation = random(0, 360);  
	return slime;
}

sfSprite* slimeSpriteInit(sfRenderWindow* gameWindow, Slime* slime)
{
#pragma region Sprite Creation
	sfTexture* slimeTexture = sfTexture_createFromFile("sprites/slimemc.png", NULL);
	sfSprite* slimeSprite = sfSprite_create();
	sfSprite_setTexture(slimeSprite, slimeTexture, sfFalse);
	sfVector2f sOrigin = { 32.0f, 32.0f };
	sfSprite_setOrigin(slimeSprite, sOrigin);
	sfVector2f slimeCord = { slime->x, slime->y };
	sfSprite_setPosition(slimeSprite, slimeCord);
#pragma endregion

	if (slime->size == 2) 
	{
		sfSprite_setScale(slimeSprite, (sfVector2f) { 2.2f, 2.2f });
		return slimeSprite; 
	}
	else if (slime->size == 1)
	{
		sfSprite_setScale(slimeSprite, (sfVector2f) { 1.4f, 1.4f });
		return slimeSprite; 
	}
}

void handleSlimeMovementsCollisions(sfRenderWindow* gameWindow, Slime* slime, Player* player, Arrow* arrow, sfSprite* playerSprite, sfSprite* arrowSprite, sfSprite* slimeSprite)
{
	sfVector2f slimePosition = sfSprite_getPosition(slimeSprite);

#pragma region ArrowCollisions
	sfFloatRect arrowBounds = sfSprite_getGlobalBounds(arrowSprite);
	sfFloatRect slimeBounds = sfSprite_getGlobalBounds(slimeSprite);

	if (slime->hitCount > 0)
	{
		if (sfFloatRect_intersects(&arrowBounds, &slimeBounds, NULL))
		{
			arrow->IsShot = 0;
			slime->hitCount--;

			if (slime->hitCount == 0)
			{
				slime->size = 0;
				player->score = player->score + 200;
			}
			else
			{
				sfVector2f newScale;
				sfVector2f newPosition = sfSprite_getPosition(slimeSprite);

				if (slime->hitCount == 2)
				{
					newScale = (sfVector2f){ 2.2f, 2.2f };
					slime->speed = 6;
				}
				else if (slime->hitCount == 1)
				{
					newScale = (sfVector2f){ 1.4f, 1.4f };
					slime->speed = 8;
				}
				sfSprite_setScale(slimeSprite, newScale);
				sfSprite_setPosition(slimeSprite, newPosition);
			}
		}
	}
#pragma endregion

	if (slime->size > 0)
	{
		if (rand() % 150 == 0)
		{
			slime->rotation = random(0, 360);
		}

		slimePosition.x += cosf(slime->rotation * 3.14159265 / 180) * slime->speed;
		slimePosition.y += sinf(slime->rotation * 3.14159265 / 180) * slime->speed;

#pragma region SlimeWarp
	if (slimePosition.y < -30)
	{
		slimePosition.y = 1070;
	}
	if (slimePosition.y > 1120)
	{
		slimePosition.y = 0;
	}
	if (slimePosition.x < -30)
	{
		slimePosition.x = 1910;
	}
	if (slimePosition.x > 1960)
	{
		slimePosition.x = 0;
	}

	sfSprite_setPosition(slimeSprite, slimePosition);
#pragma endregion

#pragma region Player Collisions
	sfFloatRect playerBounds = sfSprite_getGlobalBounds(playerSprite);
	if (sfFloatRect_intersects(&playerBounds, &slimeBounds, NULL))
	{
		player->IsAlive = 0;
	}
#pragma endregion
	}
}