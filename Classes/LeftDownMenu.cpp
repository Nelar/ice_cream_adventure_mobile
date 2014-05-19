#include "LeftDownMenu.h"
#include "MainMenuScene.h"
#include "GameMapLayer.h"
#include "cGlobal.h"
#include "Options.h"
#include "SimpleAudioEngine.h"
#include "cFacebook.h"

using namespace cocos2d;
using namespace CocosDenshion;


bool LeftDownMenuScene::init()
{
	if (!CCLayer::init())
		return false;
	tutorialNum = 0;
	isSetting = false;
	isSound = true;
	isMusic = true;
	islock = false;
    isTutorial = false;
    
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

	this->setTouchEnabled(true);

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
	this->addChild(settingBlob, 10);

#ifdef NEW_ART
    CCSprite* settingSpriteNormal = CCSprite::create("updateArt/button.png");
	CCSprite* settingSpriteSelected = CCSprite::create("updateArt/button.png");
    settingSpriteSelected->setAnchorPoint(ccp(0.5f, 0.5f));
    settingSpriteSelected->setColor(ccGRAY);
    settingSpriteSelected->setScale(0.9f);
    settingSpriteSelected->setPosition(ccp(settingSpriteSelected->getContentSize().width/20.0f, settingSpriteSelected->getContentSize().height/20.0f));
    
	setting = CCMenuItemSprite::create(settingSpriteNormal, settingSpriteSelected, this, menu_selector(LeftDownMenuScene::menuSettingCallback));
	setting->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width /2.0f + setting->getContentSize().width / 2.0f,
                             -CCDirector::sharedDirector()->getWinSize().height /2.0f + setting->getContentSize().height / 2.0f));
#else
	CCSprite* settingSpriteNormal = CCSprite::create("gameMap/setting.png");
	CCSprite* settingSpriteSelected = CCSprite::create("gameMap/setting.png");
    settingSpriteSelected->setScale(0.9f);
	setting = CCMenuItemSprite::create(settingSpriteNormal, settingSpriteSelected, this, menu_selector(LeftDownMenuScene::menuSettingCallback));
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
    
    
	invite = CCMenuItemSprite::create(inviteButton, inviteButtonDown, this, menu_selector(LeftDownMenuScene::inviteCallback));
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
	invite = CCMenuItemSprite::create(inviteNormal, inviteSelected, this, menu_selector(LeftDownMenuScene::inviteCallback));
	invite->setPosition(ccp(invite->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f - settingBlob->getContentSize().width / 20.0f,
                            -invite->getContentSize().height/1.3f - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height/0.99f));
#endif


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
    
    
	sound = CCMenuItemSprite::create(soundButton, soundButtonDown, this, menu_selector(LeftDownMenuScene::soundCallback));
	sound->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale());
    
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
	sound = CCMenuItemSprite::create(soundNormal, soundSelected, this, menu_selector(LeftDownMenuScene::soundCallback));
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
    
    
	music = CCMenuItemSprite::create(musicButton, musicButtonDown, this, menu_selector(LeftDownMenuScene::musicCallback));
	music->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale()*2.0f);
    
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
	music = CCMenuItemSprite::create(musicNormal, musicSelected, this, menu_selector(LeftDownMenuScene::musicCallback));
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
    
    
	ask = CCMenuItemSprite::create(askButton, askButtonDown, this, menu_selector(LeftDownMenuScene::helpCallback));
	ask->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale()*3.0f);
    
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
	ask = CCMenuItemSprite::create(askNormal, askSelected, this, menu_selector(LeftDownMenuScene::helpCallback));
	ask->setPosition(ccp(ask->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width / 1.7f,
                         -ask->getContentSize().height - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height / 1.6f));
#endif

