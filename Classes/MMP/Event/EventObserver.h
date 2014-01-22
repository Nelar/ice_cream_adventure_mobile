//
//  Created by Alexander Simakhin
//

#pragma once

#include "Macros.h"
#include <list>
#include <string>

namespace Core
{
        class Event;
        class EventObserver
        {

        public:

            virtual ~EventObserver();

            typedef bool (EventObserver::*EventHandler)( Core::Event* );

            // bool SampleHandler( Core::Event* event );
            void Register( const std::string& eventTypeName, EventHandler handler, int priority = 0 );
            void Unregister() const;
            void Unregister( const std::string& eventTypeName ) const;
            
#ifdef DEBUG
            PROPERTY_PROTECTED( mutable std::list<std::string>, _eventTypeNames )
#endif

        };
}

#define EVENT_HANDLER( function ) static_cast<Core::EventObserver::EventHandler>( &function )
