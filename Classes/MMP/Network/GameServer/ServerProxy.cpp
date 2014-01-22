//
//  ServerProxy.cpp
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/12/13.
//
//

#include "./ServerProxy.h"
#include "./InputAdapter.h"
#include "./OutputAdapter.h"

namespace AngryPets
{
    namespace GlobalWorld
    {
        namespace Network
        {
            
            ServerProxy::ServerProxy():
            _httpClient( NULL )
            , _inputAdapter( NULL )
            , _outputAdapter( NULL )
            {
                
            }

            ServerProxy::~ServerProxy()
            {
                if( _inputAdapter )
                {
                    delete _inputAdapter;
                }
                if( _outputAdapter )
                {
                    delete _outputAdapter;
                }
                if( _httpClient )
                {
                    cocos2d::extension::CCHttpClient::destroyInstance();
                }
            }

            void ServerProxy::Init()
            {
                SetCookieFileName(cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + "cookie");
                FILE * pFile;
                pFile = fopen ( GetCookieFileName().c_str(),"w");
                if ( !pFile )
                {
                    fclose ( pFile );
                }
                cocos2d::extension::CCHttpClient::cookieFilepath = GetCookieFileName();
                _httpClient = cocos2d::extension::CCHttpClient::getInstance();

                _outputAdapter = new OutputAdapter();

                _inputAdapter = new InputAdapter();
                _inputAdapter->SetServerProxy( this );
                _inputAdapter->SetOutputAdapter( _outputAdapter );
            }

            void ServerProxy::Send( cocos2d::extension::CCHttpRequest* request )
            {
                CCLOG( "ServerProxy::Send %s data %s", request->getUrl(), request->getRequestData() );
                _httpClient->send( request );
            }
            
        }
    }
}