#ifdef NEW_ART
    CCSprite *cogwheelButton = CCSprite::create("buttonSetting.png");
    cogwheelButton->setCascadeOpacityEnabled(true);
    
    CCSprite *cogwheelButtonDown = CCSprite::create("buttonSetting.png");
    cogwheelButtonDown->setColor(ccGRAY);
    cogwheelButtonDown->setCascadeOpacityEnabled(true);
    
    mark = CCSprite::createWithSpriteFrameName("common/exit.png");
    markDown = CCSprite::createWithSpriteFrameName("common/exit.png");
    mark->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    markDown->setPosition(ccp(inviteButton->getContentSize().width/7.0f, inviteButton->getContentSize().height/2.0f));
    cogwheelButton->addChild(mark);
    cogwheelButtonDown->addChild(markDown);
    
    
	cogwheel = CCMenuItemSprite::create(cogwheelButton, cogwheelButtonDown, this, menu_selector(LeftDownMenuScene::cogwheelCallback));
	cogwheel->setPosition(invite->getPositionX(), invite->getPositionY() - invite->getContentSize().height*settingBlob->getScale()*4.0f);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("EXIT", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(IceCreamBrown);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    cogwheel->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/1.9f, labelTTF->getParent()->getContentSize().height/2.0f));
#else
	CCSprite* cogwheelNormal = CCSprite::createWithSpriteFrameName("common/cogwheel.png");
	CCSprite* cogwheelSelected = CCSprite::createWithSpriteFrameName("common/cogwheel_selected.png");
	cogwheelSelected->setScale(0.9f);
	cogwheelSelected->setPosition(ccp(cogwheelSelected->getContentSize().width/20.0f, cogwheelSelected->getContentSize().height/20.0f));
	cogwheel = CCMenuItemSprite::create(cogwheelNormal, cogwheelSelected, this, menu_selector(LeftDownMenuScene::cogwheelCallback));
	cogwheel->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width - cogwheel->getContentSize().width/1.25f,
                              cogwheel->getContentSize().height/2.0f - CCDirector::sharedDirector()->getWinSize().height/2.0f));
#endif
    
#ifdef NEW_ART
    closeLeft = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
                                         CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(LeftDownMenuScene::menuSettingCallback));
	
    closeLeft->setPosition(settingBlob->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width /2.5f,
                           settingBlob->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height /2.3f);
     
    closeLeft->setVisible(false);
#endif


	sound->setVisible(false);
	music->setVisible(false);
	ask->setVisible(false);
	cogwheel->setVisible(false);
    invite->setVisible(false);
    
#ifdef NEW_ART
	settingBlob->setPosition(ccp(settingBlob->getPositionX(), settingBlob->getPositionY() - WINSIZE.height));
#else
    settingBlob->setPosition(ccp(-settingBlob->getContentSize().width / 2.0f, -settingBlob->getContentSize().height / 2.0f));
#endif

	menu = CCMenu::create(setting, sound, music, ask, cogwheel, invite, closeLeft, NULL);
	this->addChild(menu, 10);

	tutorialPopup= CCSprite::createWithSpriteFrameName("common/panel.png");
    

	CCPoint posPopup;
	posPopup.y = CCDirector::sharedDirector()->getWinSize().height/2.0f;
	posPopup.x = CCDirector::sharedDirector()->getWinSize().width/2.0f;

	tutorialPopup->setPosition(posPopup);

	tutorialClose = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
		CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(LeftDownMenuScene::closeTutorialCallback));
	tutorialClose->setPosition(tutorialPopup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + tutorialPopup->getContentSize().width /2.5f, 
		tutorialPopup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + tutorialPopup->getContentSize().height /2.3f);
    
    CCSprite *tutorialNextButton = CCSprite::createWithSpriteFrameName("common/redButton.png");
    CCSprite *tutorialNextButtonDown = CCSprite::createWithSpriteFrameName("common/redButton.png");
    tutorialNextButtonDown->setColor(ccGRAY);

	tutorialNext = CCMenuItemSprite::create(tutorialNextButton,
		tutorialNextButtonDown, this, menu_selector(LeftDownMenuScene::tutorialCallback));
	tutorialNext->setPosition(tutorialPopup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
		(tutorialPopup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - tutorialNext->getContentSize().height*1.2f* 3.0f));
    tutorialNext->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("NEXT", NULL), FONT_COMMON, FONT_SIZE_86);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    tutorialNext->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

	tutorialSprite = CCSprite::createWithSpriteFrameName("common/1.png");
	tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 2.7f, tutorialPopup->getContentSize().height / 1.27f));
	tutorialPopup->addChild(tutorialSprite);
	tutorialSprite->setOpacity(0);
	tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCFadeIn::create(0.5f)));
    
    tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_1", NULL), FONT_COMMON, FONT_SIZE_48);
    tutorialText->setColor(IceCreamBrown);
    tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
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

	tutorialPopup->setVisible(false);
	tutorialMenu->setVisible(false);

	tutorialPopup->setPosition(ccp(tutorialPopup->getPositionX(), tutorialPopup->getPositionY() - tutorialPopup->getContentSize().height));
	tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX(), tutorialMenu->getPositionY() - tutorialPopup->getContentSize().height));
	
