#include "cPopupLayer.h"
#include "cGlobal.h"
#include "Options.h"
#include "SimpleAudioEngine.h"
#include "MMPInterface.h"

using namespace cocos2d;
using namespace CocosDenshion;


bool PopupLayer::init()
{
	if (!CCLayer::init())
		return false;
    
    background = CCLayerColor::create(ccc4(0.0f, 0.0f, 0.0f, 160.0f));
    background->setContentSize(CCSize(WINSIZE.width*2.0f, WINSIZE.height*2.0f));
    background->setAnchorPoint(ccp(-0.3f, -0.3f));
    this->addChild(background);
    background->setVisible(false);
	return true;
}

PopupLayer::~PopupLayer()
{
}

void PopupLayer::popup(char* title, char* text, char* buttonText, ePopupColor popupColor, CCObject* npSelectorTarget, SEL_CallFuncN nselector,
                       CCObject* ncloseSelectorTarget, SEL_CallFuncN ncloseselector)
{
    pSelectorTarget = npSelectorTarget;
    selector = nselector;
    closeSelector = ncloseselector;
    closeTarget = ncloseSelectorTarget;
    
    background->setVisible(true);
    
    if (popupColor == GreenPopup)
        plate = CCSprite::createWithSpriteFrameName("common/greenPlate.png");
    else if (popupColor == PinkPopup)
        plate = CCSprite::createWithSpriteFrameName("common/pinkPlate.png");
    else if (popupColor == GrayPopup)
        plate = CCSprite::createWithSpriteFrameName("common/greyPlate.png");
    
    plate->setPosition(ccp(WINSIZE.width/2.0f + WINSIZE.width, WINSIZE.height/2.0f));
    this->addChild(plate);
    
    CCMenu* menu = CCMenu::create();
    menu->setAnchorPoint(ccp(0.0f, 0.0f));
    menu->setContentSize(plate->getContentSize());
    menu->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().height/2.0f));
    plate->addChild(menu);
    
    CCLabelTTF* titleLabel = CCLabelTTF::create(title, FONT_COMMON, FONT_SIZE_36);
    titleLabel->setPosition(ccp(plate->getContentSize().width/2.0f, 9.0f*plate->getContentSize().height/10.0f));
    if (!IPAD)
        titleLabel->setScale(0.5f);
    plate->addChild(titleLabel);
    
    CCLabelTTF* textLabel = CCLabelTTF::create(text, FONT_COMMON, FONT_SIZE_26);
    textLabel->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().width/2.0f));
    plate->addChild(textLabel);
    
    CCMenuItemSprite* close = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"), CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(PopupLayer::closePopup));
    close->setAnchorPoint(ccp(1.0f, 1.0f));
    close->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().height/2.0f));
    menu->addChild(close);
    
    
    CCSprite *buttonSprite = CCSprite::createWithSpriteFrameName("common/redButton.png");
    CCSprite *buttonSpriteDown = CCSprite::createWithSpriteFrameName("common/redButton.png");
    buttonSpriteDown->setColor(ccGRAY);
    
    CCMenuItemSprite* button = CCMenuItemSprite::create(buttonSprite, buttonSpriteDown, this, menu_selector(PopupLayer::buttonClick));
    button->setPosition(ccp(0.0f, -plate->getContentSize().height/3.0f));
    
    labelTTF = CCLabelTTF::create(buttonText, FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    button->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    menu->addChild(button);
    
    plate->runAction(CCMoveBy::create(0.2f, ccp(-WINSIZE.width, 0.0f)));
    
    isPopup = true;
}

void PopupLayer::popupBoosterInApp(char* title, char* text, ePopupColor popupColor,
                       eBoosterPopupType typeBoo,
                       CCObject* npSelectorTarget, SEL_CallFuncN nselector,
                       CCObject* ncloseSelectorTarget, SEL_CallFuncN ncloseselector)
{
    Core::MMPInterface::Instance()->StoreOpened();
    
    isPopup = true;
    pSelectorTarget = npSelectorTarget;
    selector = nselector;
    closeSelector = ncloseselector;
    closeTarget = ncloseSelectorTarget;
    
    char buttonText[255] = "1$";
    
    //background->setVisible(true);
    
    if (popupColor == GreenPopup)
        plate = CCSprite::createWithSpriteFrameName("common/greenPlate.png");
    else if (popupColor == PinkPopup)
        plate = CCSprite::createWithSpriteFrameName("common/pinkPlate.png");
    else if (popupColor == GrayPopup)
        plate = CCSprite::createWithSpriteFrameName("common/greyPlate.png");
    
    plate->setPosition(ccp(WINSIZE.width/2.0f + WINSIZE.width, WINSIZE.height/2.0f));
    this->addChild(plate);
    
    CCMenu* menu = CCMenu::create();
    menu->setAnchorPoint(ccp(0.0f, 0.0f));
    menu->setContentSize(plate->getContentSize());
    menu->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().height/2.0f));
    plate->addChild(menu);
    
    CCLabelTTF* titleLabel = CCLabelTTF::create(title, FONT_COMMON, FONT_SIZE_36);
    titleLabel->setPosition(ccp(plate->getContentSize().width/2.0f, 9.0f*plate->getContentSize().height/10.0f));
    if (!IPAD)
        titleLabel->setScale(0.5f);
    plate->addChild(titleLabel);
    
    CCSprite* boost;
    switch (typeBoo) {
        case CrystalPopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/crystal.png");
            break;
        case BombPopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/bomb.png");
            break;
        case FishPopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/fish.png");
            break;
        case MovesPopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/arrow5.png");
            break;
        case TimePopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/time15.png");
            break;
        case ChupaPopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/clearBoost.png");
            break;
    }
    
    boost->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().height/1.4f));
    plate->addChild(boost);
    
    CCLabelTTF* textLabel = CCLabelTTF::create(text, FONT_COMMON, FONT_SIZE_26);
    textLabel->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().width/3.0f));
    plate->addChild(textLabel);
    
    CCMenuItemSprite* close = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"), CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(PopupLayer::closePopup));
    close->setAnchorPoint(ccp(1.0f, 1.0f));
    close->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().height/2.0f));
    menu->addChild(close);
    
    CCSprite *buttonSprite = CCSprite::createWithSpriteFrameName("common/redButton.png");
    CCSprite *buttonSpriteDown = CCSprite::createWithSpriteFrameName("common/redButton.png");
    buttonSpriteDown->setColor(ccGRAY);
    
    CCMenuItemSprite* button = CCMenuItemSprite::create(buttonSprite, buttonSpriteDown, this, menu_selector(PopupLayer::buttonClick));
    button->setPosition(ccp(0.0f, -plate->getContentSize().height/3.0f));
    
    labelTTF = CCLabelTTF::create(buttonText, FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    button->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    plate->runAction(CCMoveBy::create(0.2f, ccp(-WINSIZE.width, 0.0f)));
}

