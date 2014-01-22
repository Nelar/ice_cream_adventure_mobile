//
//  Updatable.h
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 3/30/13.
//
//

#pragma once

#include "Macros.h"
#include "cocos2d.h"
#include <list>

namespace Core
{
    typedef void (cocos2d::CCObject::*UpdateHandler)( float dt );
    
    class Updatable
    {
        PROPERTY_PROTECTED( std::list<UpdateHandler>, _registeredHandlers )
        PROPERTY_PROTECTED( cocos2d::CCObject*, _updateObject )
        
    private:
        
        Updatable();
        
    protected:
    
        void Register( UpdateHandler handler, float period, int priority = 0 );
        void RegisterUpdate();
        void UnregisterUpdate();
        void Unregister( UpdateHandler handler );
        void Unregister();
        bool IsRegistered( UpdateHandler handler ) const;
        bool IsRegisteredUpdate() const;
        Updatable( cocos2d::CCObject* updateObject );
                
    public:
        
        virtual ~Updatable();
        
    };
}


#define UPDATE_HANDLER( function ) static_cast<Core::UpdateHandler>( &function )