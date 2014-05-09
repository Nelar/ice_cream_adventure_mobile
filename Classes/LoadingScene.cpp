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
#include "Options.h"
#include "cFacebook.h"
#include "IAP.h"

#include "MD5.h"
#include "base64.h"
#include "nMMP.h"

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
}


bool LoadingScene::init()
{	
	CCLOG("LoadingScene address: %p", this);
	
    CCDirector::sharedDirector()->setAnimationInterval(1.0f / 60.0f);
	if (!CCLayer::init())
		return false;
    
    GlobalsPtr->iceCreamScene = Loading;
    
    cocos2d::extension::CCHttpClient::getInstance()->setTimeoutForConnect(3);
    cocos2d::extension::CCHttpClient::getInstance()->setTimeoutForRead(10);
    
    Options* options = new Options();
	CC_UNUSED_PARAM(options);
	OptionsPtr->load();
    
    MMP* mmp = new MMP();
	CC_UNUSED_PARAM(mmp);
	
    if (IPAD)
    {
        if (LANDSCAPE)
            background = CCSprite::create("loadingiPadLandscape.png");
        else
            background = CCSprite::create("loadingiPadPortrait.png");
    }
    else if (IPAD_MINI)
    {
        if (LANDSCAPE)
            background = CCSprite::create("loadingiPadMiniLandscape.png");
        else
            background = CCSprite::create("loadingiPadMiniPortrait.png");
    }
    else if (IPHONE_4||IPHONE_5)
    {
        if (LANDSCAPE)
            background = CCSprite::create("loadingIphoneLanscape.png");
        else
            background = CCSprite::create("loadingIphonePortrait.png");
    }
	
    background->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    
    labelLoad = CCLabelTTF::create(CCLocalizedString("LOADING", NULL), FONT_COMMON, FONT_SIZE_86);
    labelLoad->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/10.0f));
    this->addChild(labelLoad, 1001);
    
    this->addChild(background);
    
    if (getNetworkStatus())
    {
        bool firstMMPInit = CCUserDefault::sharedUserDefault()->getBoolForKey("mmpInit", false);
        if (!firstMMPInit)
        {
            mmpTrackingInit();
        }
        MMPPtr->startSession();
        if (OptionsPtr->isFacebookConnection())
        {
            FacebookPtr->login();
        }
        MMPPtr->mmpBanner();
    }

	moreGamesRequest();
    
    OptionsPtr->launchCount++;
    OptionsPtr->save();
    
    if (OptionsPtr->launchCount == 4)
        appsFlyerTrackEvent("loyal_user", "");
    
	return true;
}

void LoadingScene::trackingServerResponse(CCHttpClient * client, CCHttpResponse * response)
{
    if (!response->isSucceed())
    {
        CCLOG("Server TRACKING responce ERROR: %s", response->getErrorBuffer());
        if (getNetworkStatus())
            cocos2d::extension::CCHttpClient::getInstance()->send(response->getHttpRequest());
        return;
    }
    std::vector<char> *buffer = response->getResponseData();
    std::string str = std::string(buffer->begin(), buffer->end());
    if (str == "OK") {
        CCUserDefault::sharedUserDefault()->setBoolForKey("mmpInit", true);
        CCUserDefault::sharedUserDefault()->flush();
    }
    return;
}

void LoadingScene::mmpTrackingInit()
{
    string appKey = "8";
    string appSecret = "876dd287a9a37a485b958b7acb47bba0";
    string appID = "com.destinygroup.icecreamadventure";
    string appVersion = string(version());
    string idfaKey = string(idfa());
    string idfvKey = string(idfv());
    string deviceName = string(device());
    string viOS = string(iOSversion());
    string osLat = string(limitAdTracking());
    string net = string(networkStatus());
    string currCarrier = string(carrier());
    
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
    
    string sig = appSecret + ";" + appID + ";" + appKey + ";" + appVersion + ";" + currCarrier + ";" + idfaKey + ";" + idfvKey + ";" + "idfa" + ";" + "idfv" + ";"
    + deviceName + ";" + net + ";" + langName + ";" + osLat + ";" + "iOS" + ";" + viOS + ";;" + appSecret;
    string sigMD5 = md5(sig);

    
    string requestStr = "http://ct.destiny.li/api/v1/installs/?app_id=" + appKey +
        string("&app_eid=") + appID +
        string("&app_v=") + appVersion +
        string("&d_eid_type1=idfa") +
        string("&d_eid1=") + idfaKey +
        string("&d_eid_type2=idfv") +
        string("&d_eid2=") + idfvKey +
        string("&d_model=") + deviceName +
        string("&os_name=iOS") +
        string("&os_v=") + viOS +
        string("&os_lang=") + langName +
        string("&os_lat=") + osLat +
        string("&net=") + net +
        string("&carrier=") + currCarrier +
        string("&referrer=") +
        string("&sig=") + sigMD5;
    
    CCHttpRequest * request = new CCHttpRequest();
    request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpGet);
    request->setUrl(requestStr.c_str());
    request->setResponseCallback(this, httpresponse_selector(LoadingScene::trackingServerResponse));
    cocos2d::extension::CCHttpClient::getInstance()->send(request);    
    request->release();
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
    string requestStr = "https://id.ddestiny.ru/mobile/api/games/get/?app_id=705058125&platform=ios&lang=" + langName + "&country=" + getCountry();
    
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


void LoadingScene::moreGamesLoadedCallback(CCNode *sender)
{
	CCLOG("More games loading finished. Loaded: %lu", GlobalsPtr->globalMoreGames.size());
	
	//more games loaded from server
	if(GlobalsPtr->globalMoreGames.size() > 0) {

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
		
	} else { //restore more gamse from shared
		
		int countMoreGames = cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey("countMoreGames", 0);
		
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
		
	}
	
	loadingFinished(NULL);
}

void LoadingScene::iconsServerResponse(CCHttpClient * client, CCHttpResponse * response)
{
    if (!response->isSucceed())
    {
        CCLOG("Server MORE GAMES responce ERROR: %s", response->getErrorBuffer());
        return;
    }
	
	const char * idx = response->getHttpRequest()->getTag();

	string writeFile = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + "icon" + idx + ".png";

    CCImage* image = new CCImage();
    image->initWithImageData(response->getResponseData()->data(), response->getResponseData()->size(), CCImage::kFmtPng);
    image->saveToFile(writeFile.c_str(), false);	
}


void LoadingScene::moreGamesServerResponse(CCHttpClient * client, CCHttpResponse * response)
{
    if (response->isSucceed()) {
		
		std::vector<char> *buffer = response->getResponseData();
		std::string str = std::string(buffer->begin(), buffer->end());
		
		rapidjson::Document d;
		d.Parse<0>(str.c_str());
		
		if(d.IsObject()) {
			string status =  d["status"].GetString();
			bool error = !d["error"].IsNull();
			if (status == "ok" && !error)
			{
				rapidjson::Value response(rapidjson::kArrayType);
				response = d["response"];
				int size = response.Size();
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
					string description = object["description"].GetString();

					GlobalsPtr->globalMoreGames.push_back(sMoreGames(rating, name, url, "0", stars, genre, compatibility, icon, description));
					
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
	} else {
		CCLOG("Server MORE GAMES responce ERROR: %s", response->getErrorBuffer());
	}

	moreGamesLoadedCallback(NULL);
}

void LoadingScene::loadingFinished(CCNode* sender)
{
    CCLog("Loading more games %lu", GlobalsPtr->globalMoreGames.size());
    
    bool flagLoad = false;
    flagLoad = CCUserDefault::sharedUserDefault()->getBoolForKey("gameCenterLogin", false);
    
    if (flagLoad)
    {
        if (getNetworkStatus())
            loginGC();
        else
            CCDirector::sharedDirector()->replaceScene(MainMenuScene::scene());
    }
    else
    {
        CCDirector::sharedDirector()->replaceScene(MainMenuScene::scene());
    }
}

void LoadingScene::changeOrientation()
{
    background->removeFromParentAndCleanup(true);
    labelLoad->removeFromParentAndCleanup(true);
    if (IPAD)
    {
        if (LANDSCAPE)
            background = CCSprite::create("loadingiPadLandscape.png");
        else
            background = CCSprite::create("loadingiPadPortrait.png");
    }
    else if (IPAD_MINI)
    {
        if (LANDSCAPE)
            background = CCSprite::create("loadingiPadMiniLandscape.png");
        else
            background = CCSprite::create("loadingiPadMiniPortrait.png");
    }
    else if (IPHONE_4||IPHONE_5)
    {
        if (LANDSCAPE)
            background = CCSprite::create("loadingIphoneLanscape.png");
        else
            background = CCSprite::create("loadingIphonePortrait.png");
    }
    background->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    
    labelLoad = CCLabelTTF::create(CCLocalizedString("LOADING", NULL), FONT_COMMON, FONT_SIZE_86);
    labelLoad->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/10.0f));
    this->addChild(labelLoad, 1001);
    
    this->addChild(background);
}

