//
//  MMPBannerRequest.h
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 25.10.13.
//
//

#pragma once

#include <Network/Request/NetworkRequest.h>
#include "./Banner.h"

namespace MarketingPlatform
{
    class MMPBannerRequest : public AngryPets::GlobalWorld::NetworkRequest
    {
        REQUEST_TYPE( MMPBannerRequest )        
        
        PROPERTY_GETREF_SETREF( std::string, _appKey, AppKey )
        PROPERTY_GETREF_SETREF( std::string, _idfa, Idfa )
        PROPERTY_GETREF_SETREF( std::string, _imageUrl, ImageUrl )
        PROPERTY_GETREF_SETREF( std::string, _landingUrl, LandingUrl )
        PROPERTY_GET_SET( Banner::BannerType, _bannerType, BannerType )
        
    protected:
        
        virtual bool ParseJson( const Json::Value& root );
        virtual std::string GetRelativeURI() const;
        virtual AngryPets::GlobalWorld::NetworkRequest* Copy() const;
        
        MMPBannerRequest();
        
    public:
        
        static MMPBannerRequest* Create();
        
    };
}