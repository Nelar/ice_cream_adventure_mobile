#include "LoadingScene.h"
#include "MainMenuScene.h"
#include "cGlobal.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <ctime>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace cocos2d;
using namespace std;

CCScene* LoadingScene::scene()
{
	CCScene * scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(! scene);
        
		LoadingScene *layer = LoadingScene::create();
		CC_BREAK_IF(! layer);
        
		scene->addChild(layer);
	} while (0);
    
	return scene;
}

LoadingScene::~LoadingScene()
{
    this->stopAllActions();
    this->unscheduleAllSelectors();
    this->removeAllChildrenWithCleanup(true);
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("mainMenu.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("mainMenu.pvr.ccz");
}


bool LoadingScene::init()
{
    CCDirector::sharedDirector()->setAnimationInterval(1.0f / 60.0f);
	if (!CCLayer::init())
		return false;
    
    if (IPAD)
    {
        if (LANDSCAPE)
            background = CCSprite::create("Default-Landscape@2x~ipad.png");
        else
            background = CCSprite::create("Default-Portrait@2x~ipad.png");
    }
    else if (IPAD_MINI)
    {
        if (LANDSCAPE)
            background = CCSprite::create("Default-Landscape~ipad.png");
        else
            background = CCSprite::create("Default-Portrait~ipad.png");
    }
    else if (IPHONE_5)
    {
        background = CCSprite::create("Default-568h@2x.png");
    }
    else if (IPHONE_4)
    {
        background = CCSprite::create("Default.png");
    }
    
    background->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    
    this->addChild(background);
    
    if (getNetworkStatus())
        moreGamesRequest();
    else
        moreGamesLoadSavedData();
    
	return true;
}

void LoadingScene::moreGamesRequest()
{
    ccLanguageType curLanguage = CCApplication::sharedApplication()->getCurrentLanguage();
    string langName;
    switch (curLanguage) {
        case kLanguageEnglish:
            langName = "en";
            break;
        case kLanguageRussian:
            langName = "ru";
            break;
        case kLanguageFrench:
            langName = "fr";
            break;
        case kLanguageItalian:
            langName = "it";
            break;
        case kLanguageGerman:
            langName = "de";
            break;
        case kLanguageSpanish:
            langName = "es";
            break;
        case kLanguagePortuguese:
            langName = "pt";
            break;
        default:
            langName = "en";
            break;
    }
    
    string requestStr = "https://id.ddestiny.ru/mobile/api/games/get/?lang=" + langName + string("&country=") + string(getCountry());
    
    CCHttpRequest * request = new CCHttpRequest();
    request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpGet);
    request->setUrl(requestStr.c_str());
    request->setResponseCallback(this, httpresponse_selector(LoadingScene::moreGamesServerResponse));
    cocos2d::extension::CCHttpClient::getInstance()->send(request);
    
    request->release();
}


void LoadingScene::linkCallback(CCObject * sender)
{
    goToLink(GlobalsPtr->globalMoreGames[((CCMenuItemSprite*)sender)->getTag()].url.c_str());
}

void LoadingScene::moreGamesLoadSavedData()
{
    int countMoreGames = cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey("countMoreGames", 0);
    if (countMoreGames == 0)
        return;
    
    for (int i = 0; i < countMoreGames; i++)
    {
        sMoreGames moreGame;
        moreGame.compatibility = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey(string("MoreGamesCompatibility" + to_string(i)).c_str());
        moreGame.rating = cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(string("MoreGamesRating" + to_string(i)).c_str());
        moreGame.name = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey(string("MoreGamesName" + to_string(i)).c_str());
        moreGame.url = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey(string("MoreGamesUrl" + to_string(i)).c_str());
        moreGame.price = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey(string("MoreGamesPrice" + to_string(i)).c_str());
        moreGame.description = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey(string("MoreGamesDescription" + to_string(i)).c_str());
        moreGame.stars = cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey(string("MoreGamesStars" + to_string(i)).c_str());
        moreGame.genre = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey(string("MoreGamesGenre" + to_string(i)).c_str());
        moreGame.iconUrl = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey(string("MoreGamesIconUrl" + to_string(i)).c_str());
        GlobalsPtr->globalMoreGames.push_back(moreGame);
    }
    loadingFinished(NULL);
}

