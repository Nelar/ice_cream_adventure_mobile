//
//  MMPGetFileRequest.cpp
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 25.10.13.
//
//

#include "./MMPGetFileRequest.h"

namespace MarketingPlatform
{
    REQUEST_REGISTRATION( MMPGetFileRequest )
    
    MMPGetFileRequest* MMPGetFileRequest::Create()
    {
        MMPGetFileRequest* result = new MMPGetFileRequest();
        return result;
    }
    
    MMPGetFileRequest::MMPGetFileRequest():
    NetworkRequest( cocos2d::extension::CCHttpRequest::kHttpGet, file )
    {
        SetServerErrorDelay( 10 );
    }
    
    std::string MMPGetFileRequest::GetRelativeURI() const
    {
        return _fileUrl;
    }
    
    AngryPets::GlobalWorld::NetworkRequest* MMPGetFileRequest::Copy() const
    {
        return new MMPGetFileRequest( *this );
    }
    
    bool MMPGetFileRequest::ParseFileData( const std::vector<char> *responseData )
    {
        _responseData = *responseData;
        return !_responseData.empty();
    }
}