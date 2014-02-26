//
//  cSocialMenu.cpp
//  IceCreamAdventure
//
//  Created by Nelar on 06.11.13.
//
//

#include "cSocialMenu.h"

#include "cGlobal.h"
#include "Options.h"
#include "SimpleAudioEngine.h"
#include "cFacebook.h"
#include "GameMapLayer.h"

using namespace cocos2d;
using namespace CocosDenshion;

#define AVATAR_WIDTH 128.0f

SocialLayer::~SocialLayer()
{
    
}

bool SocialLayer::init()
{
	if (!CCLayer::init())
		return false;
    
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    
    userNodeSize = 400.0f;
    multiplier = 1.0f;
    countUserIsScoreboard = 20;
    userPosition = 10;
    scoreboard = NULL;
    
    sendButtons.clear();
    
    if (!IPAD)
    {
        userNodeSize = 200.0f;
        multiplier = 0.5f;
    }
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCCallFuncN::create(this, callfuncN_selector(SocialLayer::update))));
	return true;
}

void SocialLayer::changeOrientation()
{
    if (scoreboard)
    {
        scoreboardDelete(NULL);
        showScoreboard(currentLevel);
    }
    if (messageboard)
    {
        messageboardDelete(NULL);
        showMessageboard();
    }
    return;
}

CCLayerColor* SocialLayer::createUserNode(int place, string avatarFileName, string name, int score, string fid)
{
    CCLayerColor* node = CCLayerColor::create(ccc4(128, 128, 0, 0));
    node->setContentSize(CCSize(userNodeSize, userNodeSize));
    
    //avatar
    CCSprite* avatar = CCSprite::create(avatarFileName.c_str());
    avatar->setScaleX(AVATAR_WIDTH/avatar->getContentSize().width*multiplier);
    avatar->setScaleY(AVATAR_WIDTH/avatar->getContentSize().height*multiplier);
    avatar->setPosition(ccp(node->getContentSize().height*0.25f, node->getContentSize().height*0.7f));
    
    //border
    CCSprite* border = CCSprite::create("avatarBorder.png");
    border->setScaleX(((AVATAR_WIDTH + 15)/border->getContentSize().width)/avatar->getScaleX()*multiplier);
    border->setScaleY(((AVATAR_WIDTH + 15)/border->getContentSize().height)/avatar->getScaleY()*multiplier);
    border->setPosition(ccp(avatar->getContentSize().width/2.0f, avatar->getContentSize().height/2.0f));
    avatar->addChild(border);
    
    //name and score
    char buf[255];
    string firstName;
    if (name.find(' ') > 0)
        firstName = name.substr(0, name.find(" "));
    else
        firstName = name;
    sprintf(buf, "%s\n%d", firstName.c_str(), score);
    CCLabelTTF* labelName = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_48);
    labelName->setPosition(ccp(node->getContentSize().width/1.6f, node->getContentSize().height/5.0f));
    labelName->setColor(ccBLACK);
    
    //position
    sprintf(buf, "%d", place);
    CCLabelTTF* userPosition = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_140);
    userPosition->setColor(color);
    userPosition->setPosition(ccp(node->getContentSize().height*0.14f, node->getContentSize().height/5.0f));
    
    CCMenu* fMenu = CCMenu::create();
    node->addChild(fMenu);
    
    CCSprite* lifeSend = CCSprite::create("lifeSend.png");
    CCSprite* lifeSendDown = CCSprite::create("lifeSend.png");
    lifeSendDown->setColor(ccGRAY);
    
    CCMenuItemSprite* sendFriend = CCMenuItemSprite::create(lifeSend, lifeSendDown, this, menu_selector(SocialLayer::sendLifeCallback));
    fMenu->addChild(sendFriend);
    fMenu->setPosition(ccp(0.0f, 0.0f));
    sendFriend->setPosition(ccp(node->getContentSize().height*0.75f, node->getContentSize().height*0.7f));
//    sendFriend->setScale(multiplier);
    sendFriend->setTag(place - 1);
    
    if (!FacebookPtr->sessionIsOpened())
    {
        sendFriend->setVisible(false);
    }
    
    
    CCSprite* separator = CCSprite::create("separator.png");
    if (LANDSCAPE)
    {
        separator->setPosition(ccp(node->getContentSize().width/2.0f, 0.0f));
        separator->setRotation(90.0f);
    }
    else
    {
        separator->setPosition(ccp(node->getContentSize().width, node->getContentSize().height/2.0f));
    }
    separator->setScale(multiplier);
    node->addChild(separator);
    
    
    sprintf(buf, "%lld", FacebookPtr->fbid);
    
    if (fid == string(buf))
        sendFriend->setVisible(false);
    
    
    node->addChild(avatar);
    node->addChild(labelName);
    node->addChild(userPosition);
    
    //node->setScale(multiplier);

    return node;
}

