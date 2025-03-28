#pragma once
struct Vertex
{
	float x, y, z;
	float r, g, b;
};
struct textureVertex
{
	textureVertex(float _x, float _y, float _z, float _r, float _g, float _b, float _tX, float _tY)
		: x(_x), y(_y), z(_z), r(_r), g(_g), b(_b), tX(_tX), tY(_tY)
	{

	}
	float x, y, z;
	float r, g, b;
	float tX, tY;
};
struct HUDvertex
{
	HUDvertex(float _x, float _y, float _tX, float _tY)
		: x(_x), y(_y), tX(_tX), tY(_tY)
	{

	}
	float x, y;
	float tX, tY;
};
struct Color
{
	float r, g, b;
};

struct Vector2f
{
	float x, y;
	Vector2f operator+(Vector2f const vec)
	{
		Vector2f ret = { x,y };
		ret.x += vec.x;
		ret.y += vec.y;
		return ret;
	}
};
const Color GREEN = { 0.0f, 1.0f, 0.0f };
const Color RED = { 1.0f, 0.0f, 0.0f };
const Color BLUE = { 0.0f, 0.0f, 1.0f };
const Color BROWN = { 0.42f, 0.31f, 0.161f };