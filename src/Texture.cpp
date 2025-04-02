#include "Texture.h"

Vector2f getBlockTexture(int _b, uint16_t _f)
{
	if (_b == 1) //	GRASS
	{
		if (_f == 4) return grassTopPos;
		else if (_f == 5) return dirtPos;
		else return grassSidePos;
	}
	if (_b == 2) return dirtPos; // DIRT
	if (_b == 3) return stonePos; // STONE
	if (_b == 4) return tallGrassPos;
	if (_b == 5) return rosePos;
	if (_b == 6) return blueflowerPos;
	if (_b == 7)
	{
		if (_f == 4 || _f == 5)
		{
			return woodInside;
		}
		else return woodSide;
	}
	if (_b == 8) return leaves;
	if (_b == 9) return purpleWool;
	if (_b == 10) return pinkWool;
	if (_b == 11) return redWool;
	if (_b == 12) return blueWool;
	if (_b == 13) return yellowWool;
	if (_b == 14) return cyanWool;
	if (_b == 15) return grayWool;
	if (_b == 16) return magentaWool;
	if (_b == 17) return woodenPlanks;
	return { 0,0 };
}
