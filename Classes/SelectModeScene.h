#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class SelectMode : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
        
    // implement the "static create()" method manually
    CREATE_FUNC(SelectMode);
};

#endif // __HELLOWORLD_SCENE_H__
