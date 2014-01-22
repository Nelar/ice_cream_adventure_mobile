//
//  MMPRequest.cpp
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 21.10.13.
//
//

#include "./MMPRequest.h"

namespace MarketingPlatform
{
    REQUEST_REGISTRATION( MMPRequest )
    
    MMPRequest* MMPRequest::Create()
    {
        MMPRequest* result = new MMPRequest();
        return result;
    }
    
    MMPRequest::MMPRequest():
    NetworkRequest( cocos2d::extension::CCHttpRequest::kHttpPost )
    ,_badRequest( false )
    {
        SetServerErrorDelay( 10 );
    }
    
    std::string MMPRequest::GetRelativeURI() const
    {
        std::string result = "?app_key=";
        result += _appKey;
        result += "&idfa=";
        result += _idfa;
        return result;
    }
    
    std::string MMPRequest::GetPostData() const
    {
        return _batch;
    }
    
    AngryPets::GlobalWorld::NetworkRequest* MMPRequest::Copy() const
    {
        return new MMPRequest( *this );
    }
    
    bool MMPRequest::ParseJson( const Json::Value& root )
    {
        int responseCode = GetResponseCode();
        bool error = false;
        if( responseCode == 200 )
        {
            SetType( completeRequest );
        }
        else if( responseCode == 400 )
        {
            error = true;
            _badRequest = true;
        }
        else if( responseCode == 500 )
        {
            error = true;
            _badRequest = false;
        }
        else //something is wrong
        {
            error = true;
            _badRequest = true;
        }
        return !error;
    }
}