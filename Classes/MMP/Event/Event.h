//
//  Created by Alexander Simakhin
//

#pragma once

#include "Releasable.h"
#include "./EventMng.h"
#include "Macros.h"

#define EVENT_TYPE( eventAlias )\
friend class Core::EventMng;\
public:\
virtual std::string GetEventName()\
{\
return #eventAlias;\
}\
virtual Core::HashId GetEventType() const\
{\
static const Core::HashId hashKey = Core::GetHash( #eventAlias );\
return hashKey;\
}

#define EVENT_REGISTRATION( eventClassName )\
CALL_ONCE_PREFIX( ::Core::EventMng::Instance()->Register<eventClassName>(), Event##eventClassName );\

namespace Core
{
    class EventMng;
    
    class Event: public Releasable
    {
        friend class EventMng;
        
        PROPERTY_GET_SET( float, _delay, Delay )
        
    public:

        virtual std::string GetEventName() = 0;
        virtual Core::HashId GetEventType() const = 0;
        
        PROPERTY_GET_SET( bool, _isDelayed, IsDelayed ) //  if you use delayd event you should make them in heap


    protected:
        
        virtual void Destroy();

    public:
        
        Event();        
        virtual ~Event();
        virtual void Signal();

    };

}

