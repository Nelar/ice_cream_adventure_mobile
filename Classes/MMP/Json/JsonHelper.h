//
//  JsonHelper.h
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 2/18/13.
//
//

#pragma once
#include <jsoncpp/include/json/json.h>

namespace Core
{
    class JsonHelper
    {
        
    private:
        
        JsonHelper();
        
    public:
        
        static Json::Value GetValue( const Json::Value& fromValue, const std::string& path, const Json::Value& defValue = Json::nullValue );
        static Json::Value GetValue( const Json::Value& fromValue, int arrayElementIndex, const Json::Value& defValue = Json::nullValue );
        static int ToInt( const Json::Value& value, int defaultResult = 0 );
        static float ToFloat( const Json::Value& value, float defaultResult = 0 );
        static long ToLong( const Json::Value& value, long defaultResult = 0 );
        static std::string ToString( const Json::Value& value, const std::string& defaultResult = "" );
        static bool ToBool( const Json::Value& value, bool defaultResult = false );
    };
}
