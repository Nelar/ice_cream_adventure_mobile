#include "MainMenuScene.h"
#include "GameMapLayer.h"
#include "cGlobal.h"
#include "SimpleAudioEngine.h"
#include "Options.h"
#include "cComixScene.h"
#include "cFacebook.h"
#include "utils.h"
#include "CCLocalizedString.h"

#include "cSocialMenu.h"
#include "IAP.h"
#include "nMMP.h"


using namespace CocosDenshion;
using namespace cocos2d;

string fontStd = "Avenir-Heavy";

CCScene* MainMenuScene::scene()
{
	CCScene * scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(! scene);
		MainMenuScene *layer = MainMenuScene::create();
		CC_BREAK_IF(! layer);
		scene->addChild(layer);
	} while (0);

	return scene;
}

MainMenuScene::~MainMenuScene()
{
    this->stopAllActions();
    this->unscheduleAllSelectors();
    this->removeAllChildrenWithCleanup(true);
}

void MainMenuScene::facebookButtonHide()
{
    facebook->setVisible(false);
    faceConnect->setVisible(false);
    faceConnect->setCascadeOpacityEnabled(true);
    faceConnect->setEnabled(false);
}

void MainMenuScene::gameCenterButtonHide()
{
    if (gameCenter == NULL)
        return;
    gameCenter->setVisible(false);
    gameCenter->setVisible(false);
    gameCenter->setCascadeOpacityEnabled(true);
    gameCenter->setEnabled(false);
}

void MainMenuScene::gameCenterButtonShow()
{
    if (gameCenter == NULL)
        return;
    gameCenter->setVisible(true);
    gameCenter->setVisible(true);
    gameCenter->setEnabled(true);
}

bool MainMenuScene::init()
{
    CCDirector::sharedDirector()->setAnimationInterval(1.0f / 60.0f);
	if (!CCLayer::init())
		return false;
    
    GlobalsPtr->iceCreamScene = Menu;
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("common0.plist", CCTextureCache::sharedTextureCache()->addPVRImage("common0.pvr.ccz"));
    if (IPAD)
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("common1.plist", CCTextureCache::sharedTextureCache()->addPVRImage("common1.pvr.ccz"));
    else
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("common1.plist", CCTextureCache::sharedTextureCache()->addImage("common1.png"));
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("mainMenu.plist", CCTextureCache::sharedTextureCache()->addPVRImage("mainMenu.pvr.ccz"));
    
    CCTextureCache::reloadAllTextures();
    
    vector<sRequestData> requests = OptionsPtr->appRequests;
    
    isSound = OptionsPtr->useSounds();
	isMusic = OptionsPtr->useMusic();
    
    if (!isSound)
		SimpleAudioEngine::sharedEngine()->setEffectsVolume(0.0f);
	else
		SimpleAudioEngine::sharedEngine()->setEffectsVolume(1.0f);
    
	if (!isMusic)
		SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.0f);
	else
		SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(1.0f);

	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sound/intro_loop_1.mp3", true);
    
    social = SocialLayer::create();
    this->addChild(social, 300);
    
    moreGamesLayer = MoreGamesLayer::create();
    moreGamesLayer->setPosition(ccp(0.0f, 0.0f));
    this->addChild(moreGamesLayer, 301);
    

	label = NULL;
	tutorial = NULL;
	notif_1 = true;
    isLock = false;
    isPopup = false;
    isTutorial = false;

	helpNum = 0;
	tutorialNum = 0;

	isBoy = false;
	isSetting = false;
    
    popupLayer = PopupLayer::create();
    this->addChild(popupLayer, 100);

	this->setTouchEnabled(true);

    if (LANDSCAPE)
        background = CCSprite::createWithSpriteFrameName("mainMenu/mainMenuBackgroundLandscape.jpg");
    else
        background = CCSprite::createWithSpriteFrameName("mainMenu/mainMenuBackground.jpg");
    
    if (LANDSCAPE)
    {
        if (WINSIZE.height == 640)
            popupScale = 0.85f;
        else
            popupScale = 1.0f;
    }
    else
    {
        if (WINSIZE.width == 640)
            popupScale = 0.85f;
        else
            popupScale = 1.0f;
    }

	this->addChild(background);
    
    
    background->setAnchorPoint(ccp(0.0f, 1.0f));
    background->setPosition(ccp(0.0f, WINSIZE.height));


	logo = CCSprite::createWithSpriteFrameName("mainMenu/logo.png");
	logo->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 1.9f, CCDirector::sharedDirector()->getWinSize().height / 1.4f));
	this->addChild(logo, 2);


    iceBlob = CCSprite::createWithSpriteFrameName("mainMenu/iceBlotLandscape.png");
	iceBlob->setPosition(ccp(0.0f, 0.0f));
	iceBlob->setAnchorPoint(ccp(0.0f, 0.0f));
	this->addChild(iceBlob, 2);

#ifdef NEW_ART
    settingBlob = CCSprite::createWithSpriteFrameName("common/panel.png");
	settingBlob->setPosition(ccp(WINSIZE.width / 2.0f, WINSIZE.height / 2.0f));
    
    CCSprite* settingBlobCogwheel = CCSprite::create("updateArt/button.png");
    settingBlob->addChild(settingBlobCogwheel);
    settingBlobCogwheel->setPosition(ccp(settingBlob->getContentSize().width/5.0f, settingBlob->getContentSize().height/10.0f*8.5f));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("MENU", NULL), FONT_COMMON, FONT_SIZE_86);
    labelTTF->setColor(IceCreamBrown2);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    settingBlob->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/10.0f*8.5f));
    
    if (LANDSCAPE)
    {
        if (IPHONE_4 || IPHONE_5)
            settingBlob->setScale(0.63f);
        else
            settingBlob->setScale(0.75f);
    }
    else
    {
        settingBlob->setScale(1.0f);
    }
    
#else
	settingBlob = CCSprite::createWithSpriteFrameName("common/settingBlob.png");
	settingBlob->setPosition(ccp(settingBlob->getContentSize().width / 2.0f, settingBlob->getContentSize().height / 2.0f));
#endif
	this->addChild(settingBlob, 100);
    
    CCSprite* playSpriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* playSpriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
	playSpriteSelected->setScale(0.9f);
	playSpriteSelected->setAnchorPoint(ccp(0.5f, 0.5f));
    playSpriteSelected->setColor(ccGRAY);
	playSpriteSelected->setPosition(ccp(playSpriteSelected->getContentSize().width/20.0f, playSpriteSelected->getContentSize().height/20.0f));
	play = CCMenuItemSprite::create(playSpriteNormal, playSpriteSelected, this, menu_selector(MainMenuScene::menuPlayCallback));
	play->setPosition(ccp(0.0f, CCDirector::sharedDirector()->getWinSize().height / 1.4f - CCDirector::sharedDirector()->getWinSize().height/2.0f - logo->getContentSize().height/2.0f/* - play->getContentSize().height/2.0f*/));
	play->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("PLAY", NULL), FONT_COMMON, FONT_SIZE_86);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    play->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

	CCSprite* facebookSpriteNormal = CCSprite::createWithSpriteFrameName("common/facebookButtonMainMenu.png");
	CCSprite* facebookSpriteSelected = CCSprite::createWithSpriteFrameName("common/facebookButtonMainMenu.png");
	facebookSpriteSelected->setScale(0.9f);
	facebookSpriteSelected->setAnchorPoint(ccp(0.5f, 0.5f));
    facebookSpriteSelected->setColor(ccGRAY);
	facebookSpriteSelected->setPosition(ccp(facebookSpriteSelected->getContentSize().width/20.0f, facebookSpriteSelected->getContentSize().height/20.0f));
	facebook = CCMenuItemSprite::create(facebookSpriteNormal, facebookSpriteSelected, this, menu_selector(MainMenuScene::menuFacebookCallback));
	facebook->setPosition(ccp(play->getPositionX(), play->getPositionY() - facebook->getContentSize().height*1.1f));
	facebook->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("CONNECT", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    facebook->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.65f, labelTTF->getParent()->getContentSize().height/2.0f));
    if (CCApplication::sharedApplication()->getCurrentLanguage() == kLanguageSpanish)
        labelTTF->setFontSize(FONT_SIZE_48);
        
    
    CCSprite* gameCenterSpriteNormal = CCSprite::createWithSpriteFrameName("common/greenButton.png");
	CCSprite* gameCenterSpriteSelected = CCSprite::createWithSpriteFrameName("common/greenButton.png");
	gameCenterSpriteSelected->setScale(0.9f);
	gameCenterSpriteSelected->setAnchorPoint(ccp(0.5f, 0.5f));
    gameCenterSpriteSelected->setColor(ccGRAY);
	gameCenterSpriteSelected->setPosition(ccp(gameCenterSpriteSelected->getContentSize().width/20.0f, gameCenterSpriteSelected->getContentSize().height/20.0f));
	gameCenter = CCMenuItemSprite::create(gameCenterSpriteNormal, gameCenterSpriteSelected, this, menu_selector(MainMenuScene::menuGameCenterCallback));
	gameCenter->setPosition(ccp(play->getPositionX(), play->getPositionY() - gameCenter->getContentSize().height*1.9f));
    CCSprite* gcIcon = CCSprite::create("gameCenter.png");
    gcIcon->setScale(0.5f);
    gcIcon->setPosition(ccp(gameCenter->getContentSize().width/4.0f, gameCenter->getContentSize().height/2.0f));
    gameCenter->addChild(gcIcon);
    gameCenter->setScale(0.9f);
	gameCenter->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 0.9f, 0.8f), CCScaleTo::create(0.5f, 0.85f, 0.85f))));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("CONNECT", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    gameCenter->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.65f, labelTTF->getParent()->getContentSize().height/2.0f));
    if (CCApplication::sharedApplication()->getCurrentLanguage() == kLanguageSpanish)
        labelTTF->setFontSize(FONT_SIZE_48);

#ifdef NEW_ART
    CCSprite* settingSpriteNormal = CCSprite::create("updateArt/button.png");
	CCSprite* settingSpriteSelected = CCSprite::create("updateArt/button.png");
    settingSpriteSelected->setAnchorPoint(ccp(0.5f, 0.5f));
    settingSpriteSelected->setColor(ccGRAY);
    settingSpriteSelected->setScale(0.9f);
    settingSpriteSelected->setPosition(ccp(settingSpriteSelected->getContentSize().width/20.0f, settingSpriteSelected->getContentSize().height/20.0f));

	setting = CCMenuItemSprite::create(settingSpriteNormal, settingSpriteSelected, this, menu_selector(MainMenuScene::menuSettingCallback));
	setting->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width /2.0f + setting->getContentSize().width / 2.0f,
                             -CCDirector::sharedDirector()->getWinSize().height /2.0f + setting->getContentSize().height / 2.0f));
#else
	CCSprite* settingSpriteNormal = CCSprite::create("gameMap/setting.png");
	CCSprite* settingSpriteSelected = CCSprite::create("gameMap/setting.png");
    settingSpriteSelected->setScale(0.9f);
	setting = CCMenuItemSprite::create(settingSpriteNormal, settingSpriteSelected, this, menu_selector(MainMenuScene::menuSettingCallback));
	setting->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width /2.0f + setting->getContentSize().width / 2.6f,
                             -CCDirector::sharedDirector()->getWinSize().height /2.0f + setting->getContentSize().height / 2.6f));

#endif
	   
