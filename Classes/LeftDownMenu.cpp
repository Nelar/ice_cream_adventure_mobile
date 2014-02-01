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

	settingBlob = CCSprite::createWithSpriteFrameName("common/settingBlob.png");
	settingBlob->setPosition(ccp(settingBlob->getContentSize().width / 2.0f, settingBlob->getContentSize().height / 2.0f));
	this->addChild(settingBlob, 10);

	CCSprite* settingSpriteNormal = CCSprite::create("gameMap/setting.png");
	CCSprite* settingSpriteSelected = CCSprite::create("gameMap/setting.png");
	settingSpriteSelected->setScale(0.9f);
	setting = CCMenuItemSprite::create(settingSpriteNormal, settingSpriteSelected, this, menu_selector(LeftDownMenuScene::menuSettingCallback));
	setting->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width /2.0f + setting->getContentSize().width / 2.5f,
		-CCDirector::sharedDirector()->getWinSize().height /2.0f + setting->getContentSize().height / 2.5f));
    
    CCSprite* inviteNormal = CCSprite::create("inviteFriends.png");
	CCSprite* inviteSelected = CCSprite::create("inviteFriends.png");
    inviteSelected->setColor(ccGRAY);
    
    inviteSelected->setScale(0.9f);
	inviteSelected->setPosition(ccp(inviteSelected->getContentSize().width/20.0f, inviteSelected->getContentSize().height/20.0f));
	invite = CCMenuItemSprite::create(inviteNormal, inviteSelected, this, menu_selector(LeftDownMenuScene::inviteCallback));
	invite->setPosition(ccp(invite->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f - settingBlob->getContentSize().width / 20.0f,
                            -invite->getContentSize().height/1.3f - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height/0.99f));


	CCSprite* soundNormal = CCSprite::createWithSpriteFrameName("common/sound.png");
	CCSprite* soundSelected = CCSprite::createWithSpriteFrameName("common/sound_selected.png");
	sound = CCMenuItemSprite::create(soundNormal, soundSelected, this, menu_selector(LeftDownMenuScene::soundCallback));
	sound->setPosition(ccp(sound->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f  + settingBlob->getContentSize().width / 5.0f,
                           -sound->getContentSize().height/1.3f - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height/1.05));
	soundClose = CCSprite::createWithSpriteFrameName("common/soundNo.png");
	soundClose->setPosition(ccp(soundNormal->getContentSize().width/2.0f, soundNormal->getContentSize().height/2.0f));
	sound->addChild(soundClose);
	isSound = OptionsPtr->useSounds();
	if (isSound)
		soundClose->setVisible(false);
	else
		soundClose->setVisible(true);

	CCSprite* musicNormal = CCSprite::createWithSpriteFrameName("common/musicButton.png");
	CCSprite* musicSelected = CCSprite::createWithSpriteFrameName("common/music_selected.png");
	music = CCMenuItemSprite::create(musicNormal, musicSelected, this, menu_selector(LeftDownMenuScene::musicCallback));
	music->setPosition(ccp(music->getContentSize().width - CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width / 2.7f,
                           -music->getContentSize().height - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height / 1.17f));
	musicClose = CCSprite::createWithSpriteFrameName("common/soundNo.png");
	musicClose->setPosition(ccp(musicNormal->getContentSize().width/2.0f, musicNormal->getContentSize().height/2.0f));
	music->addChild(musicClose);
	isMusic = OptionsPtr->useMusic();
	if (isMusic)
		musicClose->setVisible(false);
	else
		musicClose->setVisible(true);

	CCSprite* askNormal = CCSprite::createWithSpriteFrameName("common/ask.png");
	CCSprite* askSelected = CCSprite::createWithSpriteFrameName("common/ask_selected.png");
	ask = CCMenuItemSprite::create(askNormal, askSelected, this, menu_selector(LeftDownMenuScene::helpCallback));
	ask->setPosition(ccp(ask->getContentSize().width/1.3f - CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width / 1.7f,
                         -ask->getContentSize().height - CCDirector::sharedDirector()->getWinSize().height/2.0f + settingBlob->getContentSize().height / 1.6f));

	CCSprite* cogwheelNormal = CCSprite::createWithSpriteFrameName("common/exit.png");
	CCSprite* cogwheelSelected = CCSprite::createWithSpriteFrameName("common/exit_selected.png");
	cogwheel = CCMenuItemSprite::create(cogwheelNormal, cogwheelSelected, this, menu_selector(LeftDownMenuScene::cogwheelCallback));
	cogwheel->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f + settingBlob->getContentSize().width - cogwheel->getContentSize().width/1.25f,
                              cogwheel->getContentSize().height/2.0f - CCDirector::sharedDirector()->getWinSize().height/2.0f));

	settingBlob->setPosition(ccp(-settingBlob->getContentSize().width / 2.0f, -settingBlob->getContentSize().height / 2.0f));

	sound->setVisible(false);
	music->setVisible(false);
	ask->setVisible(false);
	cogwheel->setVisible(false);
    invite->setVisible(false);

	menu = CCMenu::create(setting, sound, music, ask, cogwheel, invite, NULL);
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

	tutorialNext = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/next.png"),
		CCSprite::createWithSpriteFrameName("common/nextDown.png"), this, menu_selector(LeftDownMenuScene::tutorialCallback));
	tutorialNext->setPosition(tutorialPopup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
		(tutorialPopup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - tutorialNext->getContentSize().height*1.2f* 3.0f));
    tutorialNext->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));

	tutorialSprite = CCSprite::create("1_1.png");
	tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 2.7f, tutorialPopup->getContentSize().height / 1.27f));
	tutorialPopup->addChild(tutorialSprite);
	tutorialSprite->setOpacity(0);
	tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCFadeIn::create(0.5f)));

	tutorialMenu = CCMenu::create(tutorialClose, tutorialNext, NULL);

	this->addChild(tutorialPopup, 10);
	this->addChild(tutorialMenu, 10);

	tutorialPopup->setVisible(false);
	tutorialMenu->setVisible(false);

	tutorialPopup->setPosition(ccp(tutorialPopup->getPositionX(), tutorialPopup->getPositionY() - tutorialPopup->getContentSize().height));
	tutorialMenu->setPosition(ccp(tutorialMenu->getPositionX(), tutorialMenu->getPositionY() - tutorialPopup->getContentSize().height));
	
	setting->setPosition(ccp(setting->getPositionX() - 500, setting->getPositionY() - 500));
	setting->runAction(CCMoveBy::create(0.5f, ccp(500, 500.0f)));
    
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
//    CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}


