//
//  MMP.cpp
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 21.10.13.
//
//

#include "MMP.h"
#include "./MMPRequest.h"
#include "./MMPBannerRequest.h"
#include "./MMPGetFileRequest.h"
#include <Json/JsonHelper.h>
#include <Utils/Convert.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ctime>
#include <string>

using namespace std;

namespace MarketingPlatform
{    
    static MarketingPlatform::MMPLib::EventType ConvertFromStringToType( const std::string& typeStr )
    {
        using namespace MarketingPlatform;
        if( typeStr == "session_started" )
        {
            return MMPLib::session_started;
        }
        else if( typeStr == "session_ended" )
        {
            return MMPLib::session_ended;
        }
        else if( typeStr == "error_raised" )
        {
            return MMPLib::error_raised;
        }
        else if( typeStr == "user_profile_updated" )
        {
            return MMPLib::user_profile_updated;
        }
        else if( typeStr == "store_opened" )
        {
            return MMPLib::store_opened;
        }
        else if( typeStr == "purchase_made" )
        {
            return MMPLib::purchase_made;
        }
        else if( "virtual_purchase_made" )
        {
            return MMPLib::virtual_purchase_made;
        }
        else if( "virtual_balance_changed" )
        {
            return MMPLib::virtual_balance_changed;
        }
        else if( "tutorial_completed" )
        {
            return MMPLib::tutorial_completed;
        }
        else if( "tutorial_skipped" )
        {
            return MMPLib::tutorial_skipped;
        }
        else if( "level_completed" )
        {
            return MMPLib::level_completed;
        }
        else if( "level_started" )
        {
            return MMPLib::level_started;
        }
        else if( "rank_achieved" )
        {
            return MMPLib::rank_achieved;
        }
        else if( "game_completed" )
        {
            return MMPLib::game_completed;
        }
        else if( "share_content_clicked" )
        {
            return MMPLib::share_content_clicked;
        }
        else if( "visit_fan_page_clicked" )
        {
            return MMPLib::visit_fan_page_clicked;
        }
        else if( "invite_friends_clicked" )
        {
            return MMPLib::invite_friends_clicked;
        }
        else if( "rate_app_clicked" )
        {
            return MMPLib::rate_app_clicked;
        }  
        return MMPLib::undefined;
    }
    
    std::string MMPLib::ConvertFromTypeToStr( MarketingPlatform::MMPLib::EventType type )
    {
        using namespace MarketingPlatform;
        switch ( type )
        {
            case MMPLib::session_started:
                return "session_started";
                
            case MMPLib::session_ended:
                return "session_ended";
                
            case MMPLib::error_raised:
                return "error_raised";
                
            case MMPLib::user_profile_updated:
                return "user_profile_updated";
                
            case MMPLib::store_opened:
                return "store_opened";
                
            case MMPLib::purchase_made:
                return "purchase_made";
                
            case MMPLib::virtual_purchase_made:
                return "virtual_purchase_made";
                
            case MMPLib::virtual_balance_changed:
                return "virtual_balance_changed";
                
            case MMPLib::tutorial_completed:
                return "tutorial_completed";
                
            case MMPLib::tutorial_skipped:
                return "tutorial_skipped";
                
            case MMPLib::level_completed:
                return "level_completed";
                
            case MMPLib::level_started:
                return "level_started";
                
            case MMPLib::rank_achieved:
                return "rank_achieved";
                
            case MMPLib::game_completed:
                return "game_completed";
                
            case MMPLib::share_content_clicked:
                return "share_content_clicked";
                
            case MMPLib::visit_fan_page_clicked:
                return "visit_fan_page_clicked";
                
            case MMPLib::invite_friends_clicked:
                return "invite_friends_clicked";
                
            case MMPLib::rate_app_clicked:
                return "rate_app_clicked";
                
            case MMPLib::session_confirmed:
                return "session_confirmed";
                
            default:
                return "";
                break;
        }
    }
    
    static const std::string savedDataKey( "savedData" );
    static const std::string bannerExistingKey( "bannerExist" );
    static const std::string bannerFormatKey( "bannerFormat" );
    static const std::string bannerLandingUrlKey( "bannerLandingUrl" );
    
    MMPLib::MMPLib( const std::string& appKey, const std::string& idfa ):
    _appKey( appKey )
    ,_idfa( idfa )
    ,_host( "destiny.li/api/events" )
    ,_header( "Content-Type: application/json" )
    ,_bannerHost( "destiny.li/api/xpromo" )
    ,_banner( NULL )
    ,_bannerFilePath( cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + "mmp_local_banner_image" )
    {
    }
    
    MMPLib::~MMPLib()
    {
        SaveEvents();
        delete _banner;
        _banner = NULL;
    }
    