#ifdef NEW_ART
    CCSprite *inviteButton = CCSprite::create("buttonSetting.png");
    inviteButton->setCascadeOpacityEnabled(true);
    
    CCSprite *inviteButtonDown = CCSprite::create("buttonSetting.png");
    inviteButtonDown->setColor(ccGRAY);
    inviteButtonDown->setCascadeOpacityEnabled(true);
    
    CCSprite* mark = CCSprite::create("inviteFriends.png");
    CCSprite* markDown = CCSprite::create("inviteFriends.png");
    mark->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    markDown->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    inviteButton->addChild(mark);
    inviteButtonDown->addChild(markDown);

    
	invite = CCMenuItemSprite::create(inviteButton, inviteButtonDown, this, menu_selector(MainMenuScene::inviteCallback));
    if (LANDSCAPE)
        invite->setPosition(settingBlob->getPositionX() - WINSIZE.width/2.0f, settingBlob->getPositionY()*settingBlob->getScale() - WINSIZE.height/2.0f + invite->getContentSize().height*2.5f*settingBlob->getScale());
    else
        invite->setPosition(settingBlob->getPositionX() - WINSIZE.width/2.0f, settingBlob->getPositionY()*settingBlob->getScale() - WINSIZE.height/2.0f + invite->getContentSize().height*1.2f*settingBlob->getScale());
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("INVITE_FRIENDS_BUTTON", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    invite->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
#else
    CCSprite* inviteNormal = CCSprite::create("inviteFriends.png");
	CCSprite* inviteSelected = CCSprite::create("inviteFriends.png");
    inviteSelected->setColor(ccGRAY);
    
    inviteSelected->setScale(0.9f);
	inviteSelected->setPosition(ccp(inviteSelected->getContentSize().width/20.0f, inviteSelected->getContentSize().height/20.0f));
	invite = CCMenuItemSprite::create(inviteNormal, inviteSelected, this, menu_selector(MainMenuScene::inviteCallback));
	invite->setPosition(ccp(invite->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f - settingBlob->getContentSize().width / 20.0f,
                            -invite->getContentSize().height/1.3f - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height/0.99f));
#endif
    
    CCSprite* moreGamesNormal = CCSprite::create("moreGames.png");
	CCSprite* moreGamesSelected = CCSprite::create("moreGames.png");
    moreGamesSelected->setColor(ccGRAY);
    
    moreGamesSelected->setScale(0.9f);
	moreGamesSelected->setPosition(ccp(moreGamesSelected->getContentSize().width/20.0f, moreGamesSelected->getContentSize().height/20.0f));
	moreGames = CCMenuItemSprite::create(moreGamesNormal, moreGamesSelected, this, menu_selector(MainMenuScene::moreGamesCallback));
	moreGames->setPosition(ccp(WINSIZE.width/2.0f - moreGames->getContentSize().width/1.5f, -WINSIZE.height/2.0f + moreGames->getContentSize().height/1.5f));


#ifdef NEW_ART
    CCSprite *soundButton = CCSprite::create("buttonSetting.png");
    soundButton->setCascadeOpacityEnabled(true);
    
    CCSprite *soundButtonDown = CCSprite::create("buttonSetting.png");
    soundButtonDown->setColor(ccGRAY);
    soundButtonDown->setCascadeOpacityEnabled(true);
    
    mark = CCSprite::createWithSpriteFrameName("common/sound.png");
    markDown = CCSprite::createWithSpriteFrameName("common/sound_selected.png");
    mark->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    markDown->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    soundButton->addChild(mark);
    soundButtonDown->addChild(markDown);
    
    
	sound = CCMenuItemSprite::create(soundButton, soundButtonDown, this, menu_selector(MainMenuScene::soundCallback));
	sound->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale());
    sound->setScale(settingBlob->getScale());

    
    labelTTF = CCLabelTTF::create(CCLocalizedString("SOUND", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    sound->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    soundClose = CCLabelTTF::create(CCLocalizedString("ON", NULL), FONT_COMMON, FONT_SIZE_64);
    sound->addChild(soundClose);
	soundClose->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.26f, labelTTF->getParent()->getContentSize().height/2.0f));
    if (isSound)
		((CCLabelTTF*)soundClose)->setString(CCLocalizedString("ON", NULL));
	else
		((CCLabelTTF*)soundClose)->setString(CCLocalizedString("OFF", NULL));

#else
	CCSprite* soundNormal = CCSprite::createWithSpriteFrameName("common/sound.png");
	CCSprite* soundSelected = CCSprite::createWithSpriteFrameName("common/sound_selected.png");
	
	soundSelected->setScale(0.9f);
	soundSelected->setPosition(ccp(soundSelected->getContentSize().width/20.0f, soundSelected->getContentSize().height/20.0f));
	sound = CCMenuItemSprite::create(soundNormal, soundSelected, this, menu_selector(MainMenuScene::soundCallback));
	sound->setPosition(ccp(sound->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f  + settingBlob->getContentSize().width / 5.0f,
		-sound->getContentSize().height/1.3f - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height/1.05));
	soundClose = CCSprite::createWithSpriteFrameName("common/soundNo.png");
	soundClose->setPosition(ccp(soundNormal->getContentSize().width/2.0f, soundNormal->getContentSize().height/2.0f));
	sound->addChild(soundClose);
	if (isSound)
		soundClose->setVisible(false);
	else
		soundClose->setVisible(true);
#endif

#ifdef NEW_ART
    CCSprite *musicButton = CCSprite::create("buttonSetting.png");
    musicButton->setCascadeOpacityEnabled(true);
    
    CCSprite *musicButtonDown = CCSprite::create("buttonSetting.png");
    musicButtonDown->setColor(ccGRAY);
    musicButtonDown->setCascadeOpacityEnabled(true);
    
    mark = CCSprite::createWithSpriteFrameName("common/musicButton.png");
    markDown = CCSprite::createWithSpriteFrameName("common/music_selected.png");
    mark->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    markDown->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    musicButton->addChild(mark);
    musicButtonDown->addChild(markDown);
    
    
	music = CCMenuItemSprite::create(musicButton, musicButtonDown, this, menu_selector(MainMenuScene::musicCallback));
	music->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale()*2.0f);
    music->setScale(settingBlob->getScale());

    
    labelTTF = CCLabelTTF::create(CCLocalizedString("MUSIC", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    music->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    musicClose = CCLabelTTF::create(CCLocalizedString("ON", NULL), FONT_COMMON, FONT_SIZE_64);
    music->addChild(musicClose);
	musicClose->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.26f, labelTTF->getParent()->getContentSize().height/2.0f));
    if (isMusic)
		((CCLabelTTF*)musicClose)->setString(CCLocalizedString("ON", NULL));
	else
		((CCLabelTTF*)musicClose)->setString(CCLocalizedString("OFF", NULL));
    
#else
	CCSprite* musicNormal = CCSprite::createWithSpriteFrameName("common/musicButton.png");
	CCSprite* musicSelected = CCSprite::createWithSpriteFrameName("common/music_selected.png");
	musicSelected->setScale(0.9f);
	musicSelected->setPosition(ccp(musicSelected->getContentSize().width/20.0f, musicSelected->getContentSize().height/20.0f));
	music = CCMenuItemSprite::create(musicNormal, musicSelected, this, menu_selector(MainMenuScene::musicCallback));
	music->setPosition(ccp(music->getContentSize().width - CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width / 2.7f,
		-music->getContentSize().height - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height / 1.17f));
	musicClose = CCSprite::createWithSpriteFrameName("common/soundNo.png");
	musicClose->setPosition(ccp(musicNormal->getContentSize().width/2.0f, musicNormal->getContentSize().height/2.0f));
	music->addChild(musicClose);
	if (isMusic)
		musicClose->setVisible(false);
	else
		musicClose->setVisible(true);
#endif

#ifdef NEW_ART
    CCSprite *askButton = CCSprite::create("buttonSetting.png");
    askButton->setCascadeOpacityEnabled(true);
    
    CCSprite *askButtonDown = CCSprite::create("buttonSetting.png");
    askButtonDown->setColor(ccGRAY);
    askButtonDown->setCascadeOpacityEnabled(true);
    
    mark = CCSprite::createWithSpriteFrameName("common/ask.png");
    markDown = CCSprite::createWithSpriteFrameName("common/ask_selected.png");
    mark->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    markDown->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    askButton->addChild(mark);
    askButtonDown->addChild(markDown);
    
    
	ask = CCMenuItemSprite::create(askButton, askButtonDown, this, menu_selector(MainMenuScene::helpCallback));
	ask->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale()*3.0f);
    ask->setScale(settingBlob->getScale());

    
    labelTTF = CCLabelTTF::create(CCLocalizedString("SETTING_HOW_PLAY", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    ask->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
#else
	CCSprite* askNormal = CCSprite::createWithSpriteFrameName("common/ask.png");
	CCSprite* askSelected = CCSprite::createWithSpriteFrameName("common/ask_selected.png");
	askSelected->setScale(0.9f);
	askSelected->setPosition(ccp(askSelected->getContentSize().width/20.0f, askSelected->getContentSize().height/20.0f));
	ask = CCMenuItemSprite::create(askNormal, askSelected, this, menu_selector(MainMenuScene::helpCallback));
	ask->setPosition(ccp(ask->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width / 1.7f,
		-ask->getContentSize().height - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height / 1.6f));
#endif

#ifdef NEW_ART
    CCSprite *cogwheelButton = CCSprite::create("buttonSetting.png");
    cogwheelButton->setCascadeOpacityEnabled(true);
    
    CCSprite *cogwheelButtonDown = CCSprite::create("buttonSetting.png");
    cogwheelButtonDown->setColor(ccGRAY);
    cogwheelButtonDown->setCascadeOpacityEnabled(true);
    
    mark = CCSprite::createWithSpriteFrameName("common/cogwheel.png");
    markDown = CCSprite::createWithSpriteFrameName("common/cogwheel_selected.png");
    mark->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    markDown->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    cogwheelButton->addChild(mark);
    cogwheelButtonDown->addChild(markDown);
    
    
	cogwheel = CCMenuItemSprite::create(cogwheelButton, cogwheelButtonDown, this, menu_selector(MainMenuScene::cogwheelCallback));
	cogwheel->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale()*4.0f);
    cogwheel->setScale(settingBlob->getScale());

    
    labelTTF = CCLabelTTF::create(CCLocalizedString("SETTING", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    cogwheel->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
#else
	CCSprite* cogwheelNormal = CCSprite::createWithSpriteFrameName("common/cogwheel.png");
	CCSprite* cogwheelSelected = CCSprite::createWithSpriteFrameName("common/cogwheel_selected.png");
	cogwheelSelected->setScale(0.9f);
	cogwheelSelected->setPosition(ccp(cogwheelSelected->getContentSize().width/20.0f, cogwheelSelected->getContentSize().height/20.0f));
	cogwheel = CCMenuItemSprite::create(cogwheelNormal, cogwheelSelected, this, menu_selector(MainMenuScene::cogwheelCallback));
	cogwheel->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width - cogwheel->getContentSize().width/1.25f, 
		cogwheel->getContentSize().height/2.0f - CCDirector::sharedDirector()->getWinSize().height/2.0f));
#endif
    
#ifdef NEW_ART
    closeLeft = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
                                         CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(MainMenuScene::menuSettingCallback));
	closeLeft->setPosition(settingBlob->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width*settingBlob->getScale() /2.5f,
                       settingBlob->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height*settingBlob->getScale() /2.3f);
    closeLeft->setVisible(false);
    closeLeft->setScale(settingBlob->getScale());