void LeftDownMenuScene::inviteCallback(CCObject* pSender)
{
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
	setting->runAction(CCMoveBy::create(0.5f, ccp(-500, -500.0f)));
}

bool LeftDownMenuScene::isLock()
{
	return isOpen;
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
        ((CCMenuItemSprite*)pSender)->setNormalImage(CCSprite::createWithSpriteFrameName("common/next.png"));
        ((CCMenuItemSprite*)pSender)->setSelectedImage(CCSprite::createWithSpriteFrameName("common/nextDown.png"));
		closeTutorialCallback(pSender);
	}
    if (tutorialNum == 2)
	{
		((CCMenuItemSprite*)pSender)->setNormalImage(CCSprite::createWithSpriteFrameName("common/ok.png"));
        ((CCMenuItemSprite*)pSender)->setSelectedImage(CCSprite::createWithSpriteFrameName("common/okDown.png"));
	}
    
	tutorialSprite->removeFromParentAndCleanup(true);
    
	if (tutorialNum == 0)
	{
		tutorialSprite = CCSprite::create("1_1.png");
		tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 2.7f, tutorialPopup->getContentSize().height / 1.27f));
		tutorialPopup->addChild(tutorialSprite);
		tutorialSprite->setOpacity(0);
		tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
	}
	else if (tutorialNum == 1)
	{
		tutorialSprite = CCSprite::create("2_2.png");
		tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 1.9f, tutorialPopup->getContentSize().height / 1.55f));
		tutorialPopup->addChild(tutorialSprite);
		tutorialSprite->setOpacity(0);
		tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
	}
	else if (tutorialNum == 2)
	{
		tutorialSprite = CCSprite::create("3_3.png");
		tutorialSprite->setPosition(ccp(tutorialPopup->getContentSize().width / 2.1f, tutorialPopup->getContentSize().height / 1.4f));
		tutorialPopup->addChild(tutorialSprite);
		tutorialSprite->setOpacity(0);
		tutorialSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
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
	tutorialPopup->setVisible(false);
	tutorialMenu->setVisible(false);
	menu->setEnabled(true);
	islock = false;
}

void LeftDownMenuScene::settingFinished(CCNode* sender)
{
	sound->setVisible(true);
	sound->setScale(0.1f);
	sound->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)));
	music->setVisible(true);
	music->setScale(0.1f);
	music->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)));
	ask->setVisible(true);
	ask->setScale(0.1f);
	ask->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)));
	cogwheel->setVisible(true);
	cogwheel->setScale(0.1f);
	cogwheel->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)));
    invite->setVisible(true);
	invite->setScale(0.1f);
	invite->runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)));
}

void LeftDownMenuScene::menuSettingCallback(CCObject* pSender)
{
    if (islock)
        return;
    lock();
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
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
	if (isSound)
		soundClose->setVisible(false);
	else
		soundClose->setVisible(true);
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
	if (isMusic)
		musicClose->setVisible(false);
	else
		musicClose->setVisible(true);
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