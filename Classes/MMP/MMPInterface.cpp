//
//  MMPInterface.cpp
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 22.10.13.
//
//

#include "MMPInterface.h"
#include "./MMP.h"
#include <Json/JsonHelper.h>
#include <Utils/Convert.h>
//#include <Preference/SettingsMng.h>
#include <cGlobal.h>
#include <string>
#include "utils.h"
#include "AnalyticX.h"
#include <string>



using namespace std;
using namespace cocos2d;

namespace Core
{
    using namespace MarketingPlatform;
    
    MMPInterface::~MMPInterface()
    {
        delete _mmp;
    }
    
    void MMPInterface::Init( const std::string& appKey, const std::string& idfa)
    {
        _mmp = new MarketingPlatform::MMPLib( appKey, idfa );
        _mmp->Init();
    }
    
    std::string MMPInterface::CreateTimeAndKey( int eventType )
    {
        using namespace MarketingPlatform;
        std::string jsonStr = "\"t\":";
        
        char buf[255];
        struct timeval now;
        gettimeofday(&now, NULL);
        sprintf(buf, "%ld", now.tv_sec);
        
        jsonStr += buf;
        jsonStr += ",";
        
        jsonStr += "\"key\":";
        jsonStr += "\"" + MMPLib::ConvertFromTypeToStr( ( MMPLib::EventType ) eventType ) + "\",";
        
        return jsonStr;
    }
    
    std::string MMPInterface::CreateTimeAndKeyNoComma( int eventType )
    {
        using namespace MarketingPlatform;
        std::string jsonStr = "\"t\":";
        char buf[255];
        struct timeval now;
        gettimeofday(&now, NULL);
        sprintf(buf, "%ld", now.tv_sec);
        
        jsonStr += buf;
        jsonStr += ",";
        
        jsonStr += "\"key\":";
        jsonStr += "\"" + MMPLib::ConvertFromTypeToStr( ( MMPLib::EventType ) eventType ) + "\"";
        
        return jsonStr;
    }
    
    void MMPInterface::StartSession()
    {
        using namespace MarketingPlatform;
        std::string json = "{";
        json += "\"t\":";
        char buf[255];
        struct timeval now;
        gettimeofday(&now, NULL);
        sprintf(buf, "%ld", now.tv_sec);
        _sessionStartTime = now.tv_sec;
        json += Core::Convert::ToString( _sessionStartTime );
        json += ",";
        json += "\"key\":";
        json += "\"" + _mmp->ConvertFromTypeToStr( MMPLib::session_started ) + "\",";
        json += "\"props\":{";
        
        json += "\"is_1\":";
        json += GetIsFirstTimeRunning() + ",";
        
        json += "\"os\":";
#if defined(CC_TARGET_OS_IPHONE)
        json += "\"iOS\",";
#elif defined(ANDROID)
        json += "\"Android\",";
#endif
        json += "\"os_v\":";
        json += "\"" + GetSystemVersion() + "\",";
        json += "\"device\":";
        json += "\"" + GetDevice() + "\",";
        json += "\"screen_r\":";
//        const cocos2d::CCSize& resolution = SettingsMng::Instance()->GetDeviceDisplay();
        if (LANDSCAPE)
        {
            sprintf(buf, "%d", (int)WINSIZE.width);
            json += "\"" + std::string(buf) + "x";
            sprintf(buf, "%d", (int)WINSIZE.height);
            json += std::string(buf) + "\",";
        }
        else
        {
            sprintf(buf, "%d", (int)WINSIZE.height);
            json += "\"" + std::string(buf) + "x";
            sprintf(buf, "%d", (int)WINSIZE.width);
            json += std::string(buf) + "\",";
        }
        json += "\"lang\":";
        
        cocos2d::ccLanguageType language = cocos2d::CCApplication::sharedApplication()->getCurrentLanguage();
        std::string languageStr;
        switch ( language )
        {
            case cocos2d::kLanguageEnglish:
                languageStr = "en";
                break;
            case cocos2d::kLanguageChinese:
                languageStr = "ch";
                break;
            case cocos2d::kLanguageRussian:
                languageStr = "ru";
                break;
            case cocos2d::kLanguageSpanish:
                languageStr = "es";
                break;
            case cocos2d::kLanguagePortuguese:
                languageStr = "pt";
                break;
            case cocos2d::kLanguageFrench:
                languageStr = "fr";
                break;
            case cocos2d::kLanguageGerman:
                languageStr = "de";
                break;
            case cocos2d::kLanguageItalian:
                languageStr = "it";
                break;
            case cocos2d::kLanguageJapanese:
                languageStr = "ja";
                break;
            case cocos2d::kLanguageKorean:
                languageStr = "ko";
                break;
            case cocos2d::kLanguageArabic:
                languageStr = "ar";
                break;
            case cocos2d::kLanguageHungarian:
                languageStr = "hu";
                break;
            default:
                break;
        }
        
        json += "\"" + languageStr + "\",";
        
        json += "\"ip_addr\":";
        json += "\"" + GetIP() + "\",";
        
        json += "\"net\":";
        json += "\"" + GetNet() + "\",";
        
        json += "\"app_v\":";
        json += "\"" + std::string(version()) + "\",";
        
        json += "\"push_enabled\":";
        json += GetIsPushEnabled() + ",";
        
        json += "\"jailbroken\":";
        json += GetIsJailbroken();
        
        json += "}}";
        
        LogEvent( json );
    }
    
