#include "cocos2d.h"
#include "CCEGLView.h"
#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "LoadingScene.h"
#include "MainMenuScene.h"
#include "GameMapLayer.h"
#include "Options.h"
#include "cGlobal.h"
#include "MMPInterface.h"
#include "utils.h"
#include "AnalyticX.h"
#include "IAP.h"
#include "cFacebook.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace CocosDenshion;
using namespace MarketingPlatform;
using namespace Core;


USING_NS_CC;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::end();
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());

    // turn on display FPS
    pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0f / 60.0f);
    
    
    Globals* globals = new Globals();
    cFacebook* facebook = new cFacebook();

    
    GlobalsPtr->iceCreamScene = Invalid;
    
    CCFileUtils::sharedFileUtils()->addSearchPath("common");
    
    if (IPAD)
    {
        CCFileUtils::sharedFileUtils()->addSearchPath("ipad");
    }
    else
    {
        CCFileUtils::sharedFileUtils()->addSearchPath("iphone");
    }
    
    CCTexture2D::PVRImagesHavePremultipliedAlpha(false);
    
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/1.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/big_explosion.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/bomb_explosion.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/crystal_created.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/delicious.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/divine.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_1.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_3.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_4.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_5.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_6.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_7.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_8.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/dumb.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/footstep.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/hammer_impact.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/ice_crashed.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/item_fall_out.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/item_fall.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/landing_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/landing_3.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/landing_4.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/level_completed.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/line_explosion.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/level_failed.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/move.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/penguins_loop.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_1.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_3.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_4.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_5.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_6.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_7.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_8.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_9.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_10.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_11.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_12.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_13.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/rozhok_created.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/star_1.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/square_deleted_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/star_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/star_3.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/sweet.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/tasty.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/transformation.mp3");
	
    // create a scene. it's an autorelease object
    CCScene *pScene = LoadingScene::scene();
    
	//CCScene *pScene = GameMapLayer::scene();
    // run
    pDirector->runWithScene(pScene);
    
//    NSString *idfaString = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
//    MMPLib* mmp = new MMPLib("5f1e4b81492175dcef20d9d18a5d7014", [idfaString UTF8String]);
    AnalyticX::flurrySetAppVersion(version());
    AnalyticX::flurrySetDebugLogEnabled(true);
    AnalyticX::flurrySetSessionContinueSeconds(143);
    AnalyticX::flurrySetSecureTransportEnabled(false);
    AnalyticX::flurryStartSession("ZSFQWHCR6BVPZVZY9XFM");

    string appKey = "5f1e4b81492175dcef20d9d18a5d7014";
    string idfaKey = string(idfa());
    
    CCLOG("init serverProxy ");
    _serverProxy = new AngryPets::GlobalWorld::Network::ServerProxy();
    _serverProxy->Init();
    
    MMPInterface* mmp = new MMPInterface();
    MMPInterface::Instance()->Init(appKey, idfaKey);
    MMPInterface::Instance()->StartSession();
    
    
    
    vector<string> pi;
    pi.push_back("com.destiny.icecreamadventure.5lives");
    pi.push_back("com.destiny.icecreamadventure.5moves");
    pi.push_back("com.destiny.icecreamadventure.hammer");
    pi.push_back("com.destiny.icecreamadventure.penguins");
    pi.push_back("com.destiny.icecreamadventure.stripedandbomb");
    pi.push_back("com.destiny.icecreamadventure.superelements");
    pi.push_back("com.destiny.icecreamadventure.unlocklevelpack");
    
    
    IAP::sharedInstance().initWithProductIdentifiers(pi);
    IAP::sharedInstance().provideContentForProductIdentifier = bind(&AppDelegate::provideContentForProductIdentifier, this, std::placeholders::_1);
    IAP::sharedInstance().productsRecieved = bind(&AppDelegate::productReceived, this);
    IAP::sharedInstance().cancelProductPayment = bind(&AppDelegate::cancelPayment, this);
    IAP::sharedInstance().requestProducts();
 
    prevHeight = WINSIZE.height;
    prevWidth = WINSIZE.width;
    return true;
}

void AppDelegate::cancelPayment()
{
    if (GlobalsPtr->iceCreamScene == Game)
    {
        GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
        layer->cancelPayment();
    }
}

void AppDelegate::productReceived()
{
    int a = 0;
}