void SocialLayer::update(CCNode*)
{
    if (isConnection)
    {
        if (FacebookPtr->isLoad || timeDt >= 5.0f)
        {
            connectionPlate->removeFromParentAndCleanup(true);
            isConnection = false;
            timeDt=0.0f;
        }
        else
        {
            timeDt+=0.5f;
        }
    }
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCCallFuncN::create(this, callfuncN_selector(SocialLayer::update))));
}

void SocialLayer::showScoreboard(int level)
{
    currentLevel = level;
    scoreboard = createScoreLayer(level);
    if (LANDSCAPE)
    {
        scoreboard->setPosition(ccp(getContentSize().width - scoreboard->getContentSize().width, WINSIZE.height));
        scoreboard->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -WINSIZE.height))));
    }
    else
    {
        scoreboard->setPosition(ccp(-WINSIZE.width, 0.0f));
        scoreboard->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(WINSIZE.width, 0.0f))));
    }
    
    numOneCallback(NULL);
    this->addChild(scoreboard);
}

void SocialLayer::hideScoreBoard()
{
    if (LANDSCAPE)
    {
        scoreboard->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, WINSIZE.height))));
    }
    else
    {
        scoreboard->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(-WINSIZE.width, 0.0f))));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(POPUP_SHOW_TIME), CCCallFuncN::create(this, callfuncN_selector(SocialLayer::scoreboardDelete))));
}

void SocialLayer::showMessageboard()
{
    messageboard = createMessageboard();

    messageboard->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f + WINSIZE.height));
    messageboard->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -WINSIZE.height))));
    
    isMessageBoard = true;

    this->addChild(messageboard);
}

void SocialLayer::hideMessageboard()
{
    isMessageBoard = false;
    messageboard->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, WINSIZE.height))));
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(POPUP_SHOW_TIME), CCCallFuncN::create(this, callfuncN_selector(SocialLayer::messageboardDelete))));
}

void SocialLayer::showAskMessageboard()
{
    messageboard = createMessageboardForAsk();
    
    messageboard->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f + WINSIZE.height));
    messageboard->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -WINSIZE.height))));
    
    isMessageBoard = true;
    
    this->addChild(messageboard);
}

void SocialLayer::hideAskMessageboard()
{
    isMessageBoard = false;
    messageboard->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, WINSIZE.height))));
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(POPUP_SHOW_TIME), CCCallFuncN::create(this, callfuncN_selector(SocialLayer::messageboardDelete))));
}

void SocialLayer::scoreboardDelete(CCNode* node)
{
    scoreboard->removeFromParentAndCleanup(true);
    scoreboard = NULL;
}

void SocialLayer::messageboardDelete(CCNode* node)
{
    messageboard->removeFromParentAndCleanup(true);
    messageboard = NULL;
}


