#pragma once
#include <glad/glad.h>
#include <memory>
#include "structs.h"
#include "stb_image.h"
#include <iostream>
const float tileSize = 0.03125f;

const Vector2f grassTopPos = { tileSize, 1-tileSize };
const Vector2f grassSidePos = { 0.0f, 1-tileSize };
const Vector2f dirtPos = { 2*tileSize, 1-tileSize };
const Vector2f stonePos = { 3*tileSize, 1-tileSize };
const Vector2f tallGrassPos = { 4*tileSize, 1-tileSize };
const Vector2f rosePos = { 5 * tileSize, 1 - tileSize };
const Vector2f blueflowerPos = { 6 * tileSize, 1 - tileSize };
const Vector2f woodSide = { 7 * tileSize, 1 - tileSize };
const Vector2f woodInside = { 8 * tileSize, 1 - tileSize };
const Vector2f leaves = { 9 * tileSize, 1 - tileSize };

const Vector2f purpleWool = { 10 * tileSize, 1 - tileSize };
const Vector2f pinkWool = { 11 * tileSize, 1 - tileSize };
const Vector2f redWool = { 12 * tileSize, 1 - tileSize };
const Vector2f blueWool = { 13 * tileSize, 1 - tileSize };
const Vector2f yellowWool = { 14 * tileSize, 1 - tileSize };
const Vector2f cyanWool = { 15 * tileSize, 1 - tileSize };
const Vector2f grayWool = { 16 * tileSize, 1 - tileSize };
const Vector2f magentaWool = { 17 * tileSize, 1 - tileSize };

const Vector2f woodenPlanks = { 18 * tileSize, 1 - tileSize };

const Vector2f ironOre = { 19 * tileSize , 1 - tileSize };
const Vector2f diamondOre = { 20 * tileSize , 1 - tileSize };
const Vector2f goldOre = { 21 * tileSize , 1 - tileSize };
const Vector2f pinkOre = { 22 * tileSize , 1 - tileSize };

Vector2f getBlockTexture(int _b, uint16_t _f);


class TextureAtlas
{
public:
	TextureAtlas()
	{

	}
	TextureAtlas(const char* filePath)
	{
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &ID);
		
		glBindTexture(GL_TEXTURE_2D, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load(filePath, &width, &height, &numColCh, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	GLuint ID;

	int width, height, numColCh;

	
};

class Texture
{
public:
	Texture()
	{

	}
	Texture(const char* filePath)
	{
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &ID);

		glBindTexture(GL_TEXTURE_2D, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_set_flip_vertically_on_load(true);

		unsigned char* data = stbi_load(filePath, &width, &height, &numColCh, 0);

		if (data)
		{

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	GLuint ID;
	int width, height, numColCh;
};
class TextureManager
{
public:
	TextureManager()
	{
	}
	void Init()
	{
		crosshairTexture = Texture("src/texture/crosshair.png");
		atlas1 = TextureAtlas("src/texture/ta1.png");
		titleScreen = Texture("src/texture/title.png");
		menuButton = Texture("src/texture/button.png");
		bgDirt = Texture("src/texture/dirt.png");
		wmanager = Texture("src/texture/worldmgr.png");
		wsave = Texture("src/texture/save.png");
	}
	TextureAtlas atlas1;
	Texture crosshairTexture;
	Texture titleScreen;
	Texture menuButton;
	Texture bgDirt;
	Texture wmanager;
	Texture wsave;
};

