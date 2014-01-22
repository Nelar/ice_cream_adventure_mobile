
#include "ObserverDelegate.h"

namespace Core
{

        ObserverDelegate::ObserverDelegate( EventObserver* obs, EventObserver::EventHandler func ):
        _observer( obs ),
        _function( func )
        {
            
        }

        bool ObserverDelegate::Invoke( Event* event )
        {
            return ( _observer->*_function )( event );
        }

}