CCNode* SocialLayer::createScoreLayer(int level)
{
    CCNode* layer = CCNode::create();
    CCSprite* background = CCSprite::createWithSpriteFrameName("common/popupFacebookPanel.png");
    if(IPHONE_4 || IPHONE_5)
        background->setScaleX(0.83f);
    
    if (LANDSCAPE)
    {
        layer->setContentSize(CCSize(background->getContentSize().height*background->getScaleY(), background->getContentSize().width*background->getScaleX()));
        background->setAnchorPoint(ccp(1.0f, 0.0f));
        background->setRotation(90.0f);
    }
    else
    {
        layer->setContentSize(CCSize(background->getContentSize().width*background->getScaleX(), background->getContentSize().height*background->getScaleY()));
        background->setAnchorPoint(ccp(0.0f, 0.0f));
    }
    
    if (LANDSCAPE)
    {
        scrollView = CCScrollView::create(CCSize(layer->getContentSize().width, layer->getContentSize().height*0.7f));
        scrollView->setDirection(kCCScrollViewDirectionVertical);
        scrollView->setPosition(ccp(layer->getContentSize().width/11.0f, layer->getContentSize().height/17.0f));
    }
    else
    {
        scrollView = CCScrollView::create(CCSize(layer->getContentSize().width*0.86f, layer->getContentSize().height));
        scrollView->setDirection(kCCScrollViewDirectionHorizontal);
        scrollView->setPosition(ccp(layer->getContentSize().width/11.0f, layer->getContentSize().height/7.0f));
    }
    
    scrollView->setClippingToBounds(true);
    
    layer->addChild(background, 1);
    layer->addChild(scrollView, 2);
    
    
    CCLabelTTF* highScoreLabel = CCLabelTTF::create(CCLocalizedString("HIGH_SCORE", NULL), FONT_COMMON, FONT_SIZE_64);
    layer->addChild(highScoreLabel, 3);
    
    if (LANDSCAPE)
        highScoreLabel->setPosition(ccp(layer->getContentSize().width/2.0f, layer->getContentSize().height*0.87f));
    else
        highScoreLabel->setPosition(ccp(layer->getContentSize().width/4.0f, layer->getContentSize().height*0.8f));
    
    highScoreLabel->setScale(multiplier);
    
    highScoreLabel->setColor(color);
    
    CCMenu* cmenu = CCMenu::create();
    layer->addChild(cmenu, 3);
    
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButtonSmall.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButtonSmall.png");
    spriteSelected->setColor(ccGRAY);
    
    
    CCMenuItemSprite* me = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(SocialLayer::meCallback));
    cmenu->addChild(me);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("ME", NULL), FONT_COMMON, FONT_SIZE_36);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    me->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));


    spriteNormal = CCSprite::createWithSpriteFrameName("common/redButtonSmall.png");
	spriteSelected = CCSprite::createWithSpriteFrameName("common/redButtonSmall.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* numOne = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(SocialLayer::numOneCallback));
    cmenu->addChild(numOne);
    
    labelTTF = CCLabelTTF::create("№ 1", FONT_COMMON, FONT_SIZE_36);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    numOne->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

    
    numOne->stopAllActions();
    numOne->setScale(0.7f);
	numOne->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    me->stopAllActions();
    me->setScale(0.7f);
	me->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    if (LANDSCAPE)
    {
        me->setPosition(-layer->getContentSize().width - me->getContentSize().width/2.0f, layer->getContentSize().height/3.2f);
        numOne->setPosition(-layer->getContentSize().width + numOne->getContentSize().width/2.0f, layer->getContentSize().height/3.2f);
    }
    else
    {
        me->setPosition(0.0f, -layer->getContentSize().height + me->getContentSize().height*1.5f);
        numOne->setPosition(numOne->getContentSize().width*1.4f, -layer->getContentSize().height  + numOne->getContentSize().height*1.5f);
    }
    
    if (IPHONE_4)
    {
        if (LANDSCAPE)
        {
            me->setPosition(-layer->getContentSize().width - me->getContentSize().width/2.0f, layer->getContentSize().height/3.4f);
            numOne->setPosition(-layer->getContentSize().width + numOne->getContentSize().width/2.0f, layer->getContentSize().height/3.4f);
        }
        else
        {
            me->setPosition(0.0f, -layer->getContentSize().height + me->getContentSize().height*2.0f);
            numOne->setPosition(numOne->getContentSize().width*1.4f, -layer->getContentSize().height  + numOne->getContentSize().height*2.0f);
        }
    }
    
    if (IPHONE_5)
    {
        if (LANDSCAPE)
        {
            me->setPosition(-layer->getContentSize().width - me->getContentSize().width/2.0f, layer->getContentSize().height/3.4f);
            numOne->setPosition(-layer->getContentSize().width + numOne->getContentSize().width/2.0f, layer->getContentSize().height/3.4f);
        }
        else
        {
            me->setPosition(0.0f, -layer->getContentSize().height + me->getContentSize().height*2.0f);
            numOne->setPosition(numOne->getContentSize().width*1.4f, -layer->getContentSize().height  + numOne->getContentSize().height*2.0f);
        }
    }
    
    if (!FacebookPtr->sessionIsOpened())
    {
        me->setVisible(false);
        numOne->setVisible(false);
        
        CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("SETTING_FACEBOOK_TEXT"), FONT_COMMON, FONT_SIZE_48);
        label->setColor(ccBLACK);
        if (LANDSCAPE)
        {
            label->setPosition(ccp(layer->getContentSize().width/2.0f, layer->getContentSize().height/2.5f));
            label->setFontSize(FONT_SIZE_22);
        }
        else
        {
            label->setPosition(ccp(layer->getContentSize().width/1.5f, layer->getContentSize().height/2.5f));
            label->setFontSize(FONT_SIZE_48);
        }
        layer->addChild(label, 3);
        
        CCSprite* darkFace = CCSprite::create("facebookScore.png");
        darkFace->setColor(ccGRAY);
        CCMenuItemSprite* facebook = CCMenuItemSprite::create(CCSprite::create("facebookScore.png"), darkFace, this, menu_selector(SocialLayer::facebookCallback));
        cmenu->addChild(facebook);
        
        if (LANDSCAPE)
            facebook->setPosition(-layer->getContentSize().width, -layer->getContentSize().height/4.0f);
        else
            facebook->setPosition(layer->getContentSize().width/6.0f, -layer->getContentSize().height + facebook->getContentSize().height/2.0f);
    }
    
    level--;
    users.clear();
    
    ScoreboardCell meUser;
    if (FacebookPtr->sessionIsOpened())
    {
        meUser.score = OptionsPtr->getLevelData(level).countScore;
        char buf[255];
        sprintf(buf, "%lld", FacebookPtr->fbid);
        meUser.uid = buf;
        meUser.name = FacebookPtr->name;
        users.push_back(meUser);
    }
    else
    {
        meUser.score = OptionsPtr->getLevelData(level).countScore;
        meUser.uid = "";
        char buf[255];
        sprintf(buf, "%d", meUser.score);
        meUser.name = "Me";
        users.push_back(meUser);
    }

    
    for (int i = 0; i < FacebookPtr->friendsScores.size(); i++)
    {
        ScoreboardCell user;
        user.name = FacebookPtr->friendsScores[i].name;
        user.uid = FacebookPtr->friendsScores[i].uid;
        user.score = FacebookPtr->friendsScores[i].scores[level];
        users.push_back(user);
    }
    
    std::sort(users.begin(), users.end(), ScoreboardCell::comp);
    
    countUserIsScoreboard = users.size();
    
    CCLayer* container = CCLayer::create();
    if (LANDSCAPE)
    {
        container->setContentSize(CCSize(userNodeSize, userNodeSize*countUserIsScoreboard));
        scrollView->setContentOffset(ccp(0.0f, container->getContentSize().height));
    }
    else
    {
        container->setContentSize(CCSize(userNodeSize*countUserIsScoreboard, userNodeSize));
    }
    
    for (int i = 0; i < countUserIsScoreboard; i++)
    {
        string str = FacebookPtr->getWorkDirectory() + "/" + users[i].uid + ".png";
        if (users[i].uid == meUser.uid)
        {
            userPosition = i + 1;
            str = FacebookPtr->getAvatar();
        }
        if (!FacebookPtr->sessionIsOpened())
        {
            str = "user.png";
        }
        CCLayer* node = createUserNode((i+1), str, users[i].name, users[i].score, users[i].uid);
        container->addChild(node);
        if (LANDSCAPE)
        {
            node->setPosition(ccp(layer->getContentSize().width/10.0f, container->getContentSize().height - userNodeSize*(i + 1)));
        }
        else
        {
            node->setPosition(ccp(userNodeSize*i, 0.0f));
        }
    }

    
    scrollView->setContainer(container);
    return layer;
}