    void MMPLib::Init( const std::string &host, const std::string& bannerHost, const std::string &header )
    {
        _host = host;
        _header = header;
        _bannerHost = bannerHost;
        Core::EventObserver::Register( "MMPRequest", EVENT_HANDLER( MMPLib::OnNetworkRequest ) );
        Core::EventObserver::Register( "MMPBannerRequest", EVENT_HANDLER( MMPLib::OnBannerRequest ) );
        Core::EventObserver::Register( "MMPGetFileRequest", EVENT_HANDLER( MMPLib::OnGetFileRequest ) );
        
        LoadBanner();
    }
    
    void MMPLib::SendRequest( const std::string &batch )
    {
        MMPRequest* request = MMPRequest::Create();
        request->SetHost( _host );
        std::vector<std::string> headers;
        headers.push_back( _header );
        request->SetHeaders( headers );
        request->SetAppKey( _appKey );
        request->SetIdfa( _idfa );
        request->SetBatch( batch );
        request->Signal();
    }
    
    void MMPLib::PopRequest( MMPRequest *request )
    {
        bool haveFound = false;
        while ( !haveFound )
        {
            MMPRequest* front = _waitingRequests.front();
            _waitingRequests.pop();
            if( front == request )
            {
                haveFound = true;
                front->Release();
            }
            else
            {
                _waitingRequests.push( front );
            }
        }
    }
    
    void MMPLib::SendEvents()
    {
        const std::string& savedDataBatch = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey( savedDataKey.c_str(), "" );
        if( !savedDataBatch.empty() )
        {
            SendRequest( savedDataBatch );
            cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey( savedDataKey.c_str(), "" );
            cocos2d::CCUserDefault::sharedUserDefault()->flush();
        }
    }
    
    void MMPLib::SaveEvents()
    {
        if( !_logQueue.empty() || !_waitingRequests.empty() )
        {
            Json::FastWriter writer;
            std::string rootStr = "{ \"batch\":[";
            
            while ( !_waitingRequests.empty() )
            {
                Json::Reader reader;
                Json::Value waitingRoot;
                MMPRequest* frontWaitingRequest = _waitingRequests.front();
                const std::string& waitingRootStr = frontWaitingRequest->GetBatch();
                reader.parse( waitingRootStr, waitingRoot );
                const Json::Value& batch = Core::JsonHelper::GetValue( waitingRoot, "batch" );
                for ( Json::Value::iterator it = batch.begin(), end = batch.end(); it != end; )
                {
                    rootStr += (*it).toStyledString();
                    ++it;
                    bool isLastElement = it == end && _logQueue.empty();
                    if( !isLastElement )
                    {
                        rootStr += ",";
                    }
                }
                frontWaitingRequest->Release();
                _waitingRequests.pop();
            }
            
            while ( !_logQueue.empty() )
            {
                rootStr += writer.write( _logQueue.front().second );
                _logQueue.pop();
                bool isLastElement = _logQueue.size() == 0;
                if( !isLastElement )
                {
                    rootStr += ",";
                }
            }
            rootStr += "]}";
            CCLOG( "saved string %s", rootStr.c_str() );
            cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey( savedDataKey.c_str(), rootStr );
            cocos2d::CCUserDefault::sharedUserDefault()->flush();
        }
    }
    
    void MMPLib::LoadBanner()
    {
        bool bannerLocallyExist = cocos2d::CCUserDefault::sharedUserDefault()->getBoolForKey( bannerExistingKey.c_str(), false );
        if( bannerLocallyExist )
        {
            Banner::BannerType bannerType = (Banner::BannerType) cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey( bannerFormatKey.c_str() );
            std::string bannerLandingUrl = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey( bannerLandingUrlKey.c_str() );
            _banner = new Banner( bannerType, bannerLandingUrl, _bannerFilePath.c_str() );
        }
        else
        {
            MMPBannerRequest* bannerRequest = MMPBannerRequest::Create();
            bannerRequest->SetHost( _bannerHost );
            bannerRequest->SetAppKey( _appKey );
            bannerRequest->SetIdfa( _idfa );
            bannerRequest->Signal();
        }
    }
    
    bool MMPLib::OnNetworkRequest( Core::Event *event )
    {
        using namespace AngryPets::GlobalWorld;
        MMPRequest* request = static_cast<MMPRequest*>( event );
        switch ( request->GetType() )
        {
            case NetworkRequest::completeRequest:
            {
                
            }
                break;
                
            case NetworkRequest::failedRequest:
                if( request->GetBadRequest() )
                {
                    CCLOG( "MMP: Bad request : %s", request->GetBatch().c_str() );
                }
                else
                {
                    CCLOG( "MMP: Respose code 500. Bad server" );
                    request->Retain();
                    _waitingRequests.push( request );
                }
                break;
                
            default:
                break;
        }
        
        return false;
    }
    