#endif

#ifdef NEW_ART
	settingBlob->setPosition(ccp(settingBlob->getPositionX(), settingBlob->getPositionY() - WINSIZE.height));
#else
    settingBlob->setPosition(ccp(-settingBlob->getContentSize().width / 2.0f, -settingBlob->getContentSize().height / 2.0f));
#endif

	sound->setVisible(false);
	music->setVisible(false);
	ask->setVisible(false);
	cogwheel->setVisible(false);
    invite->setVisible(false);
	boy = CCSprite::createWithSpriteFrameName("common/boy.png");
	boy->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - boy->getContentSize().width/1.8f, boy->getContentSize().height/1.5f));
	this->addChild(boy,2);

	iceLit = CCSprite::createWithSpriteFrameName("common/icelittle.png");
	iceLit->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - iceLit->getContentSize().width*1.9f, iceLit->getContentSize().height/1.5f));
	this->addChild(iceLit,3);

	heapIce = CCSprite::createWithSpriteFrameName("mainMenu/heapIce.png");
	heapIce->setPosition(ccp(heapIce->getContentSize().width/2.0f, heapIce->getContentSize().height/2.0f));
	this->addChild(heapIce,1);

	menu = CCMenu::create(play, facebook, gameCenter, moreGames, NULL);
    menuLeft = CCMenu::create(setting, sound, music, ask, cogwheel, invite, closeLeft, NULL);
	this->addChild(menu, 10);
    this->addChild(menuLeft, 101);


	popup = CCSprite::createWithSpriteFrameName("common/panel.png");

	CCPoint posPopup;
    if (WINSIZE.height == 2048)
        posPopup.y = CCDirector::sharedDirector()->getWinSize().height - popup->getContentSize().height/1.9f;
    else
        posPopup.y = CCDirector::sharedDirector()->getWinSize().height/2.0f;


#ifdef LANDSCAPE
	posPopup.x = CCDirector::sharedDirector()->getWinSize().width/2.0f;
#else
	posPopup.x = CCDirector::sharedDirector()->getWinSize().width/2.0f;
#endif

	popup->setPosition(posPopup);

	title = CCSprite::createWithSpriteFrameName("common/markWhell.png");
	title->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.2f));
	popup->addChild(title);		

	close = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
		CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(MainMenuScene::closeSettingCallback));
	close->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /2.5f, 
		popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + popup->getContentSize().height /2.3f);
    
    
    CCSprite *buttonFaceConnectMark = CCSprite::createWithSpriteFrameName("common/markFacebook.png");
    CCSprite *buttonFaceConnectMarkDown = CCSprite::createWithSpriteFrameName("common/markFacebook.png");
    
    CCSprite *buttonFaceConnect = CCSprite::create("buttonSetting.png");
    buttonFaceConnect->setCascadeOpacityEnabled(true);
    buttonFaceConnect->addChild(buttonFaceConnectMark);
    buttonFaceConnectMark->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));
    
    CCSprite *buttonFaceConnectDown = CCSprite::create("buttonSetting.png");
    buttonFaceConnect->setCascadeOpacityEnabled(true);
    buttonFaceConnectDown->addChild(buttonFaceConnectMarkDown);
    buttonFaceConnectMarkDown->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));
    buttonFaceConnectDown->setColor(ccGRAY);

	faceConnect = CCMenuItemSprite::create(buttonFaceConnect, buttonFaceConnectDown, this, menu_selector(MainMenuScene::facebookCallback));
	faceConnect->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f, 
		popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + faceConnect->getContentSize().height*1.5f);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("CONNECT", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    faceConnect->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    
    mark = CCSprite::createWithSpriteFrameName("common/markInfo.png");
    markDown = CCSprite::createWithSpriteFrameName("common/markInfo.png");
    mark->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));
    markDown->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));
    
    
    CCSprite *howPlayButton = CCSprite::create("buttonSetting.png");
    howPlayButton->setCascadeOpacityEnabled(true);
    howPlayButton->addChild(mark);
    
    CCSprite *howPlayButtonDown = CCSprite::create("buttonSetting.png");
    howPlayButtonDown->setColor(ccGRAY);
    howPlayButtonDown->setCascadeOpacityEnabled(true);
    howPlayButtonDown->addChild(markDown);

	howPlay = CCMenuItemSprite::create(howPlayButton, howPlayButtonDown, this, menu_selector(MainMenuScene::howToPlaySettingCallback));
	howPlay->setPosition(faceConnect->getPositionX(), faceConnect->getPositionY() - faceConnect->getContentSize().height);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("SETTING_HOW_PLAY", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    howPlay->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    
    mark = CCSprite::createWithSpriteFrameName("common/markAsk.png");
    markDown = CCSprite::createWithSpriteFrameName("common/markAsk.png");
    mark->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));
    markDown->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));


    CCSprite *freqAskButton = CCSprite::create("buttonSetting.png");
    freqAskButton->setCascadeOpacityEnabled(true);
    freqAskButton->addChild(mark);
    CCSprite *freqAskButtonDown = CCSprite::create("buttonSetting.png");
    freqAskButtonDown->setCascadeOpacityEnabled(true);
    freqAskButtonDown->addChild(markDown);
    freqAskButtonDown->setColor(ccGRAY);
    
	freqAsk = CCMenuItemSprite::create(freqAskButton, freqAskButtonDown, this, menu_selector(MainMenuScene::askedCallback));
	freqAsk->setPosition(faceConnect->getPositionX(), faceConnect->getPositionY() - faceConnect->getContentSize().height*2.0f);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("SETTING_ASK", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    freqAsk->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    
    mark = CCSprite::createWithSpriteFrameName("common/markArrow.png");
    markDown = CCSprite::createWithSpriteFrameName("common/markArrow.png");
    mark->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));
    markDown->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));

    CCSprite *restorePurshaseButton = CCSprite::create("buttonSetting.png");
    restorePurshaseButton->setCascadeOpacityEnabled(true);
    restorePurshaseButton->addChild(mark);
    CCSprite *restorePurshaseButtonDown = CCSprite::create("buttonSetting.png");
    restorePurshaseButtonDown->setCascadeOpacityEnabled(true);
    restorePurshaseButtonDown->addChild(markDown);
    restorePurshaseButtonDown->setColor(ccGRAY);
    
	restorePurshase = CCMenuItemSprite::create(restorePurshaseButton, restorePurshaseButtonDown, this, menu_selector(MainMenuScene::restoreCallback));
	restorePurshase->setPosition(faceConnect->getPositionX(), faceConnect->getPositionY() - faceConnect->getContentSize().height*3.0f);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("SETTING_RESTORE", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    restorePurshase->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    mark = CCSprite::createWithSpriteFrameName("common/markRotate.png");
    markDown = CCSprite::createWithSpriteFrameName("common/markRotate.png");
    mark->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));
    markDown->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));

    CCSprite *resetButton = CCSprite::create("buttonSetting.png");
    resetButton->setCascadeOpacityEnabled(true);
    resetButton->addChild(mark);
    CCSprite *resetButtonDown = CCSprite::create("buttonSetting.png");
    resetButtonDown->setCascadeOpacityEnabled(true);
    resetButtonDown->addChild(markDown);
    resetButtonDown->setColor(ccGRAY);
    
	reset = CCMenuItemSprite::create(resetButton, resetButtonDown, this, menu_selector(MainMenuScene::resetCallback));
	reset->setPosition(faceConnect->getPositionX(), faceConnect->getPositionY() - faceConnect->getContentSize().height*4.0f);
    
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("SETTING_RESET", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    reset->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    
    mark = CCSprite::createWithSpriteFrameName("common/markNotif.png");
    markDown = CCSprite::createWithSpriteFrameName("common/markNotif.png");
    mark->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));
    markDown->setPosition(ccp(buttonFaceConnect->getContentSize().width/7.0f, buttonFaceConnect->getContentSize().height/2.0f));

    CCSprite *notificationButton = CCSprite::create("buttonSetting.png");
    notificationButton->setCascadeOpacityEnabled(true);
    notificationButton->addChild(mark);
    CCSprite *notificationButtonDown = CCSprite::create("buttonSetting.png");
    notificationButtonDown->setCascadeOpacityEnabled(true);
    notificationButtonDown->addChild(markDown);
    notificationButtonDown->setColor(ccGRAY);
    
	notification = CCMenuItemSprite::create(notificationButton, notificationButtonDown, this, menu_selector(MainMenuScene::notificationCallback));
	notification->setPosition(faceConnect->getPositionX(), faceConnect->getPositionY() - faceConnect->getContentSize().height*5.0f);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("SETTING_NOTIFICATION", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    notification->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));

	popupMenu = CCMenu::create(close, faceConnect, howPlay, freqAsk, restorePurshase, reset, notification, NULL);

	this->addChild(popup, 10);
	this->addChild(popupMenu, 10);

	popup->setVisible(false);
	popupMenu->setVisible(false);



	tutorialPopup= CCSprite::createWithSpriteFrameName("common/panel.png");

	tutorialPopup->setPosition(posPopup);

	tutorialClose = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
		CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(MainMenuScene::closeTutorialCallback));
	tutorialClose->setPosition(tutorialPopup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + tutorialPopup->getContentSize().width /2.5f, 
		tutorialPopup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + tutorialPopup->getContentSize().height /2.3f);

    CCSprite *tutorialNextButton = CCSprite::createWithSpriteFrameName("common/redButton.png");
    CCSprite *tutorialNextButtonDown = CCSprite::createWithSpriteFrameName("common/redButton.png");
    tutorialNextButtonDown->setColor(ccGRAY);
	tutorialNext = CCMenuItemSprite::create(tutorialNextButton, tutorialNextButtonDown, this, menu_selector(MainMenuScene::tutorialCallback));
	tutorialNext->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
		(popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - tutorialNext->getContentSize().height*1.2f* 3.0f));
    tutorialNext->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("NEXT", NULL), FONT_COMMON, FONT_SIZE_86);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    tutorialNext->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

	tutorialSprite = CCSprite::createWithSpriteFrameName("common/1.png");
	tutorialSprite->setPosition(ccp(popup->getContentSize().width / 2.7f, popup->getContentSize().height / 1.27f));
	tutorialPopup->addChild(tutorialSprite);
	tutorialSprite->setOpacity(0);
	tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCFadeIn::create(0.5f)));
    
    tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_1", NULL), FONT_COMMON, FONT_SIZE_48);
    tutorialText->setColor(IceCreamBrown);
    tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 1.0f*MULTIPLIER);
    tutorialSprite->addChild(tutorialText);
    tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/1.5f, tutorialText->getParent()->getContentSize().height/2.0f));
    
    tutorialHeader = CCLabelTTF::create(CCLocalizedString("SETTING_HOW_PLAY", NULL), FONT_COMMON, FONT_SIZE_86);
    tutorialHeader->setColor(IceCreamPink);
    tutorialHeader->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    tutorialPopup->addChild(tutorialHeader);
    tutorialHeader->setPosition(ccp(tutorialHeader->getParent()->getContentSize().width/2.0f, tutorialHeader->getParent()->getContentSize().height/1.1f));

	tutorialMenu = CCMenu::create(tutorialClose, tutorialNext, NULL);

	this->addChild(tutorialPopup, 10);
	this->addChild(tutorialMenu, 10);
    
    
    if (LANDSCAPE)
    {
        if (IPHONE_4 || IPHONE_5)
        {
            popup->setScale(0.64f);
            popupMenu->setScale(0.64f);
        }
        else
        {
            popup->setScale(0.76f);
            popupMenu->setScale(0.76f);
        }
        popupMenu->setPosition(ccp(popupMenu->getPositionX()*popupMenu->getScale(),
                                   popupMenu->getPositionY()*popupMenu->getScale()));
        
        if (IPHONE_4 || IPHONE_5)
        {
            tutorialPopup->setScale(0.64f);
            tutorialMenu->setScale(0.64f);
        }
        else
        {
            tutorialPopup->setScale(0.76f);
            tutorialMenu->setScale(0.76f);
        }
        tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX()*tutorialMenu->getScale(),
                                      tutorialMenu->getPositionY()*tutorialMenu->getScale()));
        
        if (WINSIZE.width == 1136)
        {
            background->setScaleX(1.1f);
            iceBlob->setScaleX(1.1f);
        }
    }
    else
    {
        popup->setScale(popupScale);
        popupMenu->setScale(popupScale);
        popupMenu->setPosition(ccp(popupMenu->getPositionX()*popupMenu->getScale(),
                                   popupMenu->getPositionY()*popupMenu->getScale()));
        
        tutorialPopup->setScale(popupScale);
        tutorialMenu->setScale(popupScale);
        tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX()*tutorialMenu->getScale(),
                                      tutorialMenu->getPositionY()*tutorialMenu->getScale()));
        
        if (WINSIZE.height == 1136)
        {
            background->setScaleY(1.1f);
            iceBlob->setScaleY(1.1f);
        }
    }

	tutorialPopup->setVisible(false);
	tutorialMenu->setVisible(false);

	tutorialPopup->setPosition(ccp(tutorialPopup->getPositionX(), tutorialPopup->getPositionY() - tutorialPopup->getContentSize().height));
	tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX(), tutorialMenu->getPositionY() - tutorialPopup->getContentSize().height));
	popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY() - popup->getContentSize().height));
	popupMenu->setPosition(ccp(popupMenu->getPositionX(), popupMenu->getPositionY() - popup->getContentSize().height));
    
    if (OptionsPtr->isFacebookConnection() && FacebookPtr->sessionIsOpened() && getNetworkStatus())
    {
        facebook->setVisible(false);
        faceConnect->setVisible(false);
        faceConnect->setVisible(false);
        faceConnect->setCascadeOpacityEnabled(true);
        faceConnect->setEnabled(false);
    }
    bool flagLoad = false;
    flagLoad = CCUserDefault::sharedUserDefault()->getBoolForKey("gameCenterLogin", false);
    if (flagLoad)
    {
        gameCenter->setVisible(false);
        gameCenter->setVisible(false);
        gameCenter->setVisible(false);
        gameCenter->setCascadeOpacityEnabled(true);
        gameCenter->setEnabled(false);
    }
    
    MMPPtr->sessionLoaded();
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::facebookCheck))));
    
	return true;
}