CCNode* SocialLayer::createMessageboardForAsk()
{
    messageAsk = true;
    CCNode* layer = CCNode::create();
    CCSprite* background = CCSprite::create("plashka.png");
    CCSprite* greyBack = CCSprite::create("message/greyPlate.png");
    
    layer->setContentSize(CCSize(background->getContentSize().width, background->getContentSize().height));
    background->setAnchorPoint(ccp(0.0f, 0.0f));
    greyBack->setPosition(ccp(layer->getContentSize().width/2.0f-10.0f, layer->getContentSize().height/2.0f));
    
    layer->setAnchorPoint(ccp(0.5f, 0.5f));
    
    
    CCScrollView* scrollMessage = CCScrollView::create(greyBack->getContentSize());
    scrollMessage->setDirection(kCCScrollViewDirectionVertical);
    scrollMessage->setPosition(ccp(0.0f, 0.0f));
    scrollMessage->setClippingToBounds(true);
    
    layer->addChild(background, 1);
    layer->addChild(greyBack, 2);
    greyBack->addChild(scrollMessage, 3);
    
    
    CCLabelTTF* highScoreLabel = CCLabelTTF::create(CCLocalizedString("ASK_FRIEND"), FONT_COMMON, FONT_SIZE_36);
    layer->addChild(highScoreLabel, 3);
    highScoreLabel->setPosition(ccp(layer->getContentSize().width/2.0f, layer->getContentSize().height*0.9f));
    highScoreLabel->setScale(multiplier);
    highScoreLabel->setColor(color);
    
    CCMenu* cmenu = CCMenu::create();
    background->addChild(cmenu, 10);
    cmenu->setContentSize(background->getContentSize());
    cmenu->setAnchorPoint(ccp(0.0f, 0.0f));
    cmenu->setPosition(background->getPosition());
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/checkAll.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/checkAllDown.png");
    spriteSelected->setColor(ccGRAY);
    
    checkAll = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(SocialLayer::checkCallback));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("CHECK_ALL", NULL), FONT_COMMON, FONT_SIZE_32);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    checkAll->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

    
    cmenu->addChild(checkAll);
    CCSprite* check = CCSprite::create("greenArrow.png");
    check->setPosition(ccp(checkAll->getContentSize().width/6.3f, checkAll->getContentSize().height/2.0f));
    check->setTag(10);
    check->setScale(0.5f);
    checkAll->addChild(check);
    
    spriteNormal = CCSprite::createWithSpriteFrameName("common/blueButton.png");
	spriteSelected = CCSprite::createWithSpriteFrameName("common/blueButton.png");
    spriteSelected->setColor(ccGRAY);

    CCMenuItemSprite* accept = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(SocialLayer::acceptCallback));
    cmenu->addChild(accept);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("ACCEPT", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    accept->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

    
    CCMenuItemSprite* close = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"), CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(SocialLayer::closeMessageboardCallback));
    close->setPosition(ccp(background->getContentSize().width - close->getContentSize().width/2.0f, background->getContentSize().height - close->getContentSize().height/2.0f));
    cmenu->addChild(close);
    
    checkAll->setPosition(background->getContentSize().width/4.0f, checkAll->getContentSize().height*1.2f);
    accept->setPosition(background->getContentSize().width/4.0f*3.0f, accept->getContentSize().height*1.1f);
    
    CCLayer* container = CCLayer::create();
    
    int countMessage = 0;
    
    for (int k = 0; k < FacebookPtr->friendsScores.size(); k++)
        countMessage++;
    
    container->setContentSize(ccp(greyBack->getContentSize().width, 270*multiplier*countMessage));
    countMessage = 0;
    messagesGreen.clear();
    
    for (int i = 0; i < FacebookPtr->friendsScores.size(); i++)
    {
        Leaderboard request = FacebookPtr->friendsScores[i];
        CCNode* nodeMessage = createAskMessage(request.uid, request.name, i);
        nodeMessage->setPosition(0.0f, container->getContentSize().height - 270*multiplier*(countMessage+1.0f));
        container->addChild(nodeMessage);
        countMessage++;
    }
    
    scrollMessage->setContentOffset(ccp(0.0f, container->getContentSize().height));
    scrollMessage->setContainer(container);
    return layer;
}

