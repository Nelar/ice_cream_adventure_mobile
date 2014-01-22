//
//  Banner.h
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 25.10.13.
//
//

#pragma once
#include "Macros.h"
#include "cocos2d.h"

namespace MarketingPlatform
{
    class Banner
    {
        
    public:
        
        enum BannerType
        {
            undefined = 0
            ,standard
            ,popup
        };
        
        PROPERTY_GET( BannerType, _type, Type )
        PROPERTY_GETREF( std::string, _landingUrl, LandingUrl )
        PROPERTY_GET( cocos2d::CCSprite*, _sprite, Sprite )
        
        Banner( BannerType type, const std::string landingUrl, const char* imageFilePath );
        ~Banner();
    };
}