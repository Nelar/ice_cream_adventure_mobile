#include "./EventObserver.h"
#include "./EventMng.h"
#include "./Event.h"

namespace Core
{

    void EventObserver::Register( const std::string& eventTypeName, EventHandler handler, int priority )
    {
#ifdef DEBUG
        _eventTypeNames.push_back( eventTypeName );
//        LOG( "Event observer register %s, %p", eventTypeName.c_str(), this );
#endif
        EventType eventType = Core::GetHash( eventTypeName );
        EventMng::Instance()->Register( eventType, ObserverDelegate( this, handler ), priority );
    }

    void EventObserver::Unregister() const
    {
#ifdef DEBUG
        for ( std::list<std::string>::const_iterator it = _eventTypeNames.begin(), end = _eventTypeNames.end(); it != end; ++it )
        {
//            LOG( "Event observer unregister %s, %p", it->c_str(), this );
        }
        _eventTypeNames.clear();
#endif
        EventMng::Instance()->Unregister( this );
    }

    void EventObserver::Unregister( const std::string& eventTypeName ) const
    {
#ifdef DEBUG        
//        LOG( "Event observer unregister %s, %p", eventTypeName.c_str(), this );
        std::remove( _eventTypeNames.begin(), _eventTypeNames.end(), eventTypeName );
#endif
        EventType eventType = Core::GetHash( eventTypeName );
        EventMng::Instance()->Unregister( eventType, this );
    }

    EventObserver::~EventObserver()
    { 
        Unregister();
    }

}