CCNode* SocialLayer::createAskMessage(string fromId, string from, int idxRequest)
{
    CCNode* node = CCNode::create();
    CCSprite* back = CCSprite::create("message/message.png");
    back->setAnchorPoint(ccp(0.0f, 0.0f));
    node->setContentSize(back->getContentSize());
    node->addChild(back);
    
    string str = FacebookPtr->getWorkDirectory() + "/" + fromId + ".png";
    
    CCSprite* avatar = CCSprite::create(str.c_str());
    if (avatar)
    {
        avatar->setScaleX(AVATAR_WIDTH/avatar->getContentSize().width*multiplier);
        avatar->setScaleY(AVATAR_WIDTH/avatar->getContentSize().height*multiplier);
        
        CCSprite* border = CCSprite::create("avatarBorder.png");
        border->setScaleX(((AVATAR_WIDTH + 15)/border->getContentSize().width)/avatar->getScaleX()*multiplier);
        border->setScaleY(((AVATAR_WIDTH + 15)/border->getContentSize().height)/avatar->getScaleY()*multiplier);
        border->setPosition(ccp(avatar->getContentSize().width/2.0f, avatar->getContentSize().height/2.0f));
        avatar->addChild(border);
        
        avatar->setPosition(ccp(node->getContentSize().width/4.0f, node->getContentSize().height/2.0f));
        node->addChild(avatar);
    }
    
    CCMenu* nodeMenu = CCMenu::create();
    node->addChild(nodeMenu);
    nodeMenu->setContentSize(node->getContentSize());
    nodeMenu->setPosition(node->getPosition());
    nodeMenu->setAnchorPoint(ccp(0.0f, 0.0f));
    
    
    CCMenuItemSprite* messageGreen = CCMenuItemSprite::create(CCSprite::create("greenArrow.png"), CCSprite::create("greenArrow.png"), this, menu_selector(SocialLayer::messageCallback));
    messageGreen->setPosition(ccp(messageGreen->getPositionX() + messageGreen->getContentSize().width/1.9f, messageGreen->getPositionY() + node->getContentSize().height/2.0f));
    nodeMenu->addChild(messageGreen);
    messageGreen->setTag(idxRequest);
    
    messagesGreen.push_back(messageGreen);
    
    char buf[255];
    sprintf(buf, "%s!", from.c_str());
    
    CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_48);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    
    label->setPosition(ccp(node->getContentSize().width/1.8f, node->getContentSize().height/2.0f));
    node->addChild(label);
    return node;
}

