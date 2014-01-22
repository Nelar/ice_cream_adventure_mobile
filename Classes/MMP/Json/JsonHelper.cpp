//
//  JsonHelper.cpp
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/18/13.
//
//

#include "JsonHelper.h"
#include "../Utils/Convert.h"
#include "../TypesDefs.h"

namespace Core
{
    Json::Value JsonHelper::GetValue( const Json::Value& fromValue, const std::string& path, const Json::Value& defValue )
    {              
        Json::Value node = fromValue;
        int foundPosFirst = 0;
        int foundPosSecond = path.find( "/" );
        bool slashFound = ( foundPosSecond != std::string::npos );
        
        while( slashFound )
        {
            if ( node.type() != Json::objectValue )
            {
                return defValue;
            }
            const std::string& nodeName = path.substr( foundPosFirst, foundPosSecond - foundPosFirst );
            node = node.get( nodeName.c_str(), Json::nullValue );
            foundPosFirst = foundPosSecond + 1;
            foundPosSecond = path.find( "/", foundPosFirst );
            slashFound = ( foundPosSecond != std::string::npos );
        }

        if ( node.type() != Json::objectValue )
        {
            return defValue;
        }
        const std::string& nodeName2 = path.substr( foundPosFirst, path.length() - foundPosFirst );
        node = node.get( nodeName2.c_str(), Json::nullValue );
        if ( node.isNull() )
        {
            return defValue;
        }
        else
        {
            return node;
        }
    };
    
    Json::Value JsonHelper::GetValue( const Json::Value& fromValue, int arrayElementIndex, const Json::Value& defValue )
    {
        if ( !(fromValue.isArray() && fromValue.isValidIndex( (Json::UInt) arrayElementIndex )) )
        {
            return defValue;
        }
        return fromValue.get(  (Json::UInt) arrayElementIndex, defValue );        
    }
    
    int JsonHelper::ToInt( const Json::Value& value, int defaultResult )
    {
        if ( value.isInt() )
        {
            return value.asInt();
        }
        if ( value.isDouble() )
        {
            return (int) value.asDouble();
        }
        if ( value.isString() )
        {
            return Core::Convert::ToInt( value.asString() );
        }
        return defaultResult;
    }
    
    float JsonHelper::ToFloat( const Json::Value& value, float defaultResult )
    {
        if ( value.isDouble() )
        {
            return (float) value.asDouble();
        }
        if ( value.isInt() )
        {
            return (float) value.asInt();
        }
        if ( value.isString() )
        {
            return Convert::ToFloat( value.asString() );
        }        
        return defaultResult;
    }
    
    std::string JsonHelper::ToString( const Json::Value& value, const std::string& defaultResult )
    {
        if ( value.isString() )
        {
            return value.asString();
        }
        if ( value.isInt() )
        {
            return Convert::ToString( value.asInt() );
        }
        if ( value.isDouble() )
        {
            return Convert::ToString( (float)value.asDouble() );
        }
        return defaultResult;
    }
    
    long JsonHelper::ToLong( const Json::Value& value, long defaultResult )
    {
        if ( value.isUInt() )
        {
            return  value.asUInt();
        }
        if ( value.isInt() )
        {
            return ((long)value.asInt());
        }
        if ( value.isDouble() )
        {
            return value.asDouble();
        }
        if ( value.isString() )
        {
            return Core::Convert::ToLong( value.asString() );
        }
        return defaultResult;
    }
    
    bool JsonHelper::ToBool( const Json::Value& value, bool defaultResult )
    {
        if ( value.isBool() )
        {
            return value.asBool();
        }
        if ( value.isInt() )
        {
            return (bool)value.asInt();
        }
        if ( value.isDouble() )
        {
            return (bool)value.asDouble();
        }
        if ( value.isUInt() )
        {
            return (bool)value.asUInt();
        }
        if ( value.isString() )
        {
            const std::string& str = value.asString();
            return ( str == "true" || str == "1" );
        }
        return defaultResult;
    }
}