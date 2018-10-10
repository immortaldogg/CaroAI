#include "SelectModeScene.h"
#include "GameplayScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* SelectMode::createScene()
{
    return SelectMode::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool SelectMode::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// play vs AI mode + play vs human (coming soon)
	Vector<MenuItem*> MenuItems;
	auto mode_vsai = MenuItemLabel::create(LabelTTF::create("Play vs AI", "fonts/Marker Felt.ttf", 31),
		[&](Ref* sender) {
		UserDefault::getInstance()->setIntegerForKey("Mode", 1);
		Director::getInstance()->replaceScene(Gameplay::createScene());
	});

	auto mode_vshuman = MenuItemLabel::create(LabelTTF::create("Play vs Human", "fonts/Marker Felt.ttf", 31),
		[&](Ref* sender) {
		UserDefault::getInstance()->setIntegerForKey("Mode", 2);
		Director::getInstance()->replaceScene(Gameplay::createScene());
	});
	MenuItems.pushBack(mode_vsai);
	MenuItems.pushBack(mode_vshuman);

	auto menu = Menu::createWithArray(MenuItems);
	menu->alignItemsVerticallyWithPadding(14.0);
	this->addChild(menu, 2);

	return true;
}