#ifdef NEW_ART
	setting->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width /2.0f + setting->getContentSize().width / 2.0f,
                             -CCDirector::sharedDirector()->getWinSize().height /2.0f + setting->getContentSize().height / 2.0f));
#else
	setting->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width /2.0f + setting->getContentSize().width / 2.6f,
                             -CCDirector::sharedDirector()->getWinSize().height /2.0f + setting->getContentSize().height / 2.6f));
#endif
    
    if (LANDSCAPE)
    {
        if (IPHONE_4 || IPHONE_5)
        {
            tutorialPopup->setScale(0.64f);
            tutorialMenu->setScale(0.64f);
        }
        else
        {
            tutorialPopup->setScale(0.8f);
            tutorialMenu->setScale(0.8f);

        }
        tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX()*tutorialMenu->getScale(),
                                      tutorialMenu->getPositionY()*tutorialMenu->getScale()));
    }
    else{
        tutorialPopup->setScale(popupScale);
        tutorialMenu->setScale(popupScale);
        tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX()*tutorialMenu->getScale(),
                                      tutorialMenu->getPositionY()*tutorialMenu->getScale()));
    }
    
    changeOrientation();
	return true;
}

LeftDownMenuScene::~LeftDownMenuScene()
{
    this->stopAllActions();
    this->unscheduleAllSelectors();
    this->removeAllChildrenWithCleanup(true);
}


void LeftDownMenuScene::inviteCallback(CCObject* pSender)
{
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    
    isInvite = true;
}

void LeftDownMenuScene::changeOrientation()
{
    tutorialMenu->setContentSize(WINSIZE);
    tutorialMenu->setPosition(WINSIZE.width/2.0f, WINSIZE.height/2.0f);
    
	CCPoint posPopup;
    posPopup.y = CCDirector::sharedDirector()->getWinSize().height/2.0f;
	posPopup.x = CCDirector::sharedDirector()->getWinSize().width/2.0f;
    
	tutorialPopup->setPosition(posPopup);
    
	tutorialClose->setPosition(tutorialPopup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + tutorialPopup->getContentSize().width /2.5f,
                               tutorialPopup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + tutorialPopup->getContentSize().height /2.3f);
    
	tutorialNext->setPosition(tutorialPopup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                              (tutorialPopup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - tutorialNext->getContentSize().height*1.2f* 3.0f));
    
    
	tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 2.7f, tutorialPopup->getContentSize().height / 1.27f));
    
    menu->setContentSize(WINSIZE);
    menu->setPosition(WINSIZE.width/2.0f, WINSIZE.height/2.0f);
    
#ifdef NEW_ART
	setting->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width /2.0f + setting->getContentSize().width / 2.0f,
                             -CCDirector::sharedDirector()->getWinSize().height /2.0f + setting->getContentSize().height / 2.0f));