void MainMenuScene::moreGamesCallback(CCObject* pSender)
{
    lock();
    menu->setEnabled(false);
    moreGamesLayer->showMessageboard();
}

void MainMenuScene::inviteCallback(CCObject* pSender)
{
    if (isLock)
        return;
    
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    
    lock();
    
	if (isSetting)
	{
#ifdef NEW_ART
        settingBlob->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -WINSIZE.height))));
		isSetting = false;
		sound->setVisible(false);
		music->setVisible(false);
		ask->setVisible(false);
		cogwheel->setVisible(false);
        closeLeft->setVisible(false);
        invite->setVisible(false);
        isLock = true;
        settingBlob->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::settingFinished))));
        setting->setVisible(true);
        setting->setEnabled(false);
#else
		settingBlob->runAction(CCMoveBy::create(0.3f, ccp(-settingBlob->getContentSize().width, -settingBlob->getContentSize().height)));
		isSetting = false;
		sound->setVisible(false);
		music->setVisible(false);
		ask->setVisible(false);
		cogwheel->setVisible(false);
        invite->setVisible(false);
        isLock = true;
        settingBlob->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::settingFinished))));
#endif
	}
    
    menu->setEnabled(false);
    popupMenu->setEnabled(false);
    tutorialMenu->setEnabled(false);
    popupLayer->popupPost((char*)CCLocalizedString("INVITE_FRIENDS_TITLE", NULL), (char*)CCLocalizedString("INVITE_FRIENDS_TEXT", NULL), (char*)CCLocalizedString("INVITE_FRIENDS_BUTTON", NULL), GreenPopup, BombPopBoot, this, callfuncN_selector(MainMenuScene::addBonus), this, callfuncN_selector(MainMenuScene::unclockMenu));
}

void MainMenuScene::facebookCheck(CCNode* pSender)
{
    if (OptionsPtr->isFacebookConnection())
    {
        if (getNetworkStatus())
            if (!FacebookPtr->sessionIsOpened())
                menuFacebookCallback(NULL);
    }
}

void MainMenuScene::changeOrientation()
{
    popupLayer->changeOrientation();
    moreGamesLayer->changeOrientation();
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("mainMenu.plist", CCTextureCache::sharedTextureCache()->addPVRImage("mainMenu.pvr.ccz"));

    this->setContentSize(WINSIZE);
    menu->setContentSize(WINSIZE);
    menuLeft->setContentSize(WINSIZE);
    
    social->changeOrientation();
    menu->setPosition(WINSIZE.width/2.0f, WINSIZE.height/2.0f);
    menuLeft->setPosition(WINSIZE.width/2.0f, WINSIZE.height/2.0f);
    tutorialMenu->setContentSize(WINSIZE);
    tutorialMenu->setPosition(WINSIZE.width/2.0f, WINSIZE.height/2.0f);
    popupMenu->setContentSize(WINSIZE);
    popupMenu->setPosition(WINSIZE.width/2.0f, WINSIZE.height/2.0f);
    if (background)
        background->removeFromParentAndCleanup(true);

    if (WINSIZE.height < WINSIZE.width)
        background = CCSprite::createWithSpriteFrameName("mainMenu/mainMenuBackgroundLandscape.jpg");
    else
        background = CCSprite::createWithSpriteFrameName("mainMenu/mainMenuBackground.jpg");
    
    
    background->setAnchorPoint(ccp(0.0f, 1.0f));
    background->setPosition(ccp(0.0f, WINSIZE.height));
    
    this->addChild(background);
    
    logo->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 1.9f, CCDirector::sharedDirector()->getWinSize().height / 1.4f));
    
#ifndef NEW_ART
    settingBlob->setPosition(ccp(settingBlob->getContentSize().width / 2.0f, settingBlob->getContentSize().height / 2.0f));
#endif
    
	play->setPosition(ccp(0.0f, CCDirector::sharedDirector()->getWinSize().height / 1.4f - CCDirector::sharedDirector()->getWinSize().height/2.0f - logo->getContentSize().height/2.0f));
    
	facebook->setPosition(ccp(play->getPositionX(), play->getPositionY() - facebook->getContentSize().height*1.1f));
    
    gameCenter->setPosition(ccp(play->getPositionX(), play->getPositionY() - gameCenter->getContentSize().height*1.9f));
    
    
#ifdef NEW_ART
	setting->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width /2.0f + setting->getContentSize().width / 2.0f,
                             -CCDirector::sharedDirector()->getWinSize().height /2.0f + setting->getContentSize().height / 2.0f));
#else
	setting->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width /2.0f + setting->getContentSize().width / 2.6f,
                             -CCDirector::sharedDirector()->getWinSize().height /2.0f + setting->getContentSize().height / 2.6f));
    
#endif

#ifndef NEW_ART
	invite->setPosition(ccp(invite->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f - settingBlob->getContentSize().width / 20.0f,
                            -invite->getContentSize().height/1.3f - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height/0.99f));

	sound->setPosition(ccp(sound->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f  + settingBlob->getContentSize().width / 5.0f,
                           -sound->getContentSize().height/1.3f - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height/1.05));

	music->setPosition(ccp(music->getContentSize().width - CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width / 2.7f,
                           -music->getContentSize().height - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height / 1.17f));
    
	ask->setPosition(ccp(ask->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width / 1.7f,
                         -ask->getContentSize().height - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height / 1.6f));
    
	cogwheel->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width - cogwheel->getContentSize().width/1.25f,
                              cogwheel->getContentSize().height/2.0f - CCDirector::sharedDirector()->getWinSize().height/2.0f));
