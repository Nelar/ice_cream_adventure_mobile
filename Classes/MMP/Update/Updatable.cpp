//
//  Updatable.cpp
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 3/30/13.
//
//

#include "./Updatable.h"
#include <algorithm>
#include "CCDirector.h"
#include "CCScheduler.h"

namespace Core
{

    Updatable::Updatable( cocos2d::CCObject* updateObject ):
    _registeredHandlers()
    ,_updateObject( updateObject )
    {
        
    }
    
    Updatable::~Updatable()
    {
        Unregister();
    }
    
    bool Updatable::IsRegisteredUpdate() const
    {
        std::list<UpdateHandler>::const_iterator handlerIt = std::find( _registeredHandlers.begin(), _registeredHandlers.end(), UPDATE_HANDLER( cocos2d::CCObject::update ) );
        return handlerIt != _registeredHandlers.end();
    }
    
    bool Updatable::IsRegistered( UpdateHandler handler ) const
    {
        std::list<UpdateHandler>::const_iterator handlerIt = std::find( _registeredHandlers.begin(), _registeredHandlers.end(), handler );
        return handlerIt != _registeredHandlers.end();
    }
    
    void Updatable::RegisterUpdate()
    {
        assert( !IsRegistered( UPDATE_HANDLER( cocos2d::CCObject::update ) ) );
        cocos2d::CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget( _updateObject, 0, false );
        _registeredHandlers.push_back( UPDATE_HANDLER( cocos2d::CCObject::update ) );        
    }
    
    void Updatable::UnregisterUpdate()
    {
        cocos2d::CCDirector::sharedDirector()->getScheduler()->unscheduleUpdateForTarget( _updateObject );
        _registeredHandlers.remove( UPDATE_HANDLER( cocos2d::CCObject::update ) );
    }
    
    void Updatable::Register( UpdateHandler handler, float period, int priority )
    {
        assert( !IsRegistered( handler ) );
        using namespace cocos2d;
        CCDirector::sharedDirector()->getScheduler()->scheduleSelector( (SEL_SCHEDULE) handler, _updateObject, period, false );
        _registeredHandlers.push_back( handler );
    }
    
    void Updatable::Unregister( UpdateHandler handler )
    {
        using namespace cocos2d;
        CCDirector::sharedDirector()->getScheduler()->unscheduleSelector( (SEL_SCHEDULE) handler, _updateObject );
        _registeredHandlers.remove( handler );
    }
    
    void Updatable::Unregister()
    {
        cocos2d::CCDirector::sharedDirector()->getScheduler()->unscheduleAllForTarget( _updateObject );
        _registeredHandlers.clear();
    }
}