//
//  MMP.h
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 21.10.13.
//
//

#pragma once
#include <string>
#include <ctime>
#include <queue>
#include <jsoncpp/include/json/json.h>
#include <Event/EventObserver.h>
#include "./Banner.h"

namespace MarketingPlatform
{
    class MMPRequest;
    class MMPLib : public Core::EventObserver
    {
        
    public:
        
        enum EventType
        {
            undefined,
            session_started,
            session_ended,
            error_raised,
            user_profile_updated,
            store_opened,
            purchase_made,
            virtual_purchase_made,
            virtual_balance_changed,
            tutorial_completed,
            tutorial_skipped,
            level_started,
            level_completed,
            rank_achieved,
            game_completed,
            share_content_clicked,
            visit_fan_page_clicked,
            invite_friends_clicked,
            rate_app_clicked,
            session_confirmed
        };
        
        typedef std::pair<EventType, Json::Value> Param;
        typedef std::vector<Param> ParamContainer;
        
    private:
        
        std::string _appKey;
        std::string _idfa;
        std::string _host;
        std::string _bannerHost;
        std::string _header;
        std::string _bannerFilePath;
        std::queue<Param> _logQueue;
        std::queue<MMPRequest*> _waitingRequests;
        Banner* _banner;
        void SendRequest( const std::string& batch );
        void PopRequest( MMPRequest* request );
        void SendEvents();
        void SaveEvents();
        void LoadBanner();
        bool OnNetworkRequest( Core::Event* event );
        bool OnBannerRequest( Core::Event* event );
        bool OnGetFileRequest( Core::Event* event );
        
    public:
        
        MMPLib( const std::string& appKey, const std::string& idfa );
        ~MMPLib();
        
        //!Need call Init and set host and http request header. 
        void Init( const std::string& host = "destiny.li/api/events", const std::string& bannerHost = "destiny.li/api/xpromo", const std::string& header = "Content-Type: application/json" );
        
        //!Setter for host.
        void SetHost( const std::string& host );
        
        //!Setter for http request header.
        void SetHeader( const std::string& header );
        
        //!Converts predefined types to std::string
        static std::string ConvertFromTypeToStr( MarketingPlatform::MMPLib::EventType type );
        
        //!Use this for logging events.
        bool LogEvent( const Json::Value& props );
        
        //!Use this for getting banner;
        Banner* GetBanner();
    };
}