#endif
    
    moreGames->setPosition(ccp(WINSIZE.width/2.0f - moreGames->getContentSize().width/1.5f, -WINSIZE.height/2.0f + moreGames->getContentSize().height/1.5f));
    
    settingBlob->stopAllActions();
    if (!isSetting)
        settingBlob->setPosition(ccp(-settingBlob->getContentSize().width / 2.0f, -settingBlob->getContentSize().height / 2.0f));
    else
        settingBlob->setPosition(ccp(settingBlob->getContentSize().width / 2.0f, settingBlob->getContentSize().height / 2.0f));
        
    
	boy->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - boy->getContentSize().width/1.8f, boy->getContentSize().height/1.5f));

	iceLit->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - iceLit->getContentSize().width*1.9f, iceLit->getContentSize().height/1.5f));
    
	heapIce->setPosition(ccp(heapIce->getContentSize().width/2.0f, heapIce->getContentSize().height/2.0f));
    
	CCPoint posPopup;
    posPopup.y = CCDirector::sharedDirector()->getWinSize().height/2.0f;
	posPopup.x = CCDirector::sharedDirector()->getWinSize().width/2.0f;

	popup->setPosition(posPopup);
    
	title->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.2f));
    
	close->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /2.5f,
                       popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + popup->getContentSize().height /2.3f);
    
	faceConnect->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                             popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + faceConnect->getContentSize().height*1.2f);
    
	howPlay->setPosition(faceConnect->getPositionX(), faceConnect->getPositionY() - faceConnect->getContentSize().height);
    
	freqAsk->setPosition(faceConnect->getPositionX(), faceConnect->getPositionY() - faceConnect->getContentSize().height*2.0f);
    
	restorePurshase->setPosition(faceConnect->getPositionX(), faceConnect->getPositionY() - faceConnect->getContentSize().height*3.0f);
    
	reset->setPosition(faceConnect->getPositionX(), faceConnect->getPositionY() - faceConnect->getContentSize().height*4.0f);
    
	notification->setPosition(faceConnect->getPositionX(), faceConnect->getPositionY() - faceConnect->getContentSize().height*5.0f);
    
	tutorialPopup->setPosition(posPopup);
    
	tutorialClose->setPosition(tutorialPopup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + tutorialPopup->getContentSize().width /2.5f,
                               tutorialPopup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + tutorialPopup->getContentSize().height /2.3f);
    
	tutorialNext->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                              (popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - tutorialNext->getContentSize().height*1.2f* 3.0f));
    
    if (WINSIZE.height < WINSIZE.width)
    {
        if (IPHONE_4 || IPHONE_5)
        {
            popup->setScale(0.64f);
            popupMenu->setScale(0.64f);
        }
        else
        {
            popup->setScale(0.76f);
            popupMenu->setScale(0.76f);
        }
        popupMenu->setPosition(ccp(popupMenu->getPositionX()*popupMenu->getScale(),
                                   popupMenu->getPositionY()*popupMenu->getScale()));
        
        if (IPHONE_4 || IPHONE_5)
        {
            tutorialPopup->setScale(0.64f);
            tutorialMenu->setScale(0.64f);
        }
        else
        {
            tutorialPopup->setScale(0.76f);
            tutorialMenu->setScale(0.76f);
        }
        tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX()*tutorialMenu->getScale(),
                                      tutorialMenu->getPositionY()*tutorialMenu->getScale()));
        
        if (WINSIZE.width == 1136)
        {
            background->setScaleX(1.1f);
            iceBlob->setScaleX(1.1f);
        }
    }
    else
    {
        popup->setScale(popupScale);
        popupMenu->setScale(popupScale);
        popupMenu->setPosition(ccp(popupMenu->getPositionX()*popupMenu->getScale(),
                                   popupMenu->getPositionY()*popupMenu->getScale()));
        
        tutorialPopup->setScale(popupScale);
        tutorialMenu->setScale(popupScale);
        tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX()*tutorialMenu->getScale(),
                                      tutorialMenu->getPositionY()*tutorialMenu->getScale()));
        
        if (WINSIZE.height == 1136)
        {
            background->setScaleY(1.1f);
            iceBlob->setScaleY(1.1f);
        }
    }
    
	tutorialSprite->setPosition(ccp(popup->getContentSize().width / 2.7f, popup->getContentSize().height / 1.27f));
    
    if (isLock)
    {
        if (tutorial)
        {
            if (helpNum == 0)
                tutorial->setPosition(ccp(popup->getContentSize().width / 2.7f, popup->getContentSize().height / 1.27f));
            else if (helpNum == 1)
                tutorial->setPosition(ccp(popup->getContentSize().width / 1.9f, popup->getContentSize().height / 1.55f));
            else if (helpNum == 2)
                tutorial->setPosition(ccp(popup->getContentSize().width / 2.1f, popup->getContentSize().height / 1.4f));
        }
    }
    
    if (tutorialNum == 0)
        tutorialSprite->setPosition(ccp(popup->getContentSize().width / 2.7f, popup->getContentSize().height / 1.27f));
    else if (tutorialNum == 1)
        tutorialSprite->setPosition(ccp(popup->getContentSize().width / 1.9f, popup->getContentSize().height / 1.55f));
    else if (tutorialNum == 2)
        tutorialSprite->setPosition(ccp(popup->getContentSize().width / 2.1f, popup->getContentSize().height / 1.4f));
    

    if (!isPopup)
    {
        popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY() - popup->getContentSize().height));
        popupMenu->setPosition(ccp(popupMenu->getPositionX(), popupMenu->getPositionY() - popup->getContentSize().height));
    }
    else
    {
        popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY()));
        popupMenu->setPosition(ccp(popupMenu->getPositionX(), popupMenu->getPositionY()));
    }
    
    if (!isTutorial)
    {
        tutorialPopup->setPosition(ccp(tutorialPopup->getPositionX(), tutorialPopup->getPositionY() - tutorialPopup->getContentSize().height));
        tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX(), tutorialMenu->getPositionY() - tutorialPopup->getContentSize().height));
    }
    else
    {
        tutorialPopup->setPosition(ccp(tutorialPopup->getPositionX(), tutorialPopup->getPositionY()));
        tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX(), tutorialMenu->getPositionY()));
    }
    
    if (spriteLoading)
    {
        spriteLoading->removeFromParentAndCleanup(true);
        labelLoad->removeFromParentAndCleanup(true);
        SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
        if (IPAD)
        {
            if (LANDSCAPE)
                spriteLoading = CCSprite::create("loadingiPadLandscape.png");
            else
                spriteLoading = CCSprite::create("loadingiPadPortrait.png");
        }
        else if (IPAD_MINI)
        {
            if (LANDSCAPE)
                spriteLoading = CCSprite::create("loadingiPadMiniLandscape.png");
            else
                spriteLoading = CCSprite::create("loadingiPadMiniPortrait.png");
        }
        else if (IPHONE_4||IPHONE_5)
        {
            if (LANDSCAPE)
                spriteLoading = CCSprite::create("loadingIphoneLanscape.png");
            else
                spriteLoading = CCSprite::create("loadingIphonePortrait.png");
        }
        labelLoad = CCLabelTTF::create(CCLocalizedString("LOADING", NULL), FONT_COMMON, FONT_SIZE_86);
        labelLoad->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/10.0f));
        this->addChild(labelLoad, 1001);
        spriteLoading->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
        this->addChild(spriteLoading, 1000);
    }
    

#ifdef NEW_ART
    if (LANDSCAPE)
    {
        if (IPHONE_4 || IPHONE_5)
            settingBlob->setScale(0.63f);
        else
            settingBlob->setScale(0.75f);
    }
    else
    {
        settingBlob->setScale(1.0f);
    }
    settingBlob->setPosition(ccp(WINSIZE.width / 2.0f, WINSIZE.height / 2.0f));
    
    if (LANDSCAPE)
        invite->setPosition(settingBlob->getPositionX() - WINSIZE.width/2.0f, settingBlob->getPositionY()*settingBlob->getScale() - WINSIZE.height/2.0f + invite->getContentSize().height*2.5f*settingBlob->getScale());
    else
        invite->setPosition(settingBlob->getPositionX() - WINSIZE.width/2.0f, settingBlob->getPositionY()*settingBlob->getScale() - WINSIZE.height/2.0f + invite->getContentSize().height*1.2f*settingBlob->getScale());
    
    closeLeft->setPosition(settingBlob->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width*settingBlob->getScale() /2.5f,
                           settingBlob->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height*settingBlob->getScale() /2.3f);
    cogwheel->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale()*4.0f);
    ask->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale()*3.0f);
    music->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale()*2.0f);
    sound->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale());
    
    closeLeft->setScale(settingBlob->getScale());
    cogwheel->setScale(settingBlob->getScale());
    ask->setScale(settingBlob->getScale());
    music->setScale(settingBlob->getScale());
    sound->setScale(settingBlob->getScale());
    invite->setScale(settingBlob->getScale());

    if (!ask->isVisible())
        settingBlob->setPosition(ccp(settingBlob->getPositionX(), settingBlob->getPositionY() - WINSIZE.height));
#endif
    
    return;
}

void MainMenuScene::facebookCallback(CCObject* pSender)
{
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    lock();
    
    if (settingState != Setting)
        return;
    
    settingState = Facebook;
    
	for (int i = 0; i < popup->getChildrenCount(); i++)
	{
		((CCSprite*)popup->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));
	}
	for (int i = 0; i < popupMenu->getChildrenCount(); i++)
	{
		if (((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i)) != close)
        {
			((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));
        }
	}
	close->setNormalImage(CCSprite::createWithSpriteFrameName("common/back.png"));
	close->setSelectedImage(CCSprite::createWithSpriteFrameName("common/back_on.png"));
	settingState = Facebook;

	CCSprite* facebook = CCSprite::createWithSpriteFrameName("common/markFacebook.png");
	facebook->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.2f));
	popup->addChild(facebook);
	facebook->setOpacity(0);
	facebook->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));

	label = CCLabelTTF::create(CCLocalizedString("SETTING_FACEBOOK_TEXT", NULL), FONT_COMMON, FONT_SIZE_64);
	label->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 2.0f));
	popup->addChild(label);
	label->setOpacity(0);
    ccColor3B color;
    color.r = 0x91;
    color.g = 0x3b;
    color.b = 0x55;
	label->setColor(color);
	label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    CCSprite* facebookButtonButton = CCSprite::createWithSpriteFrameName("common/facebookButtonMainMenu.png");
    CCSprite* facebookButtonButtonDown = CCSprite::createWithSpriteFrameName("common/facebookButtonMainMenu.png");
    facebookButtonButtonDown->setColor(ccGRAY);

	CCMenuItemSprite* facebookButton = CCMenuItemSprite::create(facebookButtonButton, facebookButtonButtonDown, this, menu_selector(MainMenuScene::menuFacebookCallback));
	facebookButton->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
		(popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - facebookButton->getContentSize().height*1.2f* 3.0f));
    facebookButton->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
	popupMenu->addChild(facebookButton);
	facebookButton->setOpacity(0);
	facebookButton->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("CONNECT", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    facebookButton->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.75f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    faceConnect->setEnabled(false);
    howPlay->setEnabled(false);
    freqAsk->setEnabled(false);
    restorePurshase->setEnabled(false);
    reset->setEnabled(false);
    notification->setEnabled(false);
}


void MainMenuScene::helpFromSetting(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    isTutorial = true;
	tutorialMenu->setVisible(true);
	tutorialPopup->setVisible(true);
    
	tutorialPopup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, tutorialPopup->getContentSize().height))));
	tutorialMenu->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, tutorialPopup->getContentSize().height))));
    
    tutorialClose->stopAllActions();
    tutorialClose->setScale(0.7f);
	tutorialClose->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    tutorialNext->stopAllActions();
    tutorialNext->setScale(0.7f);
	tutorialNext->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_1", NULL), FONT_COMMON, FONT_SIZE_48);
    tutorialText->setColor(IceCreamBrown);
    tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    tutorialSprite->addChild(tutorialText);
    tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/1.5f, tutorialText->getParent()->getContentSize().height/2.0f));
    
	menu->setEnabled(false);
}

void MainMenuScene::howToPlaySettingCallback(CCObject* pSender)
{
    closeSettingCallback(NULL);
    this->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::helpFromSetting)), NULL));
    return;
    lock();
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    if (settingState != Setting)
        return;
    
    settingState = HowPlay;
    
	for (int i = 0; i < popup->getChildrenCount(); i++)
	{
		((CCSprite*)popup->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));
	}
	for (int i = 0; i < popupMenu->getChildrenCount(); i++)
	{
		if (((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i)) != close)
			((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));				
	}
	close->setNormalImage(CCSprite::createWithSpriteFrameName("common/back.png"));
	close->setSelectedImage(CCSprite::createWithSpriteFrameName("common/back_on.png"));
	settingState = HowPlay;

	tutorial = CCSprite::createWithSpriteFrameName("common/1.png");
	tutorial->setPosition(ccp(popup->getContentSize().width / 2.7f, popup->getContentSize().height / 1.27f));
	popup->addChild(tutorial);
	tutorial->setOpacity(0);
	tutorial->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));

    label = CCLabelTTF::create("", fontStd.c_str(), 50);
	label->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 2.0f));
	popup->addChild(label);
	label->setOpacity(0);
    ccColor3B color;
    color.r = 0x91;
    color.g = 0x3b;
    color.b = 0x55;
	label->setColor(color);
	label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    CCLabelTTF* howToPlayText = CCLabelTTF::create(CCLocalizedString("HELP_1", NULL), FONT_COMMON, FONT_SIZE_48);
    howToPlayText->setColor(IceCreamBrown);
    howToPlayText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    tutorial->addChild(howToPlayText);
    howToPlayText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/1.5f, tutorialText->getParent()->getContentSize().height/2.0f));
    
    howToPlayHeader = CCLabelTTF::create(CCLocalizedString("SETTING_HOW_PLAY", NULL), FONT_COMMON, FONT_SIZE_86);
    howToPlayHeader->setColor(IceCreamPink);
    howToPlayHeader->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    popup->addChild(howToPlayHeader);
    popup->setCascadeOpacityEnabled(true);
    howToPlayHeader->setPosition(ccp(howToPlayHeader->getParent()->getContentSize().width/2.0f, howToPlayHeader->getParent()->getContentSize().height/1.1f));
    
    
    CCSprite* facebookButtonButton = CCSprite::createWithSpriteFrameName("common/redButton.png");
    CCSprite* facebookButtonButtonDown = CCSprite::createWithSpriteFrameName("common/redButton.png");
    facebookButtonButtonDown->setColor(ccGRAY);

	CCMenuItemSprite* facebookButton = CCMenuItemSprite::create(facebookButtonButton, facebookButtonButtonDown, this, menu_selector(MainMenuScene::nextCallback));
	facebookButton->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
		(popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - facebookButton->getContentSize().height*1.2f* 3.0f));
    facebookButton->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
	popupMenu->addChild(facebookButton);
	facebookButton->setOpacity(0);
	facebookButton->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("NEXT", NULL), FONT_COMMON, FONT_SIZE_86);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    facebookButton->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
}

