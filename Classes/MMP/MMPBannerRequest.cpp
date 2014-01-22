//
//  MMPBannerRequest.cpp
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 25.10.13.
//
//

#include "./MMPBannerRequest.h"

namespace MarketingPlatform
{
    REQUEST_REGISTRATION( MMPBannerRequest )
    
    static Banner::BannerType ConvertFromString( const std::string& format )
    {
        if( format == "popup" )
        {
            return Banner::popup;
        }
        else if( format == "standard" )
        {
            return Banner::standard;
        }
        return Banner::undefined;
    }
    
    MMPBannerRequest* MMPBannerRequest::Create()
    {
        MMPBannerRequest* result = new MMPBannerRequest();
        return result;
    }
    
    MMPBannerRequest::MMPBannerRequest():
    NetworkRequest( cocos2d::extension::CCHttpRequest::kHttpGet )
    {
        SetServerErrorDelay( 10 );
    }
    
    std::string MMPBannerRequest::GetRelativeURI() const
    {
        std::string result = "?app_key=";
        result += _appKey;
        result += "&idfa=";
        result += _idfa;
        return result;
    }
    
    AngryPets::GlobalWorld::NetworkRequest* MMPBannerRequest::Copy() const
    {
        return new MMPBannerRequest( *this );
    }
    
    bool MMPBannerRequest::ParseJson( const Json::Value& root )
    {
        if( !root.empty() )
        {
            const Json::Value& value_format = Core::JsonHelper::GetValue( root, "format" );
            const Json::Value& value_image_url = Core::JsonHelper::GetValue( root, "image_url" );
            const Json::Value& value_landing_url = Core::JsonHelper::GetValue( root, "landing_url" );
            
            _imageUrl = Core::JsonHelper::ToString( value_image_url );
            _landingUrl = Core::JsonHelper::ToString( value_landing_url );
            _bannerType = ConvertFromString( Core::JsonHelper::ToString( value_format ) );
        }
        return !_imageUrl.empty();
    }
}