#else
	setting->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width /2.0f + setting->getContentSize().width / 2.6f,
                             -CCDirector::sharedDirector()->getWinSize().height /2.0f + setting->getContentSize().height / 2.6f));
#endif
    if (LANDSCAPE)
    {
        if (IPHONE_4 || IPHONE_5)
        {
            tutorialPopup->setScale(0.6f);
            tutorialMenu->setScale(0.6f);
        }
        else
        {
            tutorialPopup->setScale(0.76f);
            tutorialMenu->setScale(0.76f);
        }
        tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX()*tutorialMenu->getScale(),
                                      tutorialMenu->getPositionY()*tutorialMenu->getScale()));
    }
    else
    {
        tutorialPopup->setScale(popupScale);
        tutorialMenu->setScale(popupScale);
        tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX()*tutorialMenu->getScale(),
                                      tutorialMenu->getPositionY()*tutorialMenu->getScale()));
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
    
    if (tutorialNum == 0)
		tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 2.7f, tutorialPopup->getContentSize().height / 1.27f));
	else if (tutorialNum == 1)
		tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 1.9f, tutorialPopup->getContentSize().height / 1.55f));
	else if (tutorialNum == 2)
		tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 2.1f, tutorialPopup->getContentSize().height / 1.4f));
    
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

bool LeftDownMenuScene::isIntersect(CCPoint location)
{
    CCPoint endLocation = location;
    endLocation.x -= CCDirector::sharedDirector()->getWinSize().width/2.0f;
    endLocation.y -= CCDirector::sharedDirector()->getWinSize().height/2.0f;
    if (setting->boundingBox().containsPoint(endLocation))
        return true;
    if (sound->boundingBox().containsPoint(endLocation))
        return true;
    if (music->boundingBox().containsPoint(endLocation))
        return true;
    if (ask->boundingBox().containsPoint(endLocation))
        return true;
    if (cogwheel->boundingBox().containsPoint(endLocation))
        return true;
    if (invite->boundingBox().containsPoint(endLocation))
        return true;
    
    return false;
}

void LeftDownMenuScene::hide()
{
	this->setVisible(false);
}

bool LeftDownMenuScene::isLock()
{
	return (isOpen || isTutorial);
}

void LeftDownMenuScene::lock()
{
    islock = true;
    this->setTouchEnabled(false);
    menu->setTouchEnabled(false);
    tutorialMenu->setEnabled(false);
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(LeftDownMenuScene::unlock))));
}

void LeftDownMenuScene::unlock(CCNode* sender)
{
    islock = false;
    menu->setTouchEnabled(true);
    tutorialMenu->setEnabled(true);
    this->setTouchEnabled(true);
}

void LeftDownMenuScene::setBackScene(eBackScene nScene)
{
	backScene = nScene;
}

void LeftDownMenuScene::tutorialCallback(CCObject* pSender)
{
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
    if (tutorialNum == 1)
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
	}
    
	tutorialSprite->removeFromParentAndCleanup(true);
    
	if (tutorialNum == 0)
	{
		tutorialSprite = CCSprite::createWithSpriteFrameName("common/1.png");
		tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 2.7f, tutorialPopup->getContentSize().height / 1.27f));
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
		tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 1.9f, tutorialPopup->getContentSize().height / 1.55f));
		tutorialPopup->addChild(tutorialSprite);
		tutorialSprite->setOpacity(0);
		tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
        
        tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_2", NULL), FONT_COMMON, FONT_SIZE_54);
        tutorialText->setColor(IceCreamBrown);
        tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
        tutorialSprite->addChild(tutorialText);
        tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/2.0f, tutorialText->getParent()->getContentSize().height/1.25f));
	}
	else if (tutorialNum == 2)
	{
		tutorialSprite = CCSprite::createWithSpriteFrameName("common/3.png");
		tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 2.1f, tutorialPopup->getContentSize().height / 1.4f));
		tutorialPopup->addChild(tutorialSprite);
		tutorialSprite->setOpacity(0);
		tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
        
        tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_3", NULL), FONT_COMMON, FONT_SIZE_54);
        tutorialText->setColor(IceCreamBrown);
        tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
        tutorialSprite->addChild(tutorialText);
        tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/1.9f, tutorialText->getParent()->getContentSize().height/1.4f));
	}
}