void MainMenuScene::tutorialCallback(CCObject* pSender)
{
    lock();
    
	tutorialNum++;
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");

	if (tutorialNum == 3)
	{
        tutorialNum = 0;
        ((CCMenuItemSprite*)pSender)->setNormalImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
        ((CCMenuItemSprite*)pSender)->setSelectedImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
        ((CCSprite*)(((CCMenuItemSprite*)pSender)->getSelectedImage()))->setColor(ccGRAY);
        
        labelTTF = CCLabelTTF::create(CCLocalizedString("NEXT", NULL), FONT_COMMON, FONT_SIZE_86);
        labelTTF->setColor(ccWHITE);
        labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
        ((CCMenuItemSprite*)pSender)->addChild(labelTTF);
        labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
        
        closeTutorialCallback(pSender);
        return;
    }
    if (tutorialNum == 2)
	{
		((CCMenuItemSprite*)pSender)->setNormalImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
        ((CCMenuItemSprite*)pSender)->setSelectedImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
        ((CCSprite*)(((CCMenuItemSprite*)pSender)->getSelectedImage()))->setColor(ccGRAY);
        
        labelTTF = CCLabelTTF::create(CCLocalizedString("CLOSE", NULL), FONT_COMMON, FONT_SIZE_86);
        labelTTF->setColor(ccWHITE);
        labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);

        ((CCMenuItemSprite*)pSender)->addChild(labelTTF);
        labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
	}

	tutorialSprite->removeFromParentAndCleanup(true);

	if (tutorialNum == 0)
	{
        ((CCMenuItemSprite*)pSender)->setNormalImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
        ((CCMenuItemSprite*)pSender)->setSelectedImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
        ((CCSprite*)(((CCMenuItemSprite*)pSender)->getSelectedImage()))->setColor(ccGRAY);
        
        labelTTF = CCLabelTTF::create(CCLocalizedString("NEXT", NULL), FONT_COMMON, FONT_SIZE_86);
        labelTTF->setColor(ccWHITE);
        labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
        ((CCMenuItemSprite*)pSender)->addChild(labelTTF);
        labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
        
		tutorialSprite = CCSprite::createWithSpriteFrameName("common/1.png");
		tutorialSprite->setPosition(ccp(popup->getContentSize().width / 2.7f, popup->getContentSize().height / 1.27f));
		tutorialPopup->addChild(tutorialSprite);
		tutorialSprite->setOpacity(0);
		tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
        
        tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_1", NULL), FONT_COMMON, FONT_SIZE_48);
        tutorialText->setColor(IceCreamBrown);
        tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);

        tutorialSprite->addChild(tutorialText);
        tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/1.5f, tutorialText->getParent()->getContentSize().height/2.0f));
	}
	else if (tutorialNum == 1)
	{
		tutorialSprite = CCSprite::createWithSpriteFrameName("common/2.png");
		tutorialSprite->setPosition(ccp(popup->getContentSize().width / 1.9f, popup->getContentSize().height / 1.55f));
		tutorialPopup->addChild(tutorialSprite);
		tutorialSprite->setOpacity(0);
		tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
        
        tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_2", NULL), FONT_COMMON, FONT_SIZE_54);
        tutorialText->setColor(IceCreamBrown);
        labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);

        tutorialSprite->addChild(tutorialText);
        tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/2.0f, tutorialText->getParent()->getContentSize().height/1.25f));
	}
	else if (tutorialNum == 2)
	{
		tutorialSprite = CCSprite::createWithSpriteFrameName("common/3.png");
		tutorialSprite->setPosition(ccp(popup->getContentSize().width / 2.1f, popup->getContentSize().height / 1.4f));
		tutorialPopup->addChild(tutorialSprite);
		tutorialSprite->setOpacity(0);
		tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
        
        tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_3", NULL), FONT_COMMON, FONT_SIZE_54);
        tutorialText->setColor(IceCreamBrown);
        labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);

        tutorialSprite->addChild(tutorialText);
        tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/1.9f, tutorialText->getParent()->getContentSize().height/1.4f));
	}
}

void MainMenuScene::tutorialFinished(CCNode* sender)
{
    tutorialSprite->removeFromParentAndCleanup(true);
    tutorialSprite = CCSprite::createWithSpriteFrameName("common/1.png");
    tutorialSprite->setPosition(ccp(popup->getContentSize().width / 2.7f, popup->getContentSize().height / 1.27f));
    tutorialPopup->addChild(tutorialSprite);
    tutorialSprite->setOpacity(0);
    tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    tutorialNum = 0;
    tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_1", NULL), FONT_COMMON, FONT_SIZE_48);
    tutorialText->setColor(IceCreamBrown);
    tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    
    tutorialSprite->addChild(tutorialText);
    tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/1.5f, tutorialText->getParent()->getContentSize().height/2.0f));
    
	tutorialPopup->setVisible(false);
	tutorialMenu->setVisible(false);
	menu->setEnabled(true);
}

void MainMenuScene::closeTutorialCallback(CCObject* pSender)
{
    lock();
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    isTutorial = false;
	tutorialPopup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -tutorialPopup->getContentSize().height))));
	tutorialMenu->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -tutorialPopup->getContentSize().height))));
	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(POPUP_SHOW_TIME), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::tutorialFinished))));
	menu->setEnabled(true);
    menuLeft->setEnabled(true);
    setting->setVisible(true);
}

void MainMenuScene::nextCallback(CCObject* pSender)
{
    lock();
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
	helpNum++;
	if (helpNum == 3)
	{
        helpNum = 0;
        ((CCMenuItemSprite*)pSender)->setNormalImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
        ((CCMenuItemSprite*)pSender)->setSelectedImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
        ((CCSprite*)(((CCMenuItemSprite*)pSender)->getSelectedImage()))->setColor(ccGRAY);
        
        labelTTF = CCLabelTTF::create(CCLocalizedString("NEXT", NULL), FONT_COMMON, FONT_SIZE_86);
        labelTTF->setColor(ccWHITE);
        labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
        ((CCMenuItemSprite*)pSender)->addChild(labelTTF);
        labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
        
		closeSettingCallback(pSender);
        tutorialText->setString(" ");
        tutorial->setVisible(false);
        return;
	}
    if (helpNum == 2)
	{
		((CCMenuItemSprite*)pSender)->setNormalImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
        ((CCMenuItemSprite*)pSender)->setSelectedImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
        ((CCSprite*)(((CCMenuItemSprite*)pSender)->getSelectedImage()))->setColor(ccGRAY);
        
        labelTTF = CCLabelTTF::create(CCLocalizedString("NEXT", NULL), FONT_COMMON, FONT_SIZE_86);
        labelTTF->setColor(ccWHITE);
        labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
        ((CCMenuItemSprite*)pSender)->addChild(labelTTF);
        labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
	}

	tutorial->removeFromParentAndCleanup(true);

	if (helpNum == 0)
	{
		tutorial = CCSprite::createWithSpriteFrameName("common/1.png");
		tutorial->setPosition(ccp(popup->getContentSize().width / 2.7f, popup->getContentSize().height / 1.27f));
		popup->addChild(tutorial);
		tutorial->setOpacity(0);
		tutorial->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    }
	else if (helpNum == 1)
	{
		tutorial = CCSprite::createWithSpriteFrameName("common/2.png");
		tutorial->setPosition(ccp(popup->getContentSize().width / 1.9f, popup->getContentSize().height / 1.55f));
		popup->addChild(tutorial);
		tutorial->setOpacity(0);
		tutorial->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
        
        tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_2", NULL), FONT_COMMON, FONT_SIZE_54);
        tutorialText->setColor(IceCreamBrown);
        tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
        tutorial->addChild(tutorialText);
        tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/2.0f, tutorialText->getParent()->getContentSize().height/1.25f));
	}
	else if (helpNum == 2)
	{
		tutorial = CCSprite::createWithSpriteFrameName("common/3.png");
		tutorial->setPosition(ccp(popup->getContentSize().width / 2.1f, popup->getContentSize().height / 1.4f));
		popup->addChild(tutorial);
		tutorial->setOpacity(0);
		tutorial->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
        
        tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_3", NULL), FONT_COMMON, FONT_SIZE_54);
        tutorialText->setColor(IceCreamBrown);
        tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
        tutorial->addChild(tutorialText);
        tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/1.9f, tutorialText->getParent()->getContentSize().height/1.4f));
	}
}

void MainMenuScene::askedCallback(CCObject* pSender)
{
    lock();
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    if (settingState != Setting)
        return;
    
    settingState = AskedQuestions;
    
	for (int i = 0; i < popup->getChildrenCount(); i++)
	{
		((CCSprite*)popup->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));
	}
	for (int i = 0; i < popupMenu->getChildrenCount(); i++)
	{
		if (((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i)) != close)
			((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));				
	}
	close->setNormalImage(CCSprite::createWithSpriteFrameName("common/back.png"));
	close->setSelectedImage(CCSprite::createWithSpriteFrameName("common/back_on.png"));
	settingState = AskedQuestions;

	CCSprite* facebook = CCSprite::createWithSpriteFrameName("common/markAsk2.png");
	facebook->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.2f));
	popup->addChild(facebook);
	facebook->setOpacity(0);
	facebook->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));

    label = CCLabelTTF::create(CCLocalizedString("SETTING_ASK_TEXT", NULL), FONT_COMMON, FONT_SIZE_54);
	label->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 2.0f));
	popup->addChild(label);
	label->setOpacity(0);
    ccColor3B color;
    color.r = 0x91;
    color.g = 0x3b;
    color.b = 0x55;
	label->setColor(color);
	label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    CCSprite* facebookButtonButton = CCSprite::createWithSpriteFrameName("common/greenButton.png");
    CCSprite* facebookButtonButtonDown = CCSprite::createWithSpriteFrameName("common/greenButton.png");
    facebookButtonButtonDown->setColor(ccGRAY);


	CCMenuItemSprite* facebookButton = CCMenuItemSprite::create(facebookButtonButton, facebookButtonButtonDown, this, menu_selector(MainMenuScene::goCallback));
	facebookButton->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
		(popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - facebookButton->getContentSize().height*1.2f* 3.0f));
    facebookButton->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
	popupMenu->addChild(facebookButton);
	facebookButton->setOpacity(0);
	facebookButton->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("GO", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    facebookButton->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    faceConnect->setEnabled(false);
    howPlay->setEnabled(false);
    freqAsk->setEnabled(false);
    restorePurshase->setEnabled(false);
    reset->setEnabled(false);
    notification->setEnabled(false);
}

void MainMenuScene::goCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    goToLink("http://ica.ddestiny.ru/");
}