void PopupLayer::popupPost(char* title, char* text, char* buttonText, ePopupColor popupColor,
               eBoosterPopupType typeBoo,
               CCObject* npSelectorTarget, SEL_CallFuncN nselector,
               CCObject* ncloseSelectorTarget, SEL_CallFuncN ncloseselector)
{
    
    isPopup = true;
    pSelectorTarget = npSelectorTarget;
    selector = nselector;
    closeSelector = ncloseselector;
    closeTarget = ncloseSelectorTarget;
    
    if (popupColor == GreenPopup)
        plate = CCSprite::createWithSpriteFrameName("common/greenPlate.png");
    else if (popupColor == PinkPopup)
        plate = CCSprite::createWithSpriteFrameName("common/pinkPlate.png");
    else if (popupColor == GrayPopup)
        plate = CCSprite::createWithSpriteFrameName("common/greyPlate.png");
    
    plate->setPosition(ccp(WINSIZE.width/2.0f + WINSIZE.width, WINSIZE.height/2.0f));
    this->addChild(plate);
    
    CCMenu* menu = CCMenu::create();
    menu->setAnchorPoint(ccp(0.0f, 0.0f));
    menu->setContentSize(plate->getContentSize());
    menu->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().height/2.0f));
    plate->addChild(menu);
    
    CCLabelTTF* titleLabel = CCLabelTTF::create(title, FONT_COMMON, FONT_SIZE_36);
    titleLabel->setPosition(ccp(plate->getContentSize().width/2.0f, 9.0f*plate->getContentSize().height/10.0f));
    if (!IPAD)
        titleLabel->setScale(0.5f);
    plate->addChild(titleLabel);
    
    CCSprite* boost;
    switch (typeBoo) {
        case CrystalPopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/crystal.png");
            break;
        case BombPopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/bomb.png");
            break;
        case FishPopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/fish.png");
            break;
        case MovesPopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/arrow5.png");
            break;
        case TimePopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/time15.png");
            break;
        case ChupaPopBoot:
            boost = CCSprite::createWithSpriteFrameName("common/clearBoost.png");
            break;
    }
    
    boost->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().height/1.4f));
    plate->addChild(boost);
    
    float multiplier = 1.0f;
    if (!IPAD)
        multiplier = 0.5f;
    
    CCLabelTTF* textLabel = CCLabelTTF::create(text, FONT_COMMON, FONT_SIZE_26);
    textLabel->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().width/3.0f));
    plate->addChild(textLabel);
    
    CCMenuItemSprite* close = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"), CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(PopupLayer::closePopup));
    close->setAnchorPoint(ccp(1.0f, 1.0f));
    close->setPosition(ccp(plate->getContentSize().width/2.0f, plate->getContentSize().height/2.0f));
    menu->addChild(close);
    
    CCSprite *buttonSprite = CCSprite::createWithSpriteFrameName("common/redButton.png");
    CCSprite *buttonSpriteDown = CCSprite::createWithSpriteFrameName("common/redButton.png");
    buttonSpriteDown->setColor(ccGRAY);
    
    CCMenuItemSprite* button = CCMenuItemSprite::create(buttonSprite, buttonSpriteDown, this, menu_selector(PopupLayer::buttonClick));
    button->setPosition(ccp(0.0f, -plate->getContentSize().height/3.0f));
    
    labelTTF = CCLabelTTF::create(buttonText, FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    button->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    menu->addChild(button);
    plate->runAction(CCMoveBy::create(0.2f, ccp(-WINSIZE.width, 0.0f)));
}

void PopupLayer::buttonClick(CCObject* pSender)
{
    isPopup = false;
    background->setVisible(false);
    plate->removeFromParentAndCleanup(true);
    this->runAction(CCCallFuncN::create(pSelectorTarget, selector));
    this->runAction(CCCallFuncN::create(closeTarget, closeSelector));
}

void PopupLayer::closePopup(CCObject* pSender)
{
    isPopup = false;
    background->setVisible(false);
    plate->removeFromParentAndCleanup(true);
    this->runAction(CCCallFuncN::create(closeTarget, closeSelector));
}

void PopupLayer::changeOrientation()
{
    if (isPopup)
    {
        background->setContentSize(WINSIZE);
        plate->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    }
    return;
}