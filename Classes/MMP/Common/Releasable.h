//
//  Releasable.h
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/7/13.
//
//

#pragma once

namespace Core
{
    class Releasable
    {

        public:

        virtual ~Releasable(){};

    protected:

        int _refCounter;

        Releasable():
        _refCounter( 0 )
        {

        }
        
        Releasable( const Releasable& ext ):
        _refCounter( 0 )
        {
            
        }

        virtual void Destroy() = 0; // destroy me here at all
        
    public:
        
        inline void Retain()
        {
            ++_refCounter;
        }
        
        inline void Release()
        {
            if ( _refCounter == 0 )
            {
                Destroy();
            }
            else
            {
                --_refCounter;
            }
        }

    };
}
