//
//  nMMP.cpp
//  IceCreamAdventure
//
//  Created by Nelar on 14.03.14.
//
//

#include "nMMP.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "Options.h"

#include "MD5.h"
#include "base64.h"
#include "utils.h"
#include "cGlobal.h"

#include <fstream>
#include <iostream>
#include <iterator>

using namespace std;

using namespace std;


void MMP::mmpAnalytics(string json)
{
    /*
    ICA PROD: app_id=10, app_secret=cbe2fc96b756a8adc510c05322be0eb3
    ICA DEV: app_id=4, app_secret=6a3b93ddba2b32140f4be6569928d65d
     */
    string appKey = "4";
    string appSecret = "6a3b93ddba2b32140f4be6569928d65d";
    
    string appID = "com.destinygroup.icecreamadventure";
    string appVersion = string(version());
    string idfaKey = string(idfa());
    string idfvKey = string(idfv());
    string deviceName = string(device());
    string viOS = string(iOSversion());
    string osLat = string(limitAdTracking());
    string net = string(networkStatus());
    string currCarrier = string(carrier());
    string header = "Content-Type: application/json";
    vector<string> headers;
    headers.push_back(header);
    
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
    
    string sig = appSecret + ";" + appKey + ";" + idfaKey + ";" + "idfa" + ";" + idfvKey + ";" + appSecret;
    string sigMD5 = md5(sig);
    
    
    string requestStr = "bb.destiny.li/api/v1/events/?app_id=" + appKey +
    string("&d_eid=") + idfaKey +
    string("&d_eid_type=idfa") +
    string("&idfv=") + idfvKey +
    string("&sig=") + sigMD5;
    
    CCHttpRequest * request = new CCHttpRequest();
    request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpPost);
    request->setUrl(requestStr.c_str());
    request->setResponseCallback(this, httpresponse_selector(MMP::analyticsServerResponse));
    request->setHeaders(headers);
    request->setRequestData(json.c_str(), strlen(json.c_str()));
    cocos2d::extension::CCHttpClient::getInstance()->send(request);
    request->release();
}

void MMP::mmpBanner()
{
    /*
     - QP (iOS): app_id=5
     */
    string appKey = "5";
    string appID = "com.destinygroup.4picsnewepisode";
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
    char buf[255];
    string resolution;
    if (LANDSCAPE)
    {
        sprintf(buf, "%d", (int)WINSIZE.width);
        resolution += "\"" + string(buf) + "x";
        sprintf(buf, "%d", (int)WINSIZE.height);
        resolution += std::string(buf) + "\"";
    }
    else
    {
        sprintf(buf, "%d", (int)WINSIZE.height);
        resolution += "\"" + std::string(buf) + "x";
        sprintf(buf, "%d", (int)WINSIZE.width);
        resolution += std::string(buf) + "\"";
    }

    
    
    string requestStr = "http://ads.destiny.li/api/v1/banners/?app_id=" + appKey +
    string("&d_eid_type1=idfa") +
    string("&d_eid1=") + idfaKey +
    string("&d_eid_type2=idfv") +
    string("&d_eid2=") + idfvKey +
    string("&d_model=") + deviceName +
    string("&d_screen=") + resolution +
    string("&os_name=iOS") +
    string("&os_v=") + viOS +
    string("&os_lang=") + langName +
    string("&os_lat=") + osLat +
    string("&net=") + net;
    
    CCHttpRequest * request = new CCHttpRequest();
    request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpGet);
    request->setUrl(requestStr.c_str());
    request->setResponseCallback(this, httpresponse_selector(MMP::bannerServerResponse));
    cocos2d::extension::CCHttpClient::getInstance()->send(request);
    request->release();
}

void MMP::bannerServerResponse(CCHttpClient * client, CCHttpResponse * response)
{
    std::vector<char> *buffer = response->getResponseData();
    std::string str = std::string(buffer->begin(), buffer->end());
    rapidjson::Document d;
    d.Parse<0>(str.c_str());
	
	if(!d.IsObject())
		return;
	
    if (d["status"].IsNull())
        return;
	
    string code = d["status"].GetString();
    CCLOG("BANNER RESPONCE %s", code.c_str());
    if(code == "ok")
    {
        imageUrl = d["image_url"].GetString();
        trackingUrl = d["tracking_url"].GetString();
        storeUrl = d["store_url"].GetString();
        
        CCHttpRequest * request = new CCHttpRequest();
        request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpGet);
        request->setUrl(imageUrl.c_str());
        request->setResponseCallback(this, httpresponse_selector(MMP::imageServerResponse));
        cocos2d::extension::CCHttpClient::getInstance()->send(request);
        request->release();
    }
    
    return;
}