void MainMenuScene::restoreCallback(CCObject* pSender)
{
    lock();
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    if (settingState != Setting)
        return;
    
    settingState = Restore;
    
	for (int i = 0; i < popup->getChildrenCount(); i++)
	{
		((CCSprite*)popup->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));				
	}
	for (int i = 0; i < popupMenu->getChildrenCount(); i++)
	{
		if (((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i)) != close)
			((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));				
	}
	close->setNormalImage(CCSprite::createWithSpriteFrameName("common/back.png"));
	close->setSelectedImage(CCSprite::createWithSpriteFrameName("common/back_on.png"));
	settingState = Restore;

	CCSprite* facebook = CCSprite::createWithSpriteFrameName("common/markArrow2.png");
	facebook->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.2f));
	popup->addChild(facebook);
	facebook->setOpacity(0);
	facebook->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));

    label = CCLabelTTF::create(CCLocalizedString("SETTING_RESTORE_TEXT", NULL), FONT_COMMON, FONT_SIZE_64);
	label->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 2.0f));
	popup->addChild(label);
	label->setOpacity(0);
    ccColor3B color;
    color.r = 0x91;
    color.g = 0x3b;
    color.b = 0x55;
	label->setColor(color);
	label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    CCSprite* facebookButtonButton = CCSprite::createWithSpriteFrameName("common/greenButton.png");
    CCSprite* facebookButtonButtonDown = CCSprite::createWithSpriteFrameName("common/greenButton.png");
    facebookButtonButtonDown->setColor(ccGRAY);

	CCMenuItemSprite* facebookButton = CCMenuItemSprite::create(facebookButtonButton,
		facebookButtonButtonDown, this, menu_selector(MainMenuScene::restoreInappCallback));
	facebookButton->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
		(popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - facebookButton->getContentSize().height*1.2f* 3.0f));
    facebookButton->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
	popupMenu->addChild(facebookButton);
	facebookButton->setOpacity(0);
	facebookButton->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("RESTORE", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    facebookButton->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    faceConnect->setEnabled(false);
    howPlay->setEnabled(false);
    freqAsk->setEnabled(false);
    restorePurshase->setEnabled(false);
    reset->setEnabled(false);
    notification->setEnabled(false);
}

void MainMenuScene::restoreInappCallback(CCObject* pSender)
{
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::closeSettingCallback))));
    IAP::sharedInstance().restoreCompletedTransactions();
}

void MainMenuScene::resetCallback(CCObject* pSender)
{
    lock();
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    if (settingState != Setting)
        return;
    
    settingState = Reset;
    
	for (int i = 0; i < popup->getChildrenCount(); i++)
	{
		((CCSprite*)popup->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));
	}
	for (int i = 0; i < popupMenu->getChildrenCount(); i++)
	{
		if (((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i)) != close)
			((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));				
	}
	close->setNormalImage(CCSprite::createWithSpriteFrameName("common/back.png"));
	close->setSelectedImage(CCSprite::createWithSpriteFrameName("common/back_on.png"));
	settingState = Reset;

	CCSprite* facebook = CCSprite::createWithSpriteFrameName("common/markRotate2.png");
	facebook->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.2f));
	popup->addChild(facebook);
	facebook->setOpacity(0);
	facebook->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));

    label = CCLabelTTF::create(CCLocalizedString("SETTING_RESET_TEXT", NULL), FONT_COMMON, FONT_SIZE_64);
	label->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 2.0f));
	popup->addChild(label);
	label->setOpacity(0);
    ccColor3B color;
    color.r = 0x91;
    color.g = 0x3b;
    color.b = 0x55;
	label->setColor(color);
	label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    CCSprite* facebookButtonButton = CCSprite::createWithSpriteFrameName("common/redButton.png");
    CCSprite* facebookButtonButtonDown = CCSprite::createWithSpriteFrameName("common/redButton.png");
    facebookButtonButtonDown->setColor(ccGRAY);

	CCMenuItemSprite* facebookButton = CCMenuItemSprite::create(facebookButtonButton,
		facebookButtonButtonDown, this, menu_selector(MainMenuScene::resetPopup));
    
	facebookButton->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
		(popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - facebookButton->getContentSize().height*1.2f* 3.0f));
    facebookButton->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
	popupMenu->addChild(facebookButton);
	facebookButton->setOpacity(0);
	facebookButton->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("RESET", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    facebookButton->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    faceConnect->setEnabled(false);
    howPlay->setEnabled(false);
    freqAsk->setEnabled(false);
    restorePurshase->setEnabled(false);
    reset->setEnabled(false);
    notification->setEnabled(false);
}

void MainMenuScene::resetPopup(CCObject* pSender)
{
    menu->setEnabled(false);
    popupMenu->setEnabled(false);
    tutorialMenu->setEnabled(false);
    popupLayer->popup((char*)CCLocalizedString("RESET", NULL), (char*)CCLocalizedString("RESET_TEXT", NULL), (char*)CCLocalizedString("RESET", NULL), GreenPopup, this, callfuncN_selector(MainMenuScene::resetPopupOk), this, callfuncN_selector(MainMenuScene::unclockMenu));
}

void MainMenuScene::addBonus(CCNode* pSender)
{
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    
    setting->setEnabled(true);
    if (FacebookPtr->sessionIsOpened() && getNetworkStatus())
    {
        FacebookPtr->inviteFriends();
    }
    else
    {
        FacebookPtr->loginWithInvite();
    }
}

void MainMenuScene::resetPopupOk(CCNode* pSender)
{
    OptionsPtr->restoreCurrentLevel(85);
    OptionsPtr->save();
/*    if (getNetworkStatus() && FacebookPtr->sessionIsOpened())
    {
        FacebookPtr->logout();
        facebook->setVisible(true);
        faceConnect->setVisible(true);
        faceConnect->setEnabled(true);
        OptionsPtr->setFacebookConnection(false);
        OptionsPtr->save();
    }*/
}

void MainMenuScene::unclockMenu(CCNode* pSender)
{
    menu->setEnabled(true);
    popupMenu->setEnabled(true);
    tutorialMenu->setEnabled(true);
    setting->setEnabled(true);
}

void MainMenuScene::facebookPopupConncetion(CCObject* pSender)
{
    menu->setEnabled(false);
    popupMenu->setEnabled(false);
    tutorialMenu->setEnabled(false);
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::facebookUnlock))));
}
                    
void MainMenuScene::facebookUnlock(CCNode* pSender)
{
    if (FacebookPtr->isLoad)
    {
        unclockMenu(NULL);
    }
    else
    {
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::facebookUnlock))));
    }
}

void MainMenuScene::facebookPopupOk(CCNode* pSender)
{
    
}

void MainMenuScene::notificationCallback(CCObject* pSender)
{
    lock();
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    if (settingState != Setting)
        return;
    
    settingState = Notification;
    
	for (int i = 0; i < popup->getChildrenCount(); i++)
	{
		((CCSprite*)popup->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));				
	}
	for (int i = 0; i < popupMenu->getChildrenCount(); i++)
	{
		if (((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i)) != close)
			((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i))->runAction(CCFadeOut::create(0.2f));				
	}
	close->setNormalImage(CCSprite::createWithSpriteFrameName("common/back.png"));
	close->setSelectedImage(CCSprite::createWithSpriteFrameName("common/back_on.png"));
	settingState = Notification;

	CCSprite* facebook = CCSprite::createWithSpriteFrameName("common/markNotif2.png");
	facebook->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.2f));
	popup->addChild(facebook);
	facebook->setOpacity(0);
	facebook->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));

    label = CCLabelTTF::create(CCLocalizedString("SETTING_NOTIF_TEXT", NULL), FONT_COMMON, FONT_SIZE_64);
	label->setPosition(ccp(popup->getContentSize().width / 2.0f - popup->getContentSize().width/4.0f, popup->getContentSize().height / 1.75f));
	popup->addChild(label);
	label->setOpacity(0);
    ccColor3B color;
    color.r = 0x91;
    color.g = 0x3b;
    color.b = 0x55;
	label->setColor(color);
	label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));

	notif_1_Button = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/offButton.png"),
		CCSprite::createWithSpriteFrameName("common/onButton.png"), this, menu_selector(MainMenuScene::notif_1_Callback));
	notif_1_Button->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f  + notif_1_Button->getContentSize().width*0.9f,
		(popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + notif_1_Button->getContentSize().height));
    notif_1_Button->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
	popupMenu->addChild(notif_1_Button);
	notif_1_Button->setOpacity(0);
	notif_1_Button->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    if (OptionsPtr->isNotification())
	{
		notif_1_Button->setNormalImage(CCSprite::createWithSpriteFrameName("common/onButton.png"));
        
        labelTTF = CCLabelTTF::create(CCLocalizedString("ON", NULL), FONT_COMMON, FONT_SIZE_64);
        labelTTF->setColor(ccWHITE);
        labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
        notif_1_Button->addChild(labelTTF);
        labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
	}
	else
	{
		notif_1_Button->setNormalImage(CCSprite::createWithSpriteFrameName("common/offButton.png"));
        
        labelTTF = CCLabelTTF::create(CCLocalizedString("OFF", NULL), FONT_COMMON, FONT_SIZE_64);
        labelTTF->setColor(ccWHITE);
        labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
        notif_1_Button->addChild(labelTTF);
        labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
	}
    
    faceConnect->setEnabled(false);
    howPlay->setEnabled(false);
    freqAsk->setEnabled(false);
    restorePurshase->setEnabled(false);
    reset->setEnabled(false);
    notification->setEnabled(false);

}

void MainMenuScene::notif_1_Callback(CCObject* pSender)
{
	CCSprite* spriteOn = CCSprite::createWithSpriteFrameName("common/onButton.png");
    
    CCLabelTTF* labelOn = CCLabelTTF::create(CCLocalizedString("ON", NULL), FONT_COMMON, FONT_SIZE_86);
    labelOn->setColor(ccWHITE);
    labelOn->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    spriteOn->addChild(labelOn);
    labelOn->setPosition(ccp(labelOn->getParent()->getContentSize().width/2.0f, labelOn->getParent()->getContentSize().height/2.0f));
    
    CCSprite* spriteOff = CCSprite::createWithSpriteFrameName("common/offButton.png");
    
    CCLabelTTF* labelOff = CCLabelTTF::create(CCLocalizedString("OFF", NULL), FONT_COMMON, FONT_SIZE_86);
    labelOff->setColor(ccWHITE);
    labelOff->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    spriteOff->addChild(labelOff);
    labelOff->setPosition(ccp(labelOff->getParent()->getContentSize().width/2.0f, labelOff->getParent()->getContentSize().height/2.0f));
    
	if (OptionsPtr->isNotification())
	{
        OptionsPtr->setNotification(false);
		notif_1_Button->setNormalImage(spriteOff);
        notif_1_Button->setNormalImage(spriteOff);
        removeAllNotification();
	}
	else
	{
        OptionsPtr->setNotification(true);
        notif_1_Button->setNormalImage(spriteOn);
        notif_1_Button->setNormalImage(spriteOn);
	}
    OptionsPtr->save();
}

void MainMenuScene::closeFinished(CCNode* sender)
{
 	popup->setVisible(false);
	popupMenu->setVisible(false);
	menu->setEnabled(true);
    menuLeft->setEnabled(true);
    setting->setVisible(true);
}

