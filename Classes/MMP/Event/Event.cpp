#include "Event.h"
#include "EventMng.h"

namespace Core
{

    Event::Event():
    _isDelayed( false )
    ,_delay( 0.f )
    {
    }
    
    Event::~Event()
    {

    }

    void Event::Signal()
    {
//#ifdef DEBUG
//#warning remove this!
//        LOG( "event fired: %s; pointer: %p", GetEventName().c_str(), this );
//#endif
        EventMng::Instance()->Process( this );
    }

    void Event::Destroy()
    {
//#ifdef DEBUG
//#warning remove this!
//        LOG( "event destructing: %s; pointer: %p", GetEventName().c_str(), this );
//#endif
        delete this;
    }
//    
//#ifdef DEBUG
//    std::string Event::GetEventName()
//    {
//        return "default EVENT";
//    }
//#endif
    
}