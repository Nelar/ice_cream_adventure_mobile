//
//  OutputAdapter.h
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/12/13.
//
//

#pragma once

#include <Macros.h>
#include <jsoncpp/include/json/json.h>
#include "cocos2d.h"

namespace cocos2d
{
    namespace extension
    {
        class CCHttpResponse;
    }
}

namespace AngryPets
{
    namespace GlobalWorld
    {
        namespace Network
        {
            class OutputAdapter : public cocos2d::CCObject
            {

            protected:

                static cocos2d::extension::CCHttpResponse* GetResponse( void* responsePointer );

            public:

                OutputAdapter();

                ~OutputAdapter();

                void OnResponse( cocos2d::CCNode* node, void* data );                

            };
        }
    }
}