    void MMPInterface::EndSession()
    {
        using namespace MarketingPlatform;
        assert( _sessionStartTime != 0 );
        std::string json = "{";
        json += "\"t\":";
        char buf[255];
        struct timeval now;
        gettimeofday(&now, NULL);
        sprintf(buf, "%ld", now.tv_sec);
        
        json += buf;
        json += ",";
        json += "\"key\":";
        json += "\"" + _mmp->ConvertFromTypeToStr( MMPLib::session_ended ) + "\",";
        json += "\"props\":{";
        json += "\"session_started_t\":";
        json += Core::Convert::ToString( _sessionStartTime ) + ",";
        json += "\"length\":";
        
        sprintf(buf, "%ld", now.tv_sec - _sessionStartTime);
        json += buf;
        json += "}}";
        
        _sessionStartTime = 0;
        
        LogEvent( json );
    }
    
    void MMPInterface::ErrorRaised( const std::string &error, const std::string &reason )
    {
        using namespace MarketingPlatform;
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKey( MMPLib::error_raised );
        jsonStr += "\"props\":{";
        
        jsonStr += "\"error\":";
        jsonStr += "\"" + error + "\"";
        
        jsonStr += "\"reason\":";
        jsonStr += "\"" + reason + "\"";
        
        jsonStr += "}}";
        
        LogEvent( jsonStr );
    }
    
    void MMPInterface::UserProfileUpdated( const std::map<std::string, std::string>& userData )
    {
        if( userData.empty() )
        {
            return;
        }
        using namespace MarketingPlatform;
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKey( MarketingPlatform::MMPLib::user_profile_updated );
        jsonStr += "\"props\":{";
        
        for( std::map<std::string, std::string>::const_iterator it = userData.begin(), end = userData.end(); it != end; )
        {
            jsonStr += "\"" + it->first + "\",";
            jsonStr += "\"" + it->second + "\"";
            ++it;
            bool lastElement = it == end;
            if( !lastElement )
            {
                jsonStr += ",";
            }
        }
        
        jsonStr += "}}";
        LogEvent(jsonStr);
    }
    
