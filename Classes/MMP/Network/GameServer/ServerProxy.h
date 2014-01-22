//
//  ServerProxy.h
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/12/13.
//
//

#pragma once
#include <Macros.h>
#include "HttpClient.h"

namespace AngryPets
{
    namespace GlobalWorld
    {
        namespace Network
        {

            class InputAdapter;
            class OutputAdapter;

            class ServerProxy
            {

                PROPERTY_PROTECTED( cocos2d::extension::CCHttpClient*, _httpClient );
                PROPERTY_PROTECTED( InputAdapter*, _inputAdapter );
                PROPERTY_PROTECTED( OutputAdapter*, _outputAdapter );
                PROPERTY_GETREF_SETREF( std::string, _cookieFileName, CookieFileName )
                

            public:

                ServerProxy();
                ~ServerProxy();

                void Init();

                void Send( cocos2d::extension::CCHttpRequest* request );

            };
        }
    }
}