void MainMenuScene::closeSettingCallback(CCObject* pSender)
{
    lock();
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
	if (settingState == Setting)
	{
        isPopup = false;
		popup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -popup->getContentSize().height))));
		popupMenu->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -popup->getContentSize().height))));
		this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(POPUP_SHOW_TIME), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::closeFinished))));
	}
	else
	{
        faceConnect->setEnabled(true);
        howPlay->setEnabled(true);
        freqAsk->setEnabled(true);
        restorePurshase->setEnabled(true);
        reset->setEnabled(true);
        notification->setEnabled(true);
        
        tutorialText->setString(" ");
        if (tutorial)
            tutorial->setVisible(false);
        
		label->removeFromParentAndCleanup(true);
        if (settingState == Notification)
        {
            notif_1_Button->setVisible(false);
        }
		for (int i = 0; i < popup->getChildrenCount(); i++)
		{
			if (((CCSprite*)popup->getChildren()->objectAtIndex(i))->getOpacity() == 255)
				((CCSprite*)popup->getChildren()->objectAtIndex(i))->removeFromParentAndCleanup(true);			
		}
		for (int i = 0; i < popupMenu->getChildrenCount(); i++)
		{			
			if (((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i)) != close)
				if (((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i))->getOpacity() == 255)
					((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i))->removeFromParentAndCleanup(true);
		}

		for (int i = 0; i < popup->getChildrenCount(); i++)
		{			
			((CCSprite*)popup->getChildren()->objectAtIndex(i))->runAction(CCFadeIn::create(0.5f));
		}
		for (int i = 0; i < popupMenu->getChildrenCount(); i++)
		{			
			if (((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i)) != close)
				((CCMenuItem*)popupMenu->getChildren()->objectAtIndex(i))->runAction(CCFadeIn::create(0.5f));				
		}

		close->setNormalImage(CCSprite::createWithSpriteFrameName("common/close.png"));
		close->setSelectedImage(CCSprite::createWithSpriteFrameName("common/close_on.png"));
		settingState = Setting;
	}
}

void MainMenuScene::menuGameCenterCallback(CCObject* pSender)
{
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    popupLayer->loading((char*)CCLocalizedString("CONNECTION", NULL));
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(CONNECTION_TIME/3.0f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::closeLoading))));
    play->setEnabled(false);
    moreGames->setEnabled(false);
    menu->setEnabled(false);
    loginGC();
}

void MainMenuScene::menuFacebookCallback(CCObject* pSender)
{
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    
    vector<sRequestData> requests = OptionsPtr->appRequests;
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    FacebookPtr->login();
    
    popupLayer->loading((char*)CCLocalizedString("CONNECTION", NULL));
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(CONNECTION_TIME/7.0f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::closeLoading))));
    play->setEnabled(false);
    moreGames->setEnabled(false);
    menu->setEnabled(false);
    
    if (settingState == Facebook)
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::closeSettingCallback))));
}

void MainMenuScene::closeLoading()
{
    if (!menu->isEnabled())
    {
        play->setEnabled(true);
        moreGames->setEnabled(true);
        menu->setEnabled(true);
        popupLayer->closeLoading();
    }
}

#include "GameScene.h"

void MainMenuScene::menuPlayCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");    
    if (IPAD)
    {
        if (LANDSCAPE)
            spriteLoading = CCSprite::create("loadingiPadLandscape.png");
        else
            spriteLoading = CCSprite::create("loadingiPadPortrait.png");
    }
    else if (IPAD_MINI)
    {
        if (LANDSCAPE)
            spriteLoading = CCSprite::create("loadingiPadMiniLandscape.png");
        else
            spriteLoading = CCSprite::create("loadingiPadMiniPortrait.png");
    }
    else if (IPHONE_4||IPHONE_5)
    {
        if (LANDSCAPE)
            spriteLoading = CCSprite::create("loadingIphoneLanscape.png");
        else
            spriteLoading = CCSprite::create("loadingIphonePortrait.png");
    }
    labelLoad = CCLabelTTF::create(CCLocalizedString("LOADING", NULL), FONT_COMMON, FONT_SIZE_86);
    labelLoad->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/10.0f));
    this->addChild(labelLoad, 1001);
    spriteLoading->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    this->addChild(spriteLoading, 1000);
    spriteLoading->setOpacity(255);
    labelLoad->setOpacity(255);
    
    if (getNetworkStatus() && FacebookPtr->sessionIsOpened() && OptionsPtr->isFacebookConnection())
    {
        GlobalsPtr->isLoadMap = true;
        FacebookPtr->getScores();
    }
    else
        this->runAction(CCSequence::create(CCDelayTime::create(0.7f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::playWithLoading)), NULL));
}

void MainMenuScene::playWithLoading(CCNode* pSender)
{
    CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene());
}

void MainMenuScene::settingFinished(CCNode* sender)
{
    if (isSetting)
    {
        sound->setVisible(true);
        sound->setScale(0.1f);
        sound->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, settingBlob->getScale())));

        music->setVisible(true);
        music->setScale(0.1f);
        music->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, settingBlob->getScale())));

        ask->setVisible(true);
        ask->setScale(0.1f);
        ask->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, settingBlob->getScale())));

        cogwheel->setVisible(true);
        cogwheel->setScale(0.1f);
        cogwheel->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, settingBlob->getScale())));

        closeLeft->setVisible(true);
        closeLeft->setScale(0.1f);
        closeLeft->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, settingBlob->getScale())));

        invite->setVisible(true);
        invite->setScale(0.1f);
        invite->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, settingBlob->getScale())));
    
    }
    isLock = false;
}

void MainMenuScene::menuSettingCallback(CCObject* pSender)
{
#ifdef NEW_ART
    if (isLock)
        return;
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    lock();
    
	if (isSetting)
	{
        settingBlob->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -WINSIZE.height))));
		isSetting = false;
		sound->setVisible(false);
		music->setVisible(false);
		ask->setVisible(false);
		cogwheel->setVisible(false);
        closeLeft->setVisible(false);
        invite->setVisible(false);
        menu->setEnabled(true);
        isLock = true;
        settingBlob->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::settingFinished))));
        setting->setVisible(true);
	}
	else
	{
		settingBlob->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, WINSIZE.height))));
		isSetting = true;
		settingBlob->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::settingFinished))));
        setting->setVisible(false);
        menu->setEnabled(false);
        isLock = true;
	}
#else
    if (isLock)
        return;
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    lock();
    
	if (isSetting)
	{
		settingBlob->runAction(CCMoveBy::create(0.3f, ccp(-settingBlob->getContentSize().width, -settingBlob->getContentSize().height)));
		isSetting = false;
		sound->setVisible(false);
		music->setVisible(false);
		ask->setVisible(false);
		cogwheel->setVisible(false);
        invite->setVisible(false);
        isLock = true;
        settingBlob->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::settingFinished))));
	}
	else
	{
		settingBlob->runAction(CCMoveBy::create(0.3f, ccp(settingBlob->getContentSize().width, settingBlob->getContentSize().height)));
		isSetting = true;
		settingBlob->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::settingFinished))));
        isLock = true;
	}
#endif
}

void MainMenuScene::soundCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
	if (isSound)
	{
		SimpleAudioEngine::sharedEngine()->setEffectsVolume(0.0f);
		isSound = false;
	}
	else
	{
		SimpleAudioEngine::sharedEngine()->setEffectsVolume(1.0f);
		isSound = true;
	}
	OptionsPtr->setUseSounds(isSound);
	OptionsPtr->save();
    
#ifdef NEW_ART
    if (isSound)
		((CCLabelTTF*)soundClose)->setString(CCLocalizedString("ON", NULL));
	else
		((CCLabelTTF*)soundClose)->setString(CCLocalizedString("OFF", NULL));
#else
	if (isSound)
		soundClose->setVisible(false);
	else
		soundClose->setVisible(true);
#endif
}

void MainMenuScene::musicCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
	if (isMusic)
	{
		SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.0f);
		isMusic = false;
	}
	else
	{
		SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(1.0f);
		isMusic = true;
	}
	OptionsPtr->setUseMusic(isMusic);
	OptionsPtr->save();
    
#ifdef NEW_ART
    if (isMusic)
		((CCLabelTTF*)musicClose)->setString(CCLocalizedString("ON", NULL));
	else
		((CCLabelTTF*)musicClose)->setString(CCLocalizedString("OFF", NULL));
#else
	if (isMusic)
		musicClose->setVisible(false);
	else
		musicClose->setVisible(true);
#endif
}

void MainMenuScene::helpCallback(CCObject* pSender)
{
    lock();
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    isTutorial = true;
	tutorialMenu->setVisible(true);
	tutorialPopup->setVisible(true);

	tutorialPopup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, tutorialPopup->getContentSize().height))));
	tutorialMenu->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, tutorialPopup->getContentSize().height))));
    
    tutorialClose->stopAllActions();
    tutorialClose->setScale(0.7f);
	tutorialClose->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    tutorialNext->stopAllActions();
    tutorialNext->setScale(0.7f);
	tutorialNext->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_1", NULL), FONT_COMMON, FONT_SIZE_48);
    tutorialText->setColor(IceCreamBrown);
    tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    tutorialSprite->addChild(tutorialText);
    tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/1.5f, tutorialText->getParent()->getContentSize().height/2.0f));
    
    tutorialNum = 0;
    tutorialNext->setNormalImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
    tutorialNext->setSelectedImage(CCSprite::createWithSpriteFrameName("common/redButton.png"));
    ((CCSprite*)((tutorialNext)->getSelectedImage()))->setColor(ccGRAY);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("NEXT", NULL), FONT_COMMON, FONT_SIZE_86);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    tutorialNext->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));


	menuSettingCallback(NULL);
	menu->setEnabled(false);
    setting->setVisible(false);
}

void MainMenuScene::cogwheelCallback(CCObject* pSender)
{
    lock();
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    isPopup = true;
    close->setNormalImage(CCSprite::createWithSpriteFrameName("common/close.png"));
    close->setSelectedImage(CCSprite::createWithSpriteFrameName("common/close_on.png"));
    settingState = Setting;
	menu->setEnabled(false);
    menuLeft->setEnabled(false);
	popup->setVisible(true);
	popupMenu->setVisible(true);
	
	popup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, popup->getContentSize().height))));
	popupMenu->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, popup->getContentSize().height))));
    
    close->stopAllActions();
    close->setScale(0.7f);
	close->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));

	settingState = Setting;
	menuSettingCallback(NULL);
    setting->setVisible(false);
}

void MainMenuScene::lock()
{
    this->setTouchEnabled(false);
    menu->setTouchEnabled(false);
    popupMenu->setTouchEnabled(false);
    tutorialMenu->setEnabled(false);
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.6f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::unlock))));
}

void MainMenuScene::unlock(CCNode* sender)
{
    menu->setTouchEnabled(true);
    popupMenu->setTouchEnabled(true);
    tutorialMenu->setEnabled(true);
    this->setTouchEnabled(true);
}

void MainMenuScene::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, 1);
}

void MainMenuScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
#ifndef NEW_ART
    if (isLock)
        return;
    
	if (isSetting)
	{
		settingBlob->runAction(CCMoveBy::create(0.3f, ccp(-settingBlob->getContentSize().width, -settingBlob->getContentSize().height)));
		isSetting = false;
		sound->setVisible(false);
		music->setVisible(false);
		ask->setVisible(false);
		cogwheel->setVisible(false);
        invite->setVisible(false);
        isLock = true;
        settingBlob->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(MainMenuScene::settingFinished))));
	}
#endif
}

