#ifndef __CELL_H__
#define __CELL_H__

#include "cocos2d.h"

USING_NS_CC;

class Cell : public cocos2d::Sprite
{
public:

	int _coorX;
	int _coorY;
	int _color;

    Cell* create(int coorX, int coorY, int color);
};

#endif // __CELL_H__