void LeftDownMenuScene::closeTutorialCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    isTutorial = false;
    islock = false;
	tutorialPopup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -tutorialPopup->getContentSize().height))));
	tutorialMenu->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -tutorialPopup->getContentSize().height))));
	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(LeftDownMenuScene::tutorialFinished))));
	menu->setEnabled(true);
}

void LeftDownMenuScene::tutorialFinished(CCNode* sender)
{
    tutorialSprite->removeFromParentAndCleanup(true);
    tutorialSprite = CCSprite::createWithSpriteFrameName("common/1.png");
    tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 2.7f, tutorialPopup->getContentSize().height / 1.27f));
    tutorialPopup->addChild(tutorialSprite);
    tutorialSprite->setOpacity(0);
    tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    tutorialText = CCLabelTTF::create(CCLocalizedString("HELP_1", NULL), FONT_COMMON, FONT_SIZE_48);
    tutorialText->setColor(IceCreamBrown);
    tutorialText->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    tutorialSprite->addChild(tutorialText);
    tutorialText->setPosition(ccp(tutorialText->getParent()->getContentSize().width/1.5f, tutorialText->getParent()->getContentSize().height/2.0f));
    
	tutorialPopup->setVisible(false);
	tutorialMenu->setVisible(false);
	menu->setEnabled(true);
	islock = false;
    tutorialNum = 0;
}

void LeftDownMenuScene::settingFinished(CCNode* sender)
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
    
#ifdef NEW_ART
    closeLeft->setVisible(true);
	closeLeft->setScale(0.1f);
	closeLeft->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, settingBlob->getScale())));
#endif

    invite->setVisible(true);
	invite->setScale(0.1f);
	invite->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, settingBlob->getScale())));
}

void LeftDownMenuScene::menuSettingCallback(CCObject* pSender)
{
#ifdef NEW_ART
    if (islock)
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
        isOpen = false;
        setting->setVisible(true);
	}
	else
	{
		settingBlob->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, WINSIZE.height))));
		isSetting = true;
		settingBlob->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(LeftDownMenuScene::settingFinished))));
        setting->setVisible(false);
        isOpen = true;
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
        isOpen = false;
	}
	else
	{
		settingBlob->runAction(CCMoveBy::create(0.3f, ccp(settingBlob->getContentSize().width, settingBlob->getContentSize().height)));
		isSetting = true;
		settingBlob->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(LeftDownMenuScene::settingFinished))));
        isOpen = true;
	}
#endif
}

bool LeftDownMenuScene::buttonIsSelected()
{
    if (setting->isSelected()||
        sound->isSelected()||
        music->isSelected()||
        ask->isSelected()||
        cogwheel->isSelected()||
        invite->isSelected())
        return true;
    return false;
}

void LeftDownMenuScene::hideMenu()
{
    if (isSetting)
	{
        lock();
		settingBlob->runAction(CCMoveBy::create(0.3f, ccp(-settingBlob->getContentSize().width, -settingBlob->getContentSize().height)));
		isSetting = false;
		sound->setVisible(false);
		music->setVisible(false);
		ask->setVisible(false);
		cogwheel->setVisible(false);
        closeLeft->setVisible(false);
        invite->setVisible(false);
        isOpen = false;
	}
}

void LeftDownMenuScene::soundCallback(CCObject* pSender)
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

void LeftDownMenuScene::musicCallback(CCObject* pSender)
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

void LeftDownMenuScene::helpCallback(CCObject* pSender)
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
}

void LeftDownMenuScene::cogwheelCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    OptionsPtr->save();
	if (backScene == MainMenu)
		CCDirector::sharedDirector()->replaceScene(MainMenuScene::scene());
	else
		exitPress = true;
}