void MMP::imageServerResponse(CCHttpClient * client, CCHttpResponse * response)
{
    string writeFile = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + string("banner.png");
    std::ofstream outfile(writeFile, std::ios::out | std::ios::binary);
    std::ostream_iterator<char> oi(outfile, "\0");
    
    std::vector<char> *buffer = response->getResponseData();
    std::copy(buffer->begin(), buffer->end(), oi);
    
    CCImage* image = new CCImage();
    image->initWithImageData(response->getResponseData()->data(), response->getResponseData()->size(), CCImage::kFmtPng);
    image->saveToFile(writeFile.c_str(), false);
//    setNotBackUp(writeFile.c_str());
    
    if( outfile.good() )
    {
//        setNotBackUp(writeFile.c_str());
        CCLOG( "BANNER MMP GOOD");
        isBanner = true;
    }
    else
    {
        CCLOG( "BANNER MMP BAD");
    }
    outfile.close();
    delete image;
}


string MMP::meta()
{
    string appKey = "4";
    string appSecret = "6a3b93ddba2b32140f4be6569928d65d";
    string appID = "com.destinygroup.icecreamadventure";
    string appVersion = string(version());
    string idfaKey = string(idfa());
    string idfvKey = string(idfv());
    string deviceName = string(device());
    string viOS = string(iOSversion());
    string osLat = string(limitAdTracking());
    string net = string(networkStatus());
    string currCarrier = string(carrier());
    string resolution;
    
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

    
    char buf[255];
    if (LANDSCAPE)
    {
        sprintf(buf, "%d", (int)WINSIZE.width);
        resolution += "\"" + string(buf) + "x";
        sprintf(buf, "%d", (int)WINSIZE.height);
        resolution += std::string(buf) + "\"";
    }
    else
    {
        sprintf(buf, "%d", (int)WINSIZE.height);
        resolution += "\"" + std::string(buf) + "x";
        sprintf(buf, "%d", (int)WINSIZE.width);
        resolution += std::string(buf) + "\"";
    }
    
    struct timeval now;
    gettimeofday(&now, NULL);
    sprintf(buf, "%ld", now.tv_sec);
    time_t currTime = now.tv_sec;
    
    string currTimeStr = string(buf);
    
    time_t currTime2 = currTime - startTime;
    sprintf(buf, "%ld", currTime2);
    string currTimeStr2 = string(buf);
    
    
    string metaStr = "\"meta\": {" +
        string("\"app_v\":\"") + appVersion + string("\",") +
        string("\"app_push\":") + push_enabled() + string(",") +
        string("\"d_model\":\"") + deviceName + string("\",") +
        string("\"d_screen\":") + resolution + string(",") +
        string("\"os_name\":\"iOS\",") +
        string("\"os_v\":\"") + viOS + string("\",") +
        string("\"os_lang\":\"") + langName + string("\",") +
        string("\"os_j9\":false,") +
        string("\"net\":\"") + net + string("\",") +
        string("\"carrier\":\"") + currCarrier + string("\",") +
        string("\"t\":\"") + currTimeStr + string("\",") +
        string("\"t2\":\"") + currTimeStr2 + string("\"}");
    
    return metaStr;
}

void MMP::startSession()
{
    if (!getNetworkStatus())
        return;
    
    struct timeval now;
    gettimeofday(&now, NULL);
    startTime = now.tv_sec;
    
    string str = "{\"key\":\"session_started\","+ meta() + string(",")+
        string("\"props\":{") +
        string("\"is_1\":") + getIsFirstTimeRunning() +
        string("}}");
    
    appsFlyerTrackEvent("session_started", "session_started");
    
    mmpAnalytics(str);
}

void MMP::endedSession()
{
    if (!getNetworkStatus())
        return;
    
    string str = "{\"key\":\"session_ended\","+ meta() + string("}");
    
    mmpAnalytics(str);
}

void MMP::sessionLoaded()
{
    if (!getNetworkStatus())
        return;
    
    string str = "{\"key\":\"session_loaded\","+ meta() + string("}");
    
    mmpAnalytics(str);
}

void MMP::userIdentified(string fbId)
{
    if (!getNetworkStatus())
        return;
    
    string str = "{\"key\":\"user_identified\","+ meta() + string(",")+
    string("\"props\":{") +
    string("\"type\":\"facebook\",") +
    string("\"id\":\"") + fbId + string("\"") +
    string("}}");
    
    appsFlyerTrackEvent("user_identified", "facebook");
    
    mmpAnalytics(str);
}

void MMP::purchaseStarted()
{
    if (!getNetworkStatus())
        return;
    
    string str = "{\"key\":\"purchase_started\","+ meta() + string("}");
    
    appsFlyerTrackEvent("purchase_started", "");
    
    mmpAnalytics(str);
}

void MMP::purchaseCanceled()
{
    if (!getNetworkStatus())
        return;
    
    string str = "{\"key\":\"purchase_canceled\","+ meta() + string("}");
    appsFlyerTrackEvent("purchase_canceled", "");
    
    mmpAnalytics(str);
}

void MMP::purchaseConfirmed()
{
    if (!getNetworkStatus())
        return;
    
    string str = "{\"key\":\"purchase_confirmed\","+ meta() + string("}");
    appsFlyerTrackEvent("purchase_confirmed", "");
    
    mmpAnalytics(str);
}

