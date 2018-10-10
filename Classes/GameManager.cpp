#include "GameManager.h"
#include "SimpleAudioEngine.h"


USING_NS_CC;

Node* GameManager::createNode()
{
    return GameManager::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool GameManager::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Node::init() )
    {
        return false;
    }

	// init starting value of the game state	
	this->turn = 0;
	this->isOver = false;

    return true;
}

void GameManager::make_play() {

}