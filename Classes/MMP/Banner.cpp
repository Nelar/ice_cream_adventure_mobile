//
//  Banner.cpp
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 25.10.13.
//
//

#include "Banner.h"

namespace MarketingPlatform
{
    Banner::Banner( BannerType type, const std::string landingUrl, const char* imageFilePath ):
    _type( type )
    ,_landingUrl( landingUrl )
    ,_sprite( NULL )
    {
        _sprite = cocos2d::CCSprite::create( imageFilePath );
        _sprite->retain();
    }
    
    Banner::~Banner()
    {
        _sprite->release();
        _sprite = NULL;
    }
}