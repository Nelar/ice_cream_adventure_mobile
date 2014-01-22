
//
//  NetworkRequest.cpp
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/18/13.
//
//

#include "NetworkRequest.h"
#include <sstream>
#include <Network/GameServer/OutputAdapter.h>

namespace AngryPets
{
    namespace GlobalWorld
    {
        
        using namespace cocos2d;
        
        NetworkRequest::NetworkRequest(  cocos2d::extension::CCHttpRequest::HttpRequestType requestType
                                       ,ResponseDataType responseDataType)
        :
        _type( startRequest )
        ,_attempts( 0 )
        ,_requestType( requestType )
        ,_scheme( "http://" )
        ,_outputHandler( callfuncND_selector( GlobalWorld::Network::OutputAdapter::OnResponse ) )
        ,_responseDataType( responseDataType )
        ,_useSavedResponse( false )
        ,_needSaveResponse( false )
        ,_isAsynchParsing( true )
        ,_responseCode( 0 )
        ,_serverErrorDelay( 7.f )
        ,_connectionErrorDelay( 0.1f )
#ifdef DEBUG
        ,_useDebugResponse( false )
#endif
        {
            SetIsDelayed( true );            
        }
        
        std::string NetworkRequest::GetUrl() const
        {
            if( _responseDataType == file )
            {
                return GetRelativeURI();
            }
            else
            {
                return ( GetScheme() + GetHost() + "/" + GetRelativeURI() );
            }
        }
        
        std::string NetworkRequest::GetPostData() const
        {
            assert( !"implement in derived class" );
            return std::string();
        }
        
        bool NetworkRequest::ParseJson( const Json::Value& root )
        {            
            // flash
            const Json::Value& value_flash = Core::JsonHelper::GetValue( root, "flash" );
            if ( value_flash.isArray() )
            {
                for ( Json::Value::iterator it = value_flash.begin(); it != value_flash.end(); ++it )
                {
                    const Json::Value& element_value = *it;
                    const Json::Value& code_value = Core::JsonHelper::GetValue( element_value, "code" );
                    const std::string& codeStr = Core::JsonHelper::ToString( code_value );
                    Core::HashId codeHash = Core::GetHash( codeStr );
                    if ( ErrorParser::ParseError( codeHash, element_value ) )
                    {
                        continue;
                    }
                    
                }
            }
            return true;
        }
        
        bool NetworkRequest::ParseXml( const Core::XmlNode* node )
        {
            assert(!"implement in derived class");
            return false;
        }
        
        bool NetworkRequest::ParseFileData( const std::vector<char> *responseData )
        {
            assert( !"implement in derived class" );
            return false;
        }
        
        void NetworkRequest::ParseResponse( const std::vector<char>* responseData )
        {
#ifdef DEBUG
            if ( GetUseDebugResponse() )
            {
                responseData = &GetDebugResponse();
            }
#endif
            if( !responseData )
            {
                SetType( NetworkRequest::failedRequest );
                return;
            }
#ifdef DEBUG_REQUEST            
            if( GetRequestType() == cocos2d::extension::CCHttpRequest::kHttpGet )
            {
                LOG("GET request URL: %s", GetUrl().c_str() );
            }
            else
            {
                LOG("POST request URL: %s", GetUrl().c_str() );
                
                const std::string& postData = GetPostData();
                LOG("POST size=%li data=", postData.size() );
                for( unsigned int i = 0; i < postData.size(); ++i )
                {
                    printf( "%c", postData[i] );
                }
            }
            std::stringstream ss;
            for( unsigned int i = 0; i < responseData->size(); ++i )
            {
                ss << ( *responseData )[i];
            }
            LOG("Http dump data: \n %s", ss.str().c_str() );
#endif
            if ( _needSaveResponse )
            {
                _savedResponse.clear();
                std::copy( responseData->begin(), responseData->end(), std::back_inserter( _savedResponse ) );
            }
            
            bool isRequestCompleted = false;
            if ( GetResponseDataType() == json )
            {
                Json::Value root;
                Json::Reader reader;
                
                const char* beginDoc = &responseData->at( 0 );
                const char* endDoc = beginDoc + responseData->size();
                bool isJsonCorrect = responseData->size() > 0 && reader.parse( beginDoc, endDoc, root ) && !root.isNull();                
                isRequestCompleted = isJsonCorrect && ParseJson( root ) && !GetHasError();
            }
            else if ( GetResponseDataType() == xml )
            {
                isRequestCompleted = true;
            }
            else if ( GetResponseDataType() == file )
            {
                isRequestCompleted = ParseFileData( responseData );
            }

            if( isRequestCompleted )
            {
                SetType( NetworkRequest::completeRequest );
            }
            else
            {
                LOG( "Failed parse Response" );
                SetType( NetworkRequest::failedRequest );
            }
        }
        
        void NetworkRequest::OnSend()
        {
            
        }
        
        void NetworkRequest::OnRecieve()
        {
            
        }
#ifdef DEBUG
        void NetworkRequest::SetDebugResponse( const std::string& filename )
        {
            
            _useDebugResponse = true;
            
            std::string filepath =  "temp/request/" + filename;
            filepath = cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename( filepath.c_str() );
            unsigned long size = 0;
            unsigned char* arr = cocos2d::CCFileUtils::sharedFileUtils()->getFileData( filepath.c_str(), "rb", &size );
            for ( unsigned long i = 0; i < size; ++i )
            {
                _debugResponse.push_back( arr[i] );
            }
            if (arr)
            {
                delete arr;
            }
            
        }
#endif
    }
}
