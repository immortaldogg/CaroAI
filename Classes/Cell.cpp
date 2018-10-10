#include "Cell.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include <cmath>

USING_NS_CC;

Cell* Cell::create(int coorX, int coorY, int color)
{
	_coorX = coorX;
	_coorY = coorY;
	_color = color;
}