CCNode* SocialLayer::createMessageboard()
{
    messageAsk = false;
    CCNode* layer = CCNode::create();
    CCSprite* background = CCSprite::create("plashka.png");
    CCSprite* greyBack = CCSprite::create("message/greyPlate.png");
    
    layer->setContentSize(CCSize(background->getContentSize().width, background->getContentSize().height));
    background->setAnchorPoint(ccp(0.0f, 0.0f));
    greyBack->setPosition(ccp(layer->getContentSize().width/2.0f-10.0f, layer->getContentSize().height/2.0f));
    
    layer->setAnchorPoint(ccp(0.5f, 0.5f));
    
    
    CCScrollView* scrollMessage = CCScrollView::create(greyBack->getContentSize());
    scrollMessage->setDirection(kCCScrollViewDirectionVertical);
    scrollMessage->setPosition(ccp(0.0f, 0.0f));
    scrollMessage->setClippingToBounds(true);
    
    layer->addChild(background, 1);
    layer->addChild(greyBack, 2);
    greyBack->addChild(scrollMessage, 3);
    
    
    CCLabelTTF* highScoreLabel = CCLabelTTF::create(CCLocalizedString("YOUR_MESSAGE", NULL), FONT_COMMON, FONT_SIZE_48);
    layer->addChild(highScoreLabel, 3);
    highScoreLabel->setPosition(ccp(layer->getContentSize().width/2.0f, layer->getContentSize().height*0.9f));
    highScoreLabel->setScale(multiplier);
    highScoreLabel->setColor(color);
    
    CCMenu* cmenu = CCMenu::create();
    background->addChild(cmenu, 10);
    cmenu->setContentSize(background->getContentSize());
    cmenu->setAnchorPoint(ccp(0.0f, 0.0f));
    cmenu->setPosition(background->getPosition());
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/checkAll.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/checkAllDown.png");
    spriteSelected->setColor(ccGRAY);
    
    checkAll = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(SocialLayer::checkCallback));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("CHECK_ALL", NULL), FONT_COMMON, FONT_SIZE_32);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    checkAll->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    
    cmenu->addChild(checkAll);
    CCSprite* check = CCSprite::create("greenArrow.png");
    check->setPosition(ccp(checkAll->getContentSize().width/6.3f, checkAll->getContentSize().height/2.0f));
    check->setTag(10);
    check->setScale(0.5f);
    checkAll->addChild(check);
    
    spriteNormal = CCSprite::createWithSpriteFrameName("common/blueButton.png");
	spriteSelected = CCSprite::createWithSpriteFrameName("common/blueButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* accept = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(SocialLayer::acceptCallback));
    cmenu->addChild(accept);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("ACCEPT", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    accept->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    CCMenuItemSprite* close = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"), CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(SocialLayer::closeMessageboardCallback));
    close->setPosition(ccp(background->getContentSize().width - close->getContentSize().width/2.0f, background->getContentSize().height - close->getContentSize().height/2.0f));
    cmenu->addChild(close);
    
    checkAll->setPosition(background->getContentSize().width/4.0f, checkAll->getContentSize().height*1.2f);
    accept->setPosition(background->getContentSize().width/4.0f*3.0f, accept->getContentSize().height*1.1f);
    
    CCLayer* container = CCLayer::create();
    
    vector<sRequestData> requests = OptionsPtr->appRequests;
    int countMessage = 0;
    for (int i = 0; i < FacebookPtr->messages.size(); i++)
        if (!FacebookPtr->messages[i].accept)
            countMessage++;
    
    container->setContentSize(ccp(greyBack->getContentSize().width, 270*multiplier*countMessage));
    countMessage = 0;
    messagesGreen.clear();
    for (int i = 0; i < FacebookPtr->messages.size(); i++)
    {
        if (!FacebookPtr->messages[i].accept)
        {
            GiftMessage request = FacebookPtr->messages[i];
            CCNode* nodeMessage = createMessage(request.from, request.name, request.notif, i);
            nodeMessage->setPosition(0.0f, container->getContentSize().height - 270*multiplier*(countMessage+1.0f));
            container->addChild(nodeMessage);
            countMessage++;
        }
    }
    
    scrollMessage->setContentOffset(ccp(0.0f, container->getContentSize().height));

    scrollMessage->setContainer(container);
    
    return layer;
}

void SocialLayer::closeMessageboardCallback(CCObject* pSender)
{
    if (GlobalsPtr->iceCreamScene == Map)
    {
        GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
        layer->showMessageBoard();
    }
    hideMessageboard();
}

CCNode* SocialLayer::createMessage(string fromId, string from, NotificationType notif, int idxRequest)
{
    CCNode* node = CCNode::create();
    CCSprite* back = CCSprite::create("message/message.png");
    back->setAnchorPoint(ccp(0.0f, 0.0f));
    node->setContentSize(back->getContentSize());
    node->addChild(back);
    
    string str = FacebookPtr->getWorkDirectory() + "/" + fromId + ".png";
    
    CCSprite* avatar = CCSprite::create(str.c_str());
    if (avatar)
    {
        avatar->setScaleX(AVATAR_WIDTH/avatar->getContentSize().width*multiplier);
        avatar->setScaleY(AVATAR_WIDTH/avatar->getContentSize().height*multiplier);
        
        CCSprite* border = CCSprite::create("avatarBorder.png");
        border->setScaleX(((AVATAR_WIDTH + 15)/border->getContentSize().width)/avatar->getScaleX()*multiplier);
        border->setScaleY(((AVATAR_WIDTH + 15)/border->getContentSize().height)/avatar->getScaleY()*multiplier);
        border->setPosition(ccp(avatar->getContentSize().width/2.0f, avatar->getContentSize().height/2.0f));
        avatar->addChild(border);
        
        avatar->setPosition(ccp(node->getContentSize().width/4.0f, node->getContentSize().height/2.0f));
        node->addChild(avatar);
    }
    
    CCMenu* nodeMenu = CCMenu::create();
    node->addChild(nodeMenu);
    nodeMenu->setContentSize(node->getContentSize());
    nodeMenu->setPosition(node->getPosition());
    nodeMenu->setAnchorPoint(ccp(0.0f, 0.0f));
    
    
    CCMenuItemSprite* messageGreen = CCMenuItemSprite::create(CCSprite::create("greenArrow.png"), CCSprite::create("greenArrow.png"), this, menu_selector(SocialLayer::messageCallback));
    messageGreen->setPosition(ccp(messageGreen->getPositionX() + messageGreen->getContentSize().width/1.9f, messageGreen->getPositionY() + node->getContentSize().height/2.0f));
    nodeMenu->addChild(messageGreen);
    messageGreen->setTag(idxRequest);
    
    messagesGreen.push_back(messageGreen);

    char buf[255];
    if (notif == HELPED_YOU)
        sprintf(buf, "%s \n %s %s", CCLocalizedString("YOUR_GOT_GIFT", NULL), from.c_str(), CCLocalizedString("GAVE_LIFE", NULL));
    else if (notif == HELP_ME)
        sprintf(buf, "%s \n %s %s", CCLocalizedString("HELP_FRIEND", NULL), from.c_str(), CCLocalizedString("REQUEST_LIFE", NULL));
    
    CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_32);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    
    label->setPosition(ccp(node->getContentSize().width/1.8f, node->getContentSize().height/2.0f));
    node->addChild(label);
    
    return node;
}

void SocialLayer::messageCallback(CCObject* pSender)
{
    CCMenuItemSprite* sender = (CCMenuItemSprite*)pSender;
    if (messageAsk)
    {
        if (!FacebookPtr->friendsScores[sender->getTag()].accept)
        {
            sender->setNormalImage(CCSprite::create("okSend.png"));
            sender->setSelectedImage(CCSprite::create("okSend.png"));
            FacebookPtr->friendsScores[sender->getTag()].accept = true;
        }
        else
        {
            sender->setNormalImage(CCSprite::create("greenArrow.png"));
            sender->setSelectedImage(CCSprite::create("greenArrow.png"));
            FacebookPtr->friendsScores[sender->getTag()].accept = false;
        }
    }
    else
    {
        if (!FacebookPtr->messages[sender->getTag()].accept)
        {
            sender->setNormalImage(CCSprite::create("okSend.png"));
            sender->setSelectedImage(CCSprite::create("okSend.png"));
            
            FacebookPtr->messages[sender->getTag()].accept = true;
        }
        else
        {
            sender->setNormalImage(CCSprite::create("greenArrow.png"));
            sender->setSelectedImage(CCSprite::create("greenArrow.png"));
            
            FacebookPtr->messages[sender->getTag()].accept = false;
        }
    }
    
    
    
    flagCheckAll = true;
    if (!messageAsk)
    {
        for (int i = 0; i < FacebookPtr->messages.size(); i++)
        {
            if (!FacebookPtr->messages[i].accept)
                flagCheckAll = false;
        }
    }
    else
    {
        for (int i = 0; i < FacebookPtr->friendsScores.size(); i++)
        {
            if (!FacebookPtr->friendsScores[i].accept)
                flagCheckAll = false;
        }
    }
    
    if (flagCheckAll)
        ((CCSprite*)checkAll->getChildByTag(10))->setDisplayFrame(CCSprite::create("okSend.png")->displayFrame());
    else
        ((CCSprite*)checkAll->getChildByTag(10))->setDisplayFrame(CCSprite::create("greenArrow.png")->displayFrame());
}

void SocialLayer::checkCallback(CCObject* pSender)
{
    if (!flagCheckAll)
    {
        ((CCSprite*)checkAll->getChildByTag(10))->setDisplayFrame(CCSprite::create("okSend.png")->displayFrame());
        
        for (int i = 0; i < messagesGreen.size(); i++)
        {
            CCMenuItemSprite* sender = messagesGreen[i];
            sender->setNormalImage(CCSprite::create("okSend.png"));
            sender->setSelectedImage(CCSprite::create("okSend.png"));
            if (messageAsk)
                FacebookPtr->friendsScores[sender->getTag()].accept = true;
            else
                FacebookPtr->messages[sender->getTag()].accept = true;
        }
        flagCheckAll = true;
    }
    else
    {
        ((CCSprite*)checkAll->getChildByTag(10))->setDisplayFrame(CCSprite::create("greenArrow.png")->displayFrame());
        
        for (int i = 0; i < messagesGreen.size(); i++)
        {
            CCMenuItemSprite* sender = messagesGreen[i];
            sender->setNormalImage(CCSprite::create("greenArrow.png"));
            sender->setSelectedImage(CCSprite::create("greenArrow.png"));
            if (!messageAsk)
                FacebookPtr->messages[sender->getTag()].accept = false;
            else
                FacebookPtr->friendsScores[i].accept = false;
        }
        flagCheckAll = false;
    }
}

