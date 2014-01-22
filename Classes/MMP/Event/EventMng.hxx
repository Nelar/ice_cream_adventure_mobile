//
//  EventMng.hxx
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/12/13.
//
//
#pragma once

#include "./EventMng.h"
#include <assert.h>

namespace Core
{
    template<typename EventType>
    inline void EventMng::Register()
    {
        HashId classHashKey = EventType().GetEventType();
        
        assert( _eventObserversCtn.find( classHashKey ) == _eventObserversCtn.end() );   //  already register event
        
        ObserverContainer* cnt = new ObserverContainer();
        _eventObserversCtn.insert( EventObserversPair( classHashKey, cnt ) );
    }
    
}
