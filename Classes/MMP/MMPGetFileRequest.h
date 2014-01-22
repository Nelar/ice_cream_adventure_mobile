//
//  MMPGetFileRequest.h
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 25.10.13.
//
//

#pragma once

#include "Network/Request/NetworkRequest.h"

namespace MarketingPlatform
{
    class MMPGetFileRequest : public AngryPets::GlobalWorld::NetworkRequest
    {
        REQUEST_TYPE( MMPGetFileRequest )
        
        PROPERTY_GETREF_SETREF( std::string, _fileUrl, FileUrl )
        PROPERTY_GETREF_SETREF( std::vector<char>, _responseData, ResponseData )
        
    protected:
        
        virtual bool ParseFileData( const std::vector<char>* responseData );
        virtual std::string GetRelativeURI() const;
        virtual AngryPets::GlobalWorld::NetworkRequest* Copy() const;
        
        MMPGetFileRequest();
        
    public:
        
        static MMPGetFileRequest* Create();
        
    };
}