    bool MMPLib::OnBannerRequest( Core::Event *event )
    {
        using namespace AngryPets::GlobalWorld;
        MMPBannerRequest* bannerRequest = static_cast<MMPBannerRequest*>( event );
        switch ( bannerRequest->GetType() )
        {
            case AngryPets::GlobalWorld::NetworkRequest::completeRequest:
            {
                const std::string& imageUrl = bannerRequest->GetImageUrl();
                if( !imageUrl.empty() )
                {
                    cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey( bannerFormatKey.c_str(), (int) bannerRequest->GetBannerType() );
                    cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey( bannerLandingUrlKey.c_str(), bannerRequest->GetLandingUrl() );                    
                    MMPGetFileRequest* getFileRequest = MMPGetFileRequest::Create();
                    getFileRequest->SetFileUrl( imageUrl );;
                    getFileRequest->Signal();
                }
            }
                break;
                
            default:
                break;
        }
        
        return false;
    }
    
    bool MMPLib::OnGetFileRequest( Core::Event *event )
    {
        using namespace AngryPets::GlobalWorld;
        MMPGetFileRequest* getFileRequest = static_cast<MMPGetFileRequest*>(event);
        switch ( getFileRequest->GetType())
        {
            case AngryPets::GlobalWorld::NetworkRequest::completeRequest:
            {
                std::ofstream outfile( _bannerFilePath.c_str(), std::ios::out | std::ios::binary );
                std::ostream_iterator<char> oi(outfile, '\0');
                copy( getFileRequest->GetResponseData().begin(), getFileRequest->GetResponseData().end(), oi );
                if( outfile.good() )
                {
                    Banner::BannerType bannerType = (Banner::BannerType) cocos2d::CCUserDefault::sharedUserDefault()->getIntegerForKey( bannerFormatKey.c_str() );
                    std::string bannerLandingUrl = cocos2d::CCUserDefault::sharedUserDefault()->getStringForKey( bannerLandingUrlKey.c_str() );
                    _banner = new Banner( bannerType, bannerLandingUrl, _bannerFilePath.c_str() );
                    cocos2d::CCUserDefault::sharedUserDefault()->setBoolForKey( bannerExistingKey.c_str(), true );
                    cocos2d::CCUserDefault::sharedUserDefault()->flush();
                }
                else
                {
                    CCLOG( "MMP: Banner file path is bad" );
                }
                outfile.close();
            }
                break;
                
            case AngryPets::GlobalWorld::NetworkRequest::failedRequest:
            {
                using namespace MarketingPlatform;
                std::string jsonStr = "{";
                jsonStr +=  "\"t\":";
                jsonStr += Core::Convert::ToString( std::time( 0 ) );
                jsonStr += ",";
                
                jsonStr += "\"key\":";
                jsonStr += "\"" + ConvertFromTypeToStr( error_raised ) + "\",";
                
                jsonStr += "\"props\":{";
                
                jsonStr += "\"error\":";
                jsonStr += "\"banner_not_loaded\",";
                
                jsonStr += "\"reason\":";
                jsonStr += "\"can't load banner image\"";
                
                jsonStr += "}}";
                
                Json::Reader reader;
                Json::Value root;
                reader.parse( jsonStr, root );
                
                LogEvent( root );
            }
                break;
                
            default:
                break;
        }
        
        return false;
    }
    
    bool MMPLib::LogEvent( const Json::Value &root )
    {
        const Json::Value& value_key = Core::JsonHelper::GetValue( root, "key" );
        const std::string& key = Core::JsonHelper::ToString( value_key );
        EventType type = ConvertFromStringToType( key );
        bool oldSessionIsNotEnded = !_logQueue.empty() && _logQueue.back().first != session_ended;
        if( type == session_started && oldSessionIsNotEnded )
        {
            Json::FastWriter writer;
            CCLOG( "MMP: Old session is not ended. Can't start event: %s", writer.write( root ).c_str() );
            return false;
        }
        if( type == session_started )
        {
            SendEvents();
        }
        _logQueue.push( Param() );
        _logQueue.back().first = type;
        _logQueue.back().second = root;
        if( type == session_ended )
        {
            SaveEvents();
        }
        
        return true;
    }
    
    Banner* MMPLib::GetBanner()
    {
        bool bannerLocallyExist = cocos2d::CCUserDefault::sharedUserDefault()->getBoolForKey( bannerExistingKey.c_str(), false );
        if( bannerLocallyExist )
        {
            //we "delete" banner from local data
            cocos2d::CCUserDefault::sharedUserDefault()->setBoolForKey( bannerExistingKey.c_str(), false );
            cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey( bannerFormatKey.c_str(), 0 );
            cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey( bannerLandingUrlKey.c_str(), "" );
            cocos2d::CCUserDefault::sharedUserDefault()->flush();
            std::ofstream outfile( _bannerFilePath.c_str(), std::ios::out | std::ios::binary );
            outfile.clear();
            outfile.close();
            return _banner;
        }
        else
        {
            return NULL;
        }
    }
}