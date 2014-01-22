//
//  InputAdapter.h
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/12/13.
//
//

#pragma once

#include <Macros.h>
#include <Event/EventObserver.h>
#include <vector>

namespace cocos2d
{
    namespace extension
    {
        class CCHttpRequest;
    }
}

namespace AngryPets
{
    namespace GlobalWorld
    {
        class NetworkRequest;
        
        namespace Network
        {
            class ServerProxy;
            class OutputAdapter;

            class InputAdapter : public Core::EventObserver
            {
                
                typedef std::list<NetworkRequest*> RequestList;                
                
                PROPERTY_PRIVATE( static std::vector<std::string>*, requestNames )
                PROPERTY_PROTECTED( RequestList, _activeRequests )
                PROPERTY_PROTECTED( RequestList, _waitingRequests )
                PROPERTY_GET_SET( ServerProxy*, _serverProxy, ServerProxy )
                PROPERTY_GET_SET( OutputAdapter*, _outputAdapter, OutputAdapter )
                               

            protected:

                cocos2d::extension::CCHttpRequest* CreateRequest( NetworkRequest* event );
                void SendHttpRequest( NetworkRequest* networkRequest );
                bool OnNetworkRequest( Core::Event* event );
                
                void FinishRequest( NetworkRequest* networkRequest );

            public:

                InputAdapter();
                ~InputAdapter();
                
                template <typename RequestType>
                static void Register();

            };
        }
    }
}
#include "./InputAdapter.hxx"