void LoadingScene::moreGamesLoadedCallback(CCNode *sender)
{
    cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey("countMoreGames", GlobalsPtr->globalMoreGames.size());
    for (int i = 0; i < GlobalsPtr->globalMoreGames.size(); i++)
    {
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesCompatibility" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].compatibility);
        cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(string("MoreGamesRating" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].rating);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesName" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].name);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesUrl" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].url);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesPrice" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].price);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesDescription" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].description);
        cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(string("MoreGamesStars" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].stars);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesGenre" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].genre);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesIconUrl" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].iconUrl);
    }
    cocos2d::CCUserDefault::sharedUserDefault()->flush();
    loadingFinished(NULL);
}

void LoadingScene::iconsServerResponse(CCHttpClient * client, CCHttpResponse * response)
{
    string writeFile = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + string("icon") + response->getHttpRequest()->getTag() + string(".png");
    std::ofstream outfile(writeFile, std::ios::out | std::ios::binary);
    std::ostream_iterator<char> oi(outfile, '\0');
    
    std::vector<char> *buffer = response->getResponseData();
    std::copy(buffer->begin(), buffer->end(), oi);
    
    CCImage* image = new CCImage();
    image->initWithImageData(response->getResponseData()->data(), response->getResponseData()->size(), CCImage::kFmtPng);
    image->saveToFile(writeFile.c_str(), false);
    
    if( outfile.good() )
    {
        CCLOG( "Icon: Banner good");
        if (atoi(response->getHttpRequest()->getTag()) == (sizeMoreGames - 1))
        {
            int size = GlobalsPtr->globalMoreGames.size();
            moreGamesLoadedCallback(NULL);
        }
    }
    else
    {
        CCLOG( "Icon: Banner file path is bad");
    }
    outfile.close();
}


void LoadingScene::moreGamesServerResponse(CCHttpClient * client, CCHttpResponse * response)
{
    std::vector<char> *buffer = response->getResponseData();
    std::string str = std::string(buffer->begin(), buffer->end());
    
    rapidjson::Document d;
    d.Parse<0>(str.c_str());
    string status =  d["status"].GetString();
    if (status == "ok")
    {
        rapidjson::Value response(rapidjson::kArrayType);
        response = d["response"];
        int size = response.Size();
        sizeMoreGames = size;
        for (int i = 0; i < size; i++)
        {
            rapidjson::Value object(rapidjson::kObjectType);
            object = response[rapidjson::SizeType(i)];
            int rating = object["rating"].GetInt();
            string name = object["name"].GetString();
            string url = object["url"].GetString();
            int stars = object["stars"].GetInt();
            string genre = object["genre"].GetString();
            string compatibility = object["compatibility"].GetString();
            string icon = object["icon"].GetString();
            GlobalsPtr->globalMoreGames.push_back(sMoreGames(object["rating"].GetInt(), object["name"].GetString(), object["url"].GetString(), "0", object["stars"].GetInt(),object["genre"].GetString(),object["compatibility"].GetString(), object["icon"].GetString(), object["description"].GetString()));
            
            CCHttpRequest * request = new CCHttpRequest();
            request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpGet);
            request->setUrl(icon.c_str());
            char buf[255];
            sprintf(buf, "%d", i);
            request->setTag(buf);
            request->setResponseCallback(this, httpresponse_selector(LoadingScene::iconsServerResponse));
            cocos2d::extension::CCHttpClient::getInstance()->send(request);
            request->release();
        }
    }
}

void LoadingScene::loadingFinished(CCNode* sender)
{
    CCLog("Loading more games %d", GlobalsPtr->globalMoreGames.size());
    CCDirector::sharedDirector()->replaceScene(MainMenuScene::scene());
}

void LoadingScene::changeOrientation()
{
    
}

