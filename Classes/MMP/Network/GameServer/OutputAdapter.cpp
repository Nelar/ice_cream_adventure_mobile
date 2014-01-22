//
//  OutputAdapter.cpp
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/12/13.
//
//

#include "./OutputAdapter.h"

#include "HttpResponse.h"
#include <Network/Request/NetworkRequest.h>


using namespace cocos2d::extension;

namespace AngryPets
{
    namespace GlobalWorld
    {
        namespace Network
        {
            static bool needStopThread = false;
            static pthread_t                    parserThread;
            static pthread_mutex_t              sleepMutex;
            static pthread_cond_t               sleepCondition;
            static pthread_mutex_t              queueMutex;
            typedef std::list<CCHttpResponse*> ResponseContainer;
            static ResponseContainer*   responseQueue = NULL;
            
            static void AddToQueue( CCHttpResponse* response )
            {
                response->retain();
                pthread_mutex_lock(&queueMutex);
                responseQueue->push_back( response );
                pthread_mutex_unlock(&queueMutex);
            }
            
            static void RemoveFirstFromQueue()
            {
                CCHttpResponse* first = NULL;
                pthread_mutex_lock(&queueMutex);
                first = responseQueue->front();
                responseQueue->pop_front();
                pthread_mutex_unlock(&queueMutex);
                first->release();
            }
            
            static bool GetIsQueueEmpty()
            {
                bool result = false;
                pthread_mutex_lock(&queueMutex);
                result = responseQueue->empty();
                pthread_mutex_unlock(&queueMutex);
                return result;
            }
            
            static CCHttpResponse* GetFirstFromQueue()
            {
                CCHttpResponse* result = NULL;
                pthread_mutex_lock(&queueMutex);
                if ( !responseQueue->empty() )
                {
                    result = responseQueue->front();
                }                
                pthread_mutex_unlock(&queueMutex);
                return result;
            }
            
            static void ProccessRequest( CCHttpResponse* response )
            {
                assert( response );
                NetworkRequest* request = static_cast<NetworkRequest*>( response->getHttpRequest()->getUserData() );
                switch ( request->GetResponseCode() )
                {
                    case 200:
                    {
                        std::vector<char>* responseData = response->getResponseData();
                        if ( responseData )
                        {
                            request->ParseResponse( responseData );
                        }
                    }
                        break;
                    case 500:
                    case 302:
                        request->SetType( NetworkRequest::serverError );
                        break;
                    default:
                        request->SetType( NetworkRequest::connectionError );
                        break;
                }
                request->Signal();
            }
            
            static void* ParseRequest( void* unusedData )
            {
                while (true)
                {
                    CCHttpResponse* response = NULL;
                    bool needWait = ( GetIsQueueEmpty() );
                    if ( needWait )
                    {
                        if ( needStopThread )
                        {
                            break;
                        }
                        else
                        {
                            pthread_cond_wait(&sleepCondition, &sleepMutex);
                            continue;
                        }
                    }
                    else
                    {
                        response = GetFirstFromQueue();
                    }
                    
                    // handler
                    ProccessRequest( response );
                    RemoveFirstFromQueue();
                }
                
                
                return 0;
            }
            
            OutputAdapter::OutputAdapter()
            {
                         
            }

            OutputAdapter::~OutputAdapter()
            {
            }

            cocos2d::extension::CCHttpResponse* OutputAdapter::GetResponse( void* responsePointer )
            {
                CCHttpResponse* response = (CCHttpResponse*) responsePointer;
                
                if( !response )
                {
                    assert( !"memory leak! you have lost pointer to NetworkRequest in CChttpRequest->userData" );
                    return NULL;
                }
                
                // You can get original request type from: response->request->reqType
#ifdef DEBUG
                if( 0 != strlen( response->getHttpRequest()->getTag() ) )
                {
                    LOG("%s completed", response->getHttpRequest()->getTag() );
                }
                int statusCode = response->getResponseCode();
                char statusString[64] = { };
                sprintf( statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag() );
                LOG("response code: %d", statusCode);
#endif
                
                if( !response->isSucceed() )
                {
                    LOG("response failed");
                    LOG("error buffer: %s", response->getErrorBuffer());
                }
                
                return response;
            }

            void OutputAdapter::OnResponse( cocos2d::CCNode* node, void* data )
            {
                CCHttpResponse* response = GetResponse( data );          
                // event which called httpRequest
                NetworkRequest* request = static_cast<NetworkRequest*>( response->getHttpRequest()->getUserData() );
                request->OnRecieve();
                request->SetResponseCode( response->getResponseCode() );
                if( request->GetIsAsynchParsing() )
                {
                    if ( responseQueue == NULL )
                    {
                        responseQueue = new ResponseContainer();
                        pthread_mutex_init( &sleepMutex, NULL );
                        pthread_mutex_init( &queueMutex, NULL );
                        pthread_cond_init( &sleepCondition, NULL );
                        pthread_create( &parserThread, NULL, ParseRequest, NULL );
                    }
                    AddToQueue( response );
                    pthread_cond_signal( &sleepCondition );
                }
                else
                {
                    ProccessRequest( response );
                }
            }

        }
    }
}
