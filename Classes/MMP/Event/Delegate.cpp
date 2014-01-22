#include "Delegate.h"

#include <assert.h>
namespace Core
{


        Delegate:: Delegate():
        _object( NULL ),
        _function( NULL )
        {
        }

        Delegate::~Delegate()
        {
        }

        void Delegate::Invoke()
        {
            assert( IsValid() );
            ( _object->*_function )();
        }
}