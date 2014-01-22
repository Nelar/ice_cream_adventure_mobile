#include "EventMng.hxx"
#include "Event.h"

using namespace std;

namespace Core
{
    static pthread_mutex_t delayedEventsMutex;
    
    EventMng::ObserverData::ObserverData( const ObserverDelegate &delegate, int priority ):
    needDelete( false )
    ,delegate( delegate)
    ,priority( priority )
    {
        
    }
    
    EventMng::EventMng():
    Core::Updatable( this )
    ,_enabled( true )
    {
        pthread_mutex_init(&delayedEventsMutex, NULL);        
    }

    EventMng::~EventMng()
    {

    }
    
    void EventMng::AddDelayedEvent( Event* event )
    {   
        if ( !Core::Updatable::IsRegisteredUpdate() )
        {
            Core::Updatable::RegisterUpdate();
        }
        
        pthread_mutex_lock(&delayedEventsMutex);
        _delayedEvents.push_back( event );
        pthread_mutex_unlock(&delayedEventsMutex);
    }
    
    void EventMng::NotifyObservers( Event* event )
    {
        EventType type = event->GetEventType();   //  if you had exeption here probably you have created delayed event in stack not in heap
        
        EventObserversCtn::iterator cntIt = _eventObserversCtn.find( type );
        
        assert( cntIt != _eventObserversCtn.end() );    //  dont forget REGISTER_EVENT
        ObserverContainer* cnt = cntIt->second;
        
        ObserverContainer::iterator it = cnt->begin();
        //  @next need if @it will be deleted during the loop
        ObserverContainer::const_iterator end = cnt->end();
        
        for ( ; it != end; ++it )
        {
            if( !it->needDelete && it->delegate.Invoke( event ) )
            {
                break;
            }
        }

        it = cnt->begin();
        while ( it != end )
        {
            if ( it->needDelete )
            {
                it = cnt->erase( it );
            }
            else
            {
                ++it;
            }
        }
        
    }

    void EventMng::Process( Event* event )
    {
        if ( event->GetIsDelayed() )
        {
            AddDelayedEvent( event );
        }
        else
        {
            NotifyObservers( event );
        }
    }
    
    void EventMng::update( float dt )
    {
        Event* event = NULL;
        Events currentEvents;
        pthread_mutex_lock(&delayedEventsMutex);
        Events::iterator it = _delayedEvents.begin();
        Events::const_iterator endIt = _delayedEvents.end();
        while ( it != endIt )
        {
            event = *it;
            event->_delay -= dt;
            if ( event->_delay <= 0.f )
            {
                currentEvents.push_back( event );
                it = _delayedEvents.erase( it );
            }
            else
            {
                ++it;
            }
        }
        bool needUnregisterUpdate =  _delayedEvents.empty() ;
        if (needUnregisterUpdate)
        {
            UnregisterUpdate();
        }
        pthread_mutex_unlock(&delayedEventsMutex);
        
        it = currentEvents.begin();
        endIt = currentEvents.end();
        while ( it != endIt )
        {
            event = *it;
            NotifyObservers( event );
            event->Release();
            ++it;
        }

        
       
    }
    
    static void RemoveObserverFromContainer( const EventObserver* observer, EventMng::ObserverContainer* cnt )
    {
        EventMng::ObserverContainer::iterator it = cnt->begin();
        EventMng::ObserverContainer::const_iterator end = cnt->end();
        for ( ; it != end; ++it )
        {
            if( it->delegate.GetObserver() == observer )
            {
                it->needDelete = true;
                break;
            }
        }
    }
    
    struct EqualToObserverPredicate
    {
        const ObserverDelegate& observerDelegate;
        EqualToObserverPredicate( const ObserverDelegate& observerDelegate ):observerDelegate( observerDelegate ) {}
        bool operator () ( const EventMng::ObserverData& data ) { return data.delegate == observerDelegate && !data.needDelete; }
    };
    
    
    static void AddObserverToContainer( const EventMng::ObserverData& data, EventMng::ObserverContainer* cnt )
    {
        //check if observer already in container
        assert( cnt->end() == std::find_if( cnt->begin(), cnt->end(), EqualToObserverPredicate( data.delegate ) ) );
        if ( cnt->empty() )
        {
            cnt->push_back( data );
        }
        else
        {
            //  #todo optimize using binary search
            //  insert into cnt, considering that highest priority is in the begining of container
            EventMng::ObserverContainer::iterator it = cnt->begin();
            EventMng::ObserverContainer::const_iterator endIt = cnt->end();
            for ( ; it != endIt; ++it )
            {
                if ( it->priority <= data.priority )
                {
                    cnt->insert( it, data );
                    break;
                }
            }
            bool newObserverHasLowestPriority = (it == endIt);
            //  if newObserverHasLowestPriority, observer wasnt added to container during the loop
            //  so add it to the end
            if ( newObserverHasLowestPriority )
            {
                cnt->push_back( data );
            }
        }
    }

    void EventMng::Register( EventType eventType, const ObserverDelegate& delegate, int priority )
    {
        EventObserversCtn::iterator it = _eventObserversCtn.find( eventType );
        assert( it != _eventObserversCtn.end() );    //  register event EventMng::Register( EventType eventType ) before!
        ObserverContainer* cnt = it->second;
        ObserverData data( delegate, priority );
        AddObserverToContainer( data, cnt );
    }
    
    void EventMng::Unregister( const EventObserver* observer )
    {
        EventObserversCtn::iterator cntIt = _eventObserversCtn.begin();
        EventObserversCtn::const_iterator endIt = _eventObserversCtn.end();
        ObserverContainer* cnt = NULL;
        for( ; cntIt != endIt; ++cntIt )
        {
            cnt = cntIt->second;
            RemoveObserverFromContainer( observer, cnt );
        }
    }
    
    void EventMng::Unregister( EventType eventType, const EventObserver* observer )
    {
        EventObserversCtn::iterator cntIt = _eventObserversCtn.find( eventType );
        assert( cntIt != _eventObserversCtn.end() );    //  register event EventMng::Register( EventType eventType ) before!
        
        ObserverContainer* cnt = cntIt->second;
        RemoveObserverFromContainer( observer, cnt );

    }

}