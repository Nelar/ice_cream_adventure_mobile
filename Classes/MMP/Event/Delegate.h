//
//  Created by Alexander Simakhin
//

#pragma once

#include "Macros.h"
namespace Core
{


        class DelegateObject
        {
        public:

            //  void SampleHandler();
            typedef void (DelegateObject::*Function)();

        };

#define DELEGATE_FUNCTION( function ) static_cast<Core::DelegateObject::Function>( &function )

        class Delegate
        {

            PROPERTY_GET_SET( DelegateObject::Function, _function, Function );
            PROPERTY_GET_SET( DelegateObject*, _object, Object );

        private:

        protected:

        public:

            inline bool IsValid() const
            {
                return ( ( _function != NULL ) && ( _object != NULL ) );
            }

            Delegate();

            virtual ~Delegate();

            void Invoke();

        };
    
}

