//
//  InputAdapter.cpp
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/12/13.
//
//

#include "./InputAdapter.hxx"
#include "./OutputAdapter.h"
#include "./ServerProxy.h"
#include <Network/Request/NetworkRequest.h>


namespace AngryPets
{
    namespace GlobalWorld
    {
        namespace Network
        {
            std::vector<std::string>* InputAdapter::requestNames;
                      
            using namespace cocos2d::extension;
            using namespace cocos2d;

            InputAdapter::InputAdapter():
            _serverProxy( NULL )
            , _outputAdapter( NULL )
            {
                std::vector<std::string>::iterator it = requestNames->begin();
                const std::vector<std::string>::const_iterator end = requestNames->end();
                for ( ; it != end; ++it )
                {
                    Core::EventObserver::Register( *it, EventHandler( EVENT_HANDLER( InputAdapter::OnNetworkRequest ) ) );
                }
            }

            InputAdapter::~InputAdapter()
            {

            }

            CCHttpRequest* InputAdapter::CreateRequest( NetworkRequest* event )
            {
                assert( event->GetType() == NetworkRequest::startRequest );
                CCHttpRequest* request = new CCHttpRequest();

                request->setRequestType( event->GetRequestType() );
                request->setUrl( event->GetUrl().c_str() );
                request->setHeaders( event->GetHeaders() );

                if( request->getRequestType() == CCHttpRequest::kHttpPost )
                {
                    const std::string& postDataStr = event->GetPostData();
                    if( !postDataStr.empty() )
                    {
                        LOG( "POST data %s", postDataStr.c_str() );
                        request->setRequestData( postDataStr.c_str(), postDataStr.length() );
                    }
                }

                request->setUserData( event );

                request->setResponseCallback( _outputAdapter, event->GetOutputHandler() );
                return request;
            }
            
            void InputAdapter::SendHttpRequest( NetworkRequest* networkRequest )
            {
                networkRequest->OnSend();
                CCHttpRequest* request = CreateRequest( networkRequest );                
                if ( networkRequest->GetUseSavedResponse() )
                {
                    //  imitate response
                    const std::string& responseStr = networkRequest->GetSavedResponse();
                    std::vector<char> responseData;
                    std::copy( responseStr.begin(), responseStr.end(), std::back_inserter( responseData ) );                    
                    CCHttpResponse* response = new CCHttpResponse(request);
                    response->setResponseCode( 200 );
                    response->setSucceed( true );
                    response->setResponseData( &responseData );                    
                    _outputAdapter->OnResponse( NULL, response );
                    response->release();
                }
                else
                {
                    _serverProxy->Send( request );
                    request->release();
                    networkRequest->SetAttempts( networkRequest->GetAttempts() + 1 );
                }
            }
            
            void InputAdapter::FinishRequest( NetworkRequest* networkRequest )
            {
                RequestList::iterator requestIt = std::find( _activeRequests.begin(), _activeRequests.end(), networkRequest );
                assert( requestIt != _activeRequests.end() );
                _activeRequests.erase( requestIt );
                if ( !_waitingRequests.empty() )
                {
                    NetworkRequest* waitingRequest = _waitingRequests.front();
                    _waitingRequests.pop_front();
                    SendHttpRequest( waitingRequest );
                    _activeRequests.push_back( waitingRequest );
                }
            }

            bool InputAdapter::OnNetworkRequest( Core::Event* event )
            {
//                LOG( "InputAdapter::OnNetworkRequest() start %s",  event->GetEventName().c_str() );
                assert( dynamic_cast<NetworkRequest*>( event ) );

                NetworkRequest* networkRequest = static_cast<NetworkRequest*>( event );
                
                switch( networkRequest->GetType() )
                {
                    case NetworkRequest::startRequest:
                    {
//                        LOG( "InputAdapter::OnNetworkRequest() NetworkRequest::startRequest %s",  event->GetEventName().c_str() );
                        networkRequest->Retain();
                        //  it will be realesed in eventMng when output will recieve answer
                        //  and signal this networkRequest again
                        //  OR
                        //  in void InputAdapter::SendHttpRequest( NetworkRequest* networkRequest )
                        //   if ( networkRequest->GetUseSavedResponse() ) == true
                        int maxActiveRequest = 50;
                        if ( _activeRequests.size() < maxActiveRequest )
                        {
                            SendHttpRequest( networkRequest );
                            _activeRequests.push_back( networkRequest );                            
                        }
                        else
                        {
                            _waitingRequests.push_back( networkRequest );
                        }
                    }
                        break;
                    case NetworkRequest::completeRequest:
                        FinishRequest( networkRequest );
                        break;
                    case NetworkRequest::failedRequest:                    
                        FinishRequest( networkRequest );
                        break;
                    case NetworkRequest::connectionError:
                    case NetworkRequest::serverError:
                    {
                        FinishRequest( networkRequest );
                        int maxConnectionAttemps = 3;
                        bool totalFail = networkRequest->GetAttempts() > maxConnectionAttemps;
                        if ( totalFail )
                        {
                            //  show lost connection window
//                            UI::ShowErrorWnd( TRANSLATE("connectionErrorAlert_title"), TRANSLATE("connectionErrorAlert_message"), TRANSLATE("connectionErrorAlert_btnText") );
//                            networkRequest->SetAttempts( 0 );
                        }
                        
                        bool isConnectionError =  networkRequest->GetType() == NetworkRequest::connectionError;
                        float nextRequestSendDelay = (isConnectionError) ? networkRequest->GetConnectionErrorDelay() : networkRequest->GetServerErrorDelay();

                        
                        NetworkRequest* newRequest = networkRequest->Copy();
                        newRequest->SetType( NetworkRequest::startRequest );
                        newRequest->SetDelay( nextRequestSendDelay );
                        newRequest->Signal();
                        networkRequest->SetAttempts( networkRequest->GetAttempts() + 1 );
                        return true;
                    }
                        break;
                    default:
//                        LOG( "InputAdapter::OnNetworkRequest() NetworkRequest::unknown" );                                      
                        assert(!"unknown type");
                        break;
                }
//                LOG( "InputAdapter::OnNetworkRequest() end %s",  event->GetEventName().c_str() );
                return false;
            }

        }
    }
}