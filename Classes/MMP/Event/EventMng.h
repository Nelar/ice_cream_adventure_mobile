//
//  Created by Alexander Simakhin
//

#pragma once

#include "ObserverDelegate.h"
#include "Updatable.h"
#include <map>

namespace Core
{

    typedef HashId EventType;
    
    class EventMng: public cocos2d::CCObject, public Updatable
        {
            
            SINGLETON_UNRELEASABLE( EventMng );
            
        public:
            
            struct ObserverData
            {
                ObserverDelegate delegate;
                int priority;
                bool needDelete;
                
                ObserverData( const ObserverDelegate &delegate, int priority );
            };
            
            typedef std::list< ObserverData > ObserverContainer;
            typedef std::pair<  EventType, ObserverContainer* > EventObserversPair;
            typedef std::map< EventType, ObserverContainer* > EventObserversCtn;
            typedef std::list<Event*> Events;
            
            PROPERTY_PRIVATE( EventObserversCtn, _eventObserversCtn )
            PROPERTY_PRIVATE( Events, _delayedEvents )
            PROPERTY_GET( bool, _enabled, Enabled )
                        
            virtual void update( float dt );
            
            void AddDelayedEvent( Event* event );
            void NotifyObservers( Event* event );
            
        protected:

             EventMng();
            ~EventMng();            

        public:

            template<typename EventType>
            void Register();
            
            void Register( EventType eventType, const ObserverDelegate& delegate, int priority );
            void Unregister( const EventObserver* observer ); // detach all observer's delegates
            void Unregister( EventType eventType, const EventObserver* observer ); // detach observer's delegate for one event            

            //  if you use delayd event you should make them in heap            
            void Process( Event* event );
            
            void SetEnabled( bool enabler );

        };

}

#include "./EventMng.hxx"