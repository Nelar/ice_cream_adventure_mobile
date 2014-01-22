//
//  ErrorParser.h
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 9/4/13.
//
//

#pragma once
#include <Macros.h>
#include <Json/JsonHelper.h>

namespace AngryPets
{
    namespace GlobalWorld
    {
        class ErrorParser
        {
            
            PROPERTY_GET( bool, _hasError, HasError ) //  if was error during ParseJson
            PROPERTY_GETREF( std::string, _msgText, MsgText )
            PROPERTY_GETREF( std::string, _msgTitle, MsgTitle )
            PROPERTY_GETREF( std::string, _msgBtnText, MsgBtnText )
            
        public:
            
            ErrorParser();
            
            bool ParseError( int codeHash, const Json::Value& element_value );
            
        };
    }
}