void SocialLayer::acceptCallback(CCObject* pSender)
{
    if (!messageAsk)
    {
        vector<sRequestData> requests = OptionsPtr->appRequests;
        for (int i = 0; i < messagesGreen.size(); i++)
        {
            CCMenuItemSprite* sender = messagesGreen[i];
            if (FacebookPtr->messages[sender->getTag()].accept)
            {
                if (FacebookPtr->messages[sender->getTag()].notif == HELP_ME)
                {
                    FacebookPtr->sendLife(FacebookPtr->messages[sender->getTag()].from);
                }
                else if (FacebookPtr->messages[sender->getTag()].notif == HELPED_YOU)
                {
                    if (OptionsPtr->getLifeCount() < 5) {
                        OptionsPtr->setLifeCount(OptionsPtr->getLifeCount() + 1);
                    }
                }
                
            }
        }
        FacebookPtr->acceptMessage();
    }
    else
    {
        for (int i = 0; i < messagesGreen.size(); i++)
        {
            CCMenuItemSprite* sender = messagesGreen[i];
            if (FacebookPtr->friendsScores[sender->getTag()].accept)
            {
                FacebookPtr->askLife(FacebookPtr->friendsScores[sender->getTag()].uid);
            }
        }
    }
    
    OptionsPtr->save();
    closeMessageboardCallback(NULL);
}


void SocialLayer::facebookCallback(CCObject* pSender)
{
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    
    FacebookPtr->login();
}

void SocialLayer::sendLifeCallback(CCObject* pSender)
{
    ((CCNode*)pSender)->setScale(multiplier);
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    CCMenuItemSprite* sendLife = (CCMenuItemSprite*) pSender;
    CCSprite* ok = CCSprite::create("okSend.png");
    sendLife->addChild(ok);
    ok->setPosition(ccp(sendLife->getContentSize().width/2.0f, sendLife->getContentSize().height/2.0f));
    ok->setScale(multiplier);
    sendLife->setEnabled(false);
    string to = users[sendLife->getTag()].uid;
    
    FacebookPtr->sendLife(to);
}

void SocialLayer::meCallback(CCObject* pSender)
{
   // SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    scrollTo(userPosition);
}

void SocialLayer::numOneCallback(CCObject* pSender)
{
   // SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    scrollTo(1);
}

void SocialLayer::scrollTo(int place)
{
    if (LANDSCAPE)
        scrollView->setContentOffset(ccp(0.0f,  -userNodeSize*(countUserIsScoreboard - (place - 1)) + scoreboard->getContentSize().height*0.7f), true);
    else
        scrollView->setContentOffset(ccp(-userNodeSize*(place - 1), 0.0f));
}

void SocialLayer::connection()
{
    if (isConnection)
        return;
    
    connectionPlate = CCSprite::createWithSpriteFrameName("common/greenPlate.png");
    
    connectionPlate->setPosition(ccp(WINSIZE.width/2.0f + WINSIZE.width, WINSIZE.height/2.0f));
    this->addChild(connectionPlate);
    
    CCLabelTTF* titleLabel = CCLabelTTF::create("Facebook", FONT_COMMON, FONT_SIZE_86);
    titleLabel->setPosition(ccp(connectionPlate->getContentSize().width/2.0f, 9.0f*connectionPlate->getContentSize().height/10.0f));
    if (!IPAD)
        titleLabel->setScale(0.5f);
    connectionPlate->addChild(titleLabel);
    
    CCLabelTTF* textLabel = CCLabelTTF::create(CCLocalizedString("CONNECTION"), FONT_COMMON, FONT_SIZE_64);
    textLabel->setPosition(ccp(connectionPlate->getContentSize().width/2.0f, 1.0f*connectionPlate->getContentSize().height/10.0f));
    connectionPlate->addChild(textLabel);
    
    
    CCSprite* boost = CCSprite::create("loading.png");
    boost->setScale(0.6f);
    boost->setPosition(ccp(connectionPlate->getContentSize().width/2.0f, connectionPlate->getContentSize().height/2.0f));
    boost->runAction(CCRepeatForever::create(CCRotateBy::create(0.5f, 90.0f)));
    connectionPlate->addChild(boost);
    
    connectionPlate->runAction(CCMoveBy::create(0.2f, ccp(-WINSIZE.width, 0.0f)));
    ((CCLayer*)this->getParent())->setTouchEnabled(false);
    
    isConnection = true;
}


