void AppDelegate::provideContentForProductIdentifier(string productIdentifier)
{
    if(productIdentifier ==  "com.destiny.icecreamadventure.5lives")
    {
        FacebookPtr->boughtItem("5 lives");
        OptionsPtr->setLifeCount(OptionsPtr->getLifeCount() + 5);
        OptionsPtr->setFirstPayLife();
    }
    else if(productIdentifier ==  "com.destiny.icecreamadventure.5moves")
    {
        FacebookPtr->boughtItem("5 moves");
        GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
        layer->addMovesInApp();
        OptionsPtr->setFirstPayMove();
    }
    else if(productIdentifier ==  "com.destiny.icecreamadventure.hammer")
    {
        FacebookPtr->boughtItem("Hammer");
        GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
        layer->addHammer();
        OptionsPtr->setFirstPayHammer();
    }
    else if(productIdentifier ==  "com.destiny.icecreamadventure.penguins")
    {
        FacebookPtr->boughtItem("Penguins");
        OptionsPtr->setFishCount(OptionsPtr->getFishCount() + 3);
        if (GlobalsPtr->iceCreamScene == Game)
        {
            GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
            layer->updateBoosters();
        }
        else if (GlobalsPtr->iceCreamScene == Map)
        {
            GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
            layer->updateMenuBooster();
        }
        OptionsPtr->setFirstPayFish();
    }
    else if(productIdentifier ==  "com.destiny.icecreamadventure.stripedandbomb")
    {
        FacebookPtr->boughtItem("Striped and Bomb");
        OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 3);
        if (GlobalsPtr->iceCreamScene == Game)
        {
            GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
            layer->updateBoosters();
        }
        else if (GlobalsPtr->iceCreamScene == Map)
        {
            GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
            layer->updateMenuBooster();
        }
        OptionsPtr->setFirstPayBomb();
    }
    else if(productIdentifier ==  "com.destiny.icecreamadventure.superelements")
    {
        FacebookPtr->boughtItem("Super Elements");
        OptionsPtr->setCrystalCount(OptionsPtr->getCrystalCOunt() + 3);
        if (GlobalsPtr->iceCreamScene == Game)
        {
            GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
            layer->updateBoosters();
        }
        else if (GlobalsPtr->iceCreamScene == Map)
        {
            GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
            layer->updateMenuBooster();
        }
        OptionsPtr->setFirstPayCrystal();
    }
    else if(productIdentifier ==  "com.destiny.icecreamadventure.unlocklevelpack")
    {
        GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
        layer->updateMenuBooster();
    }
    OptionsPtr->save();
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->stopAnimation();
    CCDirector::sharedDirector()->pause();
//    SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
//    SimpleAudioEngine::sharedEngine()->stopAllEffects();
    MMPInterface::Instance()->EndSession();
    AnalyticX::flurryEndSession();
    
    if (!OptionsPtr->isNotification())
    {
        removeAllNotification();
        return;
    }
    
    struct timeval now;
    gettimeofday(&now, NULL);
    long second = now.tv_sec - OptionsPtr->getLastTime();
    
    if (second < 0)
        OptionsPtr->setLifeCount(5);
    
    while (second >= 1800)
    {
        if (OptionsPtr->getLifeCount() < 5)
            OptionsPtr->setLifeCount(OptionsPtr->getLifeCount() + 1);
        OptionsPtr->setLastTime(OptionsPtr->getLastTime() + 1800);
        second = second - 1800;
    }
    
    if (OptionsPtr->getLifeCount() < 5)
        sendlocalNotification((float)second + 1800.0f*(4 - OptionsPtr->getLifeCount()));
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();
    CCDirector::sharedDirector()->resume();
//    SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
//    SimpleAudioEngine::sharedEngine()->resumeAllEffects();
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/1.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/big_explosion.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/bomb_explosion.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/crystal_created.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/delicious.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/divine.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_1.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_3.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_4.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_5.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_6.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_7.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/drop_8.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/dumb.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/footstep.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/hammer_impact.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/ice_crashed.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/item_fall_out.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/item_fall.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/landing_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/landing_3.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/landing_4.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/level_completed.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/line_explosion.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/level_failed.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/move.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/penguins_loop.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_1.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_3.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_4.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_5.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_6.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_7.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_8.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_9.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_10.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_11.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_12.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/pop_13.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/rozhok_created.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/star_1.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/square_deleted_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/star_2.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/star_3.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/sweet.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/tasty.mp3");
    SimpleAudioEngine::sharedEngine()->preloadEffect("sound/transformation.mp3");
    
    
    MMPInterface::Instance()->StartSession();
    AnalyticX::flurryStartSession("ZSFQWHCR6BVPZVZY9XFM");
}

void AppDelegate::applicationWillChangeOrientation()
{
    if (prevHeight != WINSIZE.height && prevWidth != WINSIZE.width)
    {
        if (CCDirector::sharedDirector()->getRunningScene())
        {
            if (CCDirector::sharedDirector()->getRunningScene()->getChildrenCount() > 0)
            {
                CCLayer* layer = ((CCLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                layer->changeOrientation();
            }
        }
        prevHeight = WINSIZE.height;
        prevWidth = WINSIZE.width;
    }
}