    void MMPInterface::StoreOpened()
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKeyNoComma( MMPLib::store_opened );
        jsonStr += "}";
        AnalyticX::flurryLogEventTimed("store_opened", true);
        gaiEvent("defined_event", "store_opened");
        LogEvent(jsonStr);
    }
    
    void MMPInterface::SessionConfirmed()
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKeyNoComma( MMPLib::session_confirmed );
        jsonStr += "}";
        LogEvent(jsonStr);
    }
    
    void MMPInterface::PurchaseMade( float price, const char* profuctSku, const std::string currency, bool firstBuyOnDevice )
    {
        using namespace MarketingPlatform;
        std::string json = "{";
        json += "\"t\":";
        
        char buf[255];
        struct timeval now;
        gettimeofday(&now, NULL);
        sprintf(buf, "%ld", now.tv_sec);
        json += buf;
        
        json += ",";
        json += "\"key\":";
        json += "\"" + _mmp->ConvertFromTypeToStr( MMPLib::purchase_made ) + "\",";
        json += "\"props\":{";
        
        json += "\"sku\":";
        json += "\"" + std::string(profuctSku) + "\",";
        
        json += "\"price\":";
        json += Core::Convert::ToString( price ) + ",";
        
        json += "\"currency\":";
        json += "\"" + currency + "\",";
        
        json += "\"is_1\":";
        json += Core::Convert::ToString( firstBuyOnDevice );
        
        json += "}}";
        
        
        CCDictionary* dict = CCDictionary::create();
        dict->setObject(CCString::create(std::string(profuctSku)), "sku");
        dict->setObject(CCString::createWithFormat("%f", price), "price");
        dict->setObject(CCString::create(currency), "currency");
        dict->setObject(firstBuyOnDevice? CCString::create("true") : CCString::create("false"), "is_1");
        AnalyticX::flurryLogEventWithParametersTimed("purchase_made", dict, true);
        
        std::map<std::string, std::string> userData;
        userData.insert(pair<string, string>(std::string(profuctSku), "sku"));
        userData.insert(pair<string, string>(to_string(price), "price"));
        userData.insert(pair<string, string>(currency, "currency"));
        userData.insert(pair<string, string>(firstBuyOnDevice? "true" : "false", "is_1"));
        gaiEvent("defined_event", "purchase_made", userData);
        
        LogEvent( json );
    }
    
    void MMPInterface::VirtualPurchasMade( const std::string &sku )
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKey( MMPLib::virtual_purchase_made );
        jsonStr += "\"props\":{";
        jsonStr += "\"sku\":";
        jsonStr += "\"" + sku + "\"";
        jsonStr += "}}";
        
        CCDictionary* dict = CCDictionary::create();
        dict->setObject(CCString::create(sku), "sku");
        
        AnalyticX::flurryLogEventWithParametersTimed("virtual_purchase_made", dict, true);
        
        std::map<std::string, std::string> userData;
        userData.insert(pair<string, string>(sku, "sku"));
        gaiEvent("defined_event", "virtual_purchase_made", userData);
        
        LogEvent( jsonStr );
    }
    
    void MMPInterface::VirtualBalanceChanged( int amount, const std::string &currency )
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKey( MMPLib::virtual_balance_changed );
        jsonStr += "\"props\":{";
        jsonStr += "\"amount\":";
        jsonStr += Core::Convert::ToString( amount ) + ",";
        
        jsonStr += "\"currency\":";
        jsonStr += "\"" + currency + "\"";
        
        jsonStr += "}}";
        
        CCDictionary* dict = CCDictionary::create();
        dict->setObject(CCString::createWithFormat("%d", amount), "amount");
        dict->setObject(CCString::create(currency), "currency");
        AnalyticX::flurryLogEventWithParametersTimed("virtual_balance_changed", dict, true);
        
        std::map<std::string, std::string> userData;
        userData.insert(pair<string, string>(to_string(amount), "amount"));
        userData.insert(pair<string, string>(currency, "currency"));
        gaiEvent("defined_event", "virtual_balance_changed", userData);
        
        LogEvent(jsonStr);
    }
    
    void MMPInterface::TutorialCompleted()
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKeyNoComma( MMPLib::tutorial_completed );
        jsonStr += "}";
        
        AnalyticX::flurryLogEventTimed("tutorial_completed", true);
        gaiEvent("defined_event", "tutorial_completed");
        
        LogEvent( jsonStr );
    }
    
    void MMPInterface::TutorialSkipped()
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKeyNoComma( MMPLib::tutorial_skipped );
        jsonStr += "}";
        
        AnalyticX::flurryLogEventTimed("tutorial_skipped", true);
        gaiEvent("defined_event", "tutorial_skipped");
        
        LogEvent( jsonStr );
    }
    
    void MMPInterface::LevelStarted( const std::string &levelId )
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKey( MMPLib::level_started );
        jsonStr += "\"props\":{";
        
        jsonStr += "\"level\":";
        jsonStr += "\"" + levelId + "\"";
        
        jsonStr += "}}";
        
        CCDictionary* dict = CCDictionary::create();
        dict->setObject(CCString::create(levelId), "level");
        
        AnalyticX::flurryLogEventWithParametersTimed("level_started", dict, true);
        
        std::map<std::string, std::string> userData;
        userData.insert(pair<string, string>(levelId, "level"));
        gaiEvent("defined_event", "level_started", userData);
        
        LogEvent( jsonStr );
    }
    
    void MMPInterface::LevelCompleted( const std::string &levelId )
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKey( MMPLib::level_completed );
        jsonStr += "\"props\":{";
        
        jsonStr += "\"level\":";
        jsonStr += "\"" + levelId + "\"";
        
        jsonStr += "}}";
        
        CCDictionary* dict = CCDictionary::create();
        dict->setObject(CCString::create(levelId), "level");
        AnalyticX::flurryLogEventWithParametersTimed("level_completed", dict, true);
        
        std::map<std::string, std::string> userData;
        userData.insert(pair<string, string>(levelId, "level"));
        gaiEvent("defined_event", "level_completed", userData);
        
        LogEvent(jsonStr);
    }
    
    void MMPInterface::RankAchieved( int rank )
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKey( MMPLib::rank_achieved );
        jsonStr += "\"props\":{";
        jsonStr += "\"rank\":";
        jsonStr += Core::Convert::ToString( rank ) + ",";
        jsonStr += "}}";
        
        CCDictionary* dict = CCDictionary::create();
        dict->setObject(CCString::createWithFormat("%d", rank), "rank");
        AnalyticX::flurryLogEventWithParametersTimed("rank_achieved", dict, true);
        
        std::map<std::string, std::string> userData;
        userData.insert(pair<string, string>(to_string(rank), "rank"));
        gaiEvent("defined_event", "rank_achieved", userData);
        
        LogEvent( jsonStr );
    }
    
    void MMPInterface::GameCompleted()
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKeyNoComma( MMPLib::game_completed );
        jsonStr = "}";
        
        AnalyticX::flurryLogEventTimed("game_completed", true);
        
        gaiEvent("defined_event", "game_completed");
        
        LogEvent( jsonStr );
    }
    
    void MMPInterface::ShareContentClicked( const std::string &mediaName )
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKey( MMPLib::share_content_clicked );
        jsonStr += "\"props\":{";
        
        jsonStr += "\"social_media\":";
        jsonStr += "\"" + mediaName + "\"";
        
        jsonStr += "}}";
        
        CCDictionary* dict = CCDictionary::create();
        dict->setObject(CCString::create(mediaName), "social_media");
        AnalyticX::flurryLogEventWithParametersTimed("share_content_clicked", dict, true);
        
        std::map<std::string, std::string> userData;
        userData.insert(pair<string, string>(mediaName, "social_media"));
        gaiEvent("defined_event", "share_content_clicked", userData);
        
        LogEvent( jsonStr );
    }
    
    void MMPInterface::VisitFanPageClicked( const std::string &mediaName )
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKey( MMPLib::visit_fan_page_clicked );
        jsonStr += "\"props\":{";
        
        jsonStr+= "\"social_media\":";
        jsonStr += "\"" + mediaName + "\"";
        
        jsonStr += "}}";
        
        CCDictionary* dict = CCDictionary::create();
        dict->setObject(CCString::create(mediaName), "social_media");
        AnalyticX::flurryLogEventWithParametersTimed("visit_fan_page_clicked", dict, true);
        
        std::map<std::string, std::string> userData;
        userData.insert(pair<string, string>(mediaName, "social_media"));
        gaiEvent("defined_event", "visit_fan_page_clicked", userData);
        
        LogEvent( jsonStr );
    }
    
    void MMPInterface::InviteFriendsClicked()
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKeyNoComma( MMPLib::invite_friends_clicked );
        jsonStr += "}";
        
        AnalyticX::flurryLogEventTimed("invite_friends_clicked", true);
        gaiEvent("defined_event", "invite_friends_clicked");
        
        LogEvent( jsonStr );
    }
    
    void MMPInterface::RateAppClicked( const std::string &choice )
    {
        std::string jsonStr = "{";
        jsonStr += CreateTimeAndKey( MMPLib::rate_app_clicked );
        jsonStr += "\"props\":{";
        
        jsonStr += "\"choice\":";
        jsonStr += "\"" + choice + "\"";
        
        jsonStr += "}}";
        
        CCDictionary* dict = CCDictionary::create();
        dict->setObject(CCString::create(choice), "choice");
        AnalyticX::flurryLogEventWithParametersTimed("rate_app_clicked", dict, true);
        
        std::map<std::string, std::string> userData;
        userData.insert(pair<string, string>(choice, "choice"));
        gaiEvent("defined_event", "rate_app_clicked", userData);
        
        LogEvent( jsonStr );
    }

    bool MMPInterface::LogEvent( const std::string &jsonStr )
    {        
        Json::Reader reader;
        Json::Value root;
        
        if( !reader.parse( jsonStr, root ) )
        {
            CCLOG( "MMP: Event failed. Bad json value: %s", jsonStr.c_str() );
            return false;
        }
        
        CCLOG( "MMP: Event added to queue. Json value: %s", jsonStr.c_str() );
        
        return _mmp->LogEvent( root );
    }
    
    MarketingPlatform::Banner* MMPInterface::GetBanner()
    {
        return _mmp->GetBanner();
    }
    
}