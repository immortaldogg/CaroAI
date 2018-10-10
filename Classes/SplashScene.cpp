#include "SplashScene.h"
#include "SimpleAudioEngine.h"

#include "HomeScene.h"

USING_NS_CC;

Scene* Splash::createScene()
{
    return Splash::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Splash::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add logo

	auto pre_logo = Sprite::create("milean_pre_logo_small.png");
	if (pre_logo == nullptr) {
		problemLoading("Pre logo loading problem");
	}
	else {
		pre_logo->setAnchorPoint(Vec2(0.5, 0.5));
		pre_logo->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));		
		this->addChild(pre_logo);
	}

    //auto label = Label::createWithTTF("Milean Games", "fonts/Marker Felt.ttf", 24);
    //if (label == nullptr)
    //{
    //    problemLoading("'fonts/Marker Felt.ttf'");
    //}
    //else
    //{
    //    // position the label on the center of the screen
    //    label->setPosition(Vec2(origin.x + visibleSize.width/2,
    //                            origin.y + visibleSize.height/2));

    //    // add the label as a child to this layer
    //    this->addChild(label, 0);
    //}

    return true;
}

void Splash::onEnter() {
	Scene::onEnter();

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto logo = Sprite::create("milean_logo_small.png");
	if (logo == nullptr) {
		problemLoading("Logo loading problem");
	}
	else {
		logo->setAnchorPoint(Vec2(0.5, 0.5));
		logo->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
		logo->setOpacity(0);
		this->addChild(logo);

		auto fadeIn = FadeIn::create(1.3f);
		logo->runAction(fadeIn);
	}

	this->scheduleOnce(schedule_selector(Splash::finishSplash), 1.5f);
}

void Splash::finishSplash(float dt) {
	Director::getInstance()->replaceScene(TransitionFade::create(1, Home::createScene()));
}