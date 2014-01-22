//
//  NetworkRequest.h
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/18/13.
//
//

#pragma once

#include <Event/Event.h>
#include "HttpRequest.h"
#include <jsoncpp/include/json/json.h>
#include <Network/GameServer/InputAdapter.h>
#include <Network/Request/ErrorParser.h>

namespace Core
{
    class XmlNode;
}

namespace AngryPets
{
    namespace GlobalWorld
    {
        class NetworkRequest :  public Core::Event,
                                public ErrorParser
            {

            public:
                
                typedef std::vector<std::string> HeadersContainer;

                enum Type
                {
                    undefinded = -1,
                    startRequest = 0,
                    completeRequest,
                    failedRequest,
                    connectionError,
                    serverError
                };
                
                enum ResponseDataType
                {
                    unknown,
                    json,
                    xml,
                    file
                };

                PROPERTY_GET_SET( NetworkRequest::Type, _type, Type )
                PROPERTY_GET_SET( int, _attempts, Attempts )
                PROPERTY_GET( ResponseDataType, _responseDataType, ResponseDataType )
                PROPERTY_GETREF_SETREF( cocos2d::extension::CCHttpRequest::HttpRequestType, _requestType, RequestType )
                PROPERTY_GETREF_SETREF( std::string, _scheme, Scheme )   //  http or https
                PROPERTY_GETREF_SETREF( std::string, _host, Host )
                PROPERTY_GETREF_SETREF( cocos2d::SEL_CallFuncND, _outputHandler, OutputHandler );
                
                PROPERTY_GETREF_SETREF( std::string, _savedResponse, SavedResponse )
                PROPERTY_GET_SET( bool, _useSavedResponse, UseSavedResponse )
                PROPERTY_GET_SET( bool, _needSaveResponse, NeedSaveResponse )
                PROPERTY_GET_SET( bool, _isAsynchParsing, IsAsynchParsing )
                PROPERTY_GET_SET( int, _responseCode, ResponseCode )
                PROPERTY_GET_SET( float, _connectionErrorDelay, ConnectionErrorDelay );
                PROPERTY_GET_SET( float, _serverErrorDelay, ServerErrorDelay );
                PROPERTY_GETREF_SETREF( std::vector<std::string>, _headers, Headers )
                
            private:
                
                NetworkRequest();
                
            protected:
                
                virtual bool ParseJson( const Json::Value& root );
                virtual bool ParseXml( const Core::XmlNode* node );
                virtual bool ParseFileData( const std::vector<char>* responseData );

            public:
                
                NetworkRequest( cocos2d::extension::CCHttpRequest::HttpRequestType requestType, ResponseDataType responseDataType = json );
                virtual NetworkRequest* Copy() const = 0;

                std::string GetUrl() const;
                virtual std::string GetPostData() const;
                virtual std::string GetRelativeURI() const = 0;
                void ParseResponse( const std::vector<char>* responseData );                
                
                virtual void OnSend();
                virtual void OnRecieve();
                
#ifdef DEBUG                
                void SetDebugResponse( const std::string& filename );
                PROPERTY_GET( bool, _useDebugResponse, UseDebugResponse )
                PROPERTY_GETREF( std::vector<char>, _debugResponse, DebugResponse )
#endif
            };
        
        
    }
}

#include <Event/EventMng.h>
#include <Network/GameServer/InputAdapter.h>
#include <Macros.h>
#define REQUEST_TYPE( alias )\
friend class ::AngryPets::GlobalWorld::Network::InputAdapter; \
EVENT_TYPE( alias );

#define REQUEST_REGISTRATION( className ) \
EVENT_REGISTRATION( className ) \
CALL_ONCE_PREFIX( ::AngryPets::GlobalWorld::Network::InputAdapter::Register<className>(), InputAdapter##className ); \


