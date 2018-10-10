#include <iostream>
#include "HomeScene.h"
#include "SelectModeScene.h"
#include "SimpleAudioEngine.h"
#include "GameplayScene.h"

USING_NS_CC;
using namespace std;

auto startCallBack = []() { cout << "Start pressed"; };

Scene* Home::createScene()
{
    return Home::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Home::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// add game title
	auto game_title = Sprite::create("GameLogo2.png");
	if (game_title == nullptr) {
		problemLoading("GameLogo2.png");
	}
	else {
		game_title->setAnchorPoint(Vec2(0.5f, 0.5f));
		game_title->setPosition(Vec2(origin.x + visibleSize.width / 2,
			origin.y + visibleSize.height * 5 / 6));

		this->addChild(game_title);
	}
		
	// menu items
	Vector<MenuItem*> MenuItems;
	auto menu_play = MenuItemImage::create("PlayVsComNormal.png", "PlayVsComSelected.png", 
		[&](Ref* sender) {
		UserDefault::getInstance()->setIntegerForKey("Mode", 1);
		Director::getInstance()->replaceScene(Gameplay::createScene());
	});

	auto menu_option = MenuItemImage::create("2PlayerNormal.png", "2PlayerSelected.png",
		[&](Ref* sender) {
		UserDefault::getInstance()->setIntegerForKey("Mode", 2);
		Director::getInstance()->replaceScene(Gameplay::createScene());
	});

	auto menu_quit = MenuItemLabel::create(LabelTTF::create("Quit", "fonts/Marker Felt.ttf", 31),
		[&](Ref* sender) {
		CCLOG("Quit pressed");
	});

	MenuItems.pushBack(menu_play);
	MenuItems.pushBack(menu_option);
	MenuItems.pushBack(menu_quit);

	auto menu = Menu::createWithArray(MenuItems);
	menu->setAnchorPoint(Vec2(0.0, 0.0));
	menu->setScale(0.5);
	menu->alignItemsVerticallyWithPadding(14.0);
	this->addChild(menu,2);

	// background image
	auto bg = Sprite::create("bg_brown.jpg");
	bg->setAnchorPoint(Vec2(0.5, 0.5));
	bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	bg->setOpacity(125);

	this->addChild(bg, -1);

    return true;
}