#include "cocos2d.h"
#include <sstream>
#include <time.h>
#include "Convert.h"

namespace Core
{
    
    std::string Convert::ToString( float var )
    {
        std::stringstream ss;
        ss << var;
        return ss.str();
    }

    std::string Convert::ToString( int var )
    {
        std::stringstream ss;
        ss << var;
        return ss.str();
    }
    
    std::string Convert::ToString( bool var )
    {
        if( var )
        {
            return "true";
        }
        else
        {
            return "false";
        }
    }
    
    std::string Convert::ToString( Core::Time var )
    {
        std::stringstream ss;
        ss << var;
        return ss.str();
    }    
    
    float Convert::ToFloat( const std::string& var )
    {
        return atof( var.c_str() );
    }
    
    int Convert::ToInt( const std::string& var )
    {
        return atoi( var.c_str() );
    }
    
    unsigned int Convert::ToUInt( const std::string& var )
    {
        assert( !"#todo" );
        return 0;
    }
    
    long Convert::ToLong( const std::string& var )
    {
//        assert( !"#todo works wrong" );        
        return atol( var.c_str() );
    }
    
}