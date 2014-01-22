//
//  MMPRequest.h
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 21.10.13.
//
//

#pragma once

#include <Network/Request/NetworkRequest.h>

namespace MarketingPlatform
{
    class MMPRequest : public AngryPets::GlobalWorld::NetworkRequest
    {
        REQUEST_TYPE( MMPRequest )
        
        PROPERTY_GETREF_SETREF( std::string, _appKey, AppKey )
        PROPERTY_GETREF_SETREF( std::string, _idfa, Idfa )
        PROPERTY_GETREF_SETREF( std::string, _batch, Batch )
        PROPERTY_GET( bool, _badRequest, BadRequest )
        
    protected:
        
        virtual bool ParseJson( const Json::Value& root );
        virtual std::string GetRelativeURI() const;
        virtual std::string GetPostData() const;
        virtual AngryPets::GlobalWorld::NetworkRequest* Copy() const;
        
        MMPRequest();
        
    public:
        
        static MMPRequest* Create();
        
    };
}