void MMP::purchaseMade(string sku, string currency, float price, bool is_valid)
{
    if (!getNetworkStatus())
        return;
    
    char buf[255];
    sprintf(buf, "%f", price);
    string validStr;
    if (is_valid)
        validStr = "true";
    else
        validStr = "false";
    
    string str = "{\"key\":\"purchase_made\","+ meta() + string(",")+
    string("\"props\":{") +
    string("\"sku\":\"") + sku + string("\",") +
    string("\"currency\":\"") + currency + string("\",") +
    string("\"price\":") + string(buf) + string(",") +
    string("\"is_valid\":") + validStr +
    string("}}");
    
    appsFlyerTrackEvent("purchase_made", sku.c_str());
    
    mmpAnalytics(str);
}

void MMP::tutorialStepStarted(int num, string name)
{
    if (!getNetworkStatus())
        return;
    
    char buf[255];
    sprintf(buf, "%d", num);
    
    string str = "{\"key\":\"tutorial_step_started\","+ meta() + string(",")+
    string("\"props\":{") +
    string("\"num\":") + string(buf) + string(",") +
    string("\"name\":\"") + name + string("\"") +
    string("}}");
    
    appsFlyerTrackEvent("tutorial_step_started", "");
    
    mmpAnalytics(str);
}

void MMP::tutorialStepCompleted(int num, string name)
{
    if (!getNetworkStatus())
        return;
    
    char buf[255];
    sprintf(buf, "%d", num);
    
    string str = "{\"key\":\"tutorial_step_completed\","+ meta() + string(",")+
    string("\"props\":{") +
    string("\"num\":") + string(buf) + string(",") +
    string("\"name\":\"") + name + string("\"") +
    string("}}");
    
    appsFlyerTrackEvent("tutorial_step_completed", "");
    
    mmpAnalytics(str);
}

void MMP::levelStarted(int num, string name)
{
    if (!getNetworkStatus())
        return;
    
    char buf[255];
    sprintf(buf, "%d",  num);
    
    string str = "{\"key\":\"level_started\","+ meta() + string(",")+
    string("\"props\":{") +
    string("\"num\":") + string(buf) + string(",") +
    string("\"name\":\"") + name + string("\"") +
    string("}}");
    
    appsFlyerTrackEvent("level_started", buf);
    
    mmpAnalytics(str);
}

void MMP::levelCompleted(int num, string name)
{
    if (!getNetworkStatus())
        return;
    
    char buf[255];
    sprintf(buf, "%d",  num);
    
    string str = "{\"key\":\"level_completed\","+ meta() + string(",")+
    string("\"props\":{") +
    string("\"num\":") + string(buf) + string(",") +
    string("\"name\":\"") + name + string("\"") +
    string("}}");
    
    appsFlyerTrackEvent("level_completed", buf);
    
    mmpAnalytics(str);
}

void MMP::facebookConnected(string fbId, string name)
{
    if (!getNetworkStatus())
        return;
    
    string str = "{\"key\":\"sn_connected\","+ meta() + string(",")+
    string("\"props\":{") +
    string("\"type\":\"facebook\",") +
    string("\"id\":\"") + fbId + string("\",") +
    string("\"name\":\"") + name + string("\"") +
    string("}}");
    
    appsFlyerTrackEvent("sn_connected", "facebook");
    
    mmpAnalytics(str);
}

void MMP::contentShared()
{
    if (!getNetworkStatus())
        return;
    
    string str = "{\"key\":\"content_shared\","+ meta() + string(",")+
    string("\"props\":{") +
    string("\"type\":\"facebook\"") +
    string("}}");
    
    appsFlyerTrackEvent("content_shared", "facebook");
    
    mmpAnalytics(str);
}

void MMP::friendsInvited()
{
    if (!getNetworkStatus())
        return;
    
    string str = "{\"key\":\"friends_invited\","+ meta() + string(",")+
    string("\"props\":{") +
    string("\"type\":\"facebook\"") +
    string("}}");
    
    appsFlyerTrackEvent("friends_invited", "facebook");
    
    mmpAnalytics(str);
}

void MMP::mmpTracking()
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
    request->setResponseCallback(this, httpresponse_selector(MMP::trackingServerResponse));
    cocos2d::extension::CCHttpClient::getInstance()->send(request);
    request->release();
}

void MMP::analyticsServerResponse(CCHttpClient * client, CCHttpResponse * response)
{
	CCLOG("MMP: %p, Server ANALYTICS responce: %s", this, response->getErrorBuffer());
    if (!response->isSucceed())
    {
        CCLOG("Server ANALYTICS responce ERROR: %s", response->getErrorBuffer());
        if (getNetworkStatus())
            cocos2d::extension::CCHttpClient::getInstance()->send(response->getHttpRequest());
        return;
    }
    return;
}

void MMP::trackingServerResponse(CCHttpClient * client, CCHttpResponse * response)
{
    if (!response->isSucceed())
    {
        CCLOG("Server responce ERROR: %s", response->getErrorBuffer());
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