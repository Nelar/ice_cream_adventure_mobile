//
//  InputAdapter.hxx
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 4/6/13.
//
//

#pragma once
#include "./InputAdapter.h"
#include <assert.h>
#include "cocos2d.h"

namespace AngryPets
{
    namespace GlobalWorld
    {
        namespace Network
        {
            template <typename RequestType>
            inline void InputAdapter::Register()
            {
                if( !requestNames )
                {
                    requestNames = new std::vector<std::string>();
                }
                const std::string& eventName = RequestType().GetEventName();
                assert( std::find( requestNames->begin(), requestNames->end(), eventName ) == requestNames->end() );
                requestNames->push_back( eventName );
                LOG( "names %p", &requestNames );
            }
        }
    }
}