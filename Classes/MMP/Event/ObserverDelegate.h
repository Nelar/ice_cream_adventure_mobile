//
//  Created by Alexander Simakhin
//

#pragma once

#include "EventObserver.h"

namespace Core
{


        class ObserverDelegate
        {
        public:
//        private:

            EventObserver::EventHandler _function;
            EventObserver* _observer;

            ObserverDelegate();

        public:

            ObserverDelegate( EventObserver* obs, EventObserver::EventHandler func );

            //	if return true ->finish signal
            //	false continue calling another observers
            bool Invoke( Event* event );

            inline EventObserver* GetObserver() const
            {
                return _observer;
            }

            bool operator==( const ObserverDelegate& var ) const
            {
                return ( ( var._function == _function ) && ( var._observer == _observer ) );
            }

        };
}
