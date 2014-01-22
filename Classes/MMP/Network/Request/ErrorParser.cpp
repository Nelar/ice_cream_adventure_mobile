//
//  ErrorParser.cpp
//  angrypetsmobile
//
//  Created by Alexander Simakhin on 9/4/13.
//
//

#include "ErrorParser.h"

namespace AngryPets
{
    namespace GlobalWorld
    {
        static const Core::HashId errorMsgCodeHash = Core::GetHash( "error_message" );
        static const Core::HashId tryAgainMsgCodeHash = Core::GetHash( "try_again" );
        static const Core::HashId noMoneyCodeHash = Core::GetHash( "no_money" );
        static const Core::HashId unknownErrorCodeHash = Core::GetHash( "unknown_error" );
        
        ErrorParser::ErrorParser():
        _hasError( false )
        ,_msgText( "Error" )
        ,_msgTitle( "Unknown error" )
        ,_msgBtnText( "Ok" )
        {
            
        }
        
        bool ErrorParser::ParseError( Core::HashId codeHash, const Json::Value& element_value )
        {
            bool isError = ( codeHash == errorMsgCodeHash ) || ( codeHash == noMoneyCodeHash ) || ( unknownErrorCodeHash == codeHash ) || ( codeHash == tryAgainMsgCodeHash );
            if( isError )
            {
                _hasError = true;
                const Json::Value& value_params = Core::JsonHelper::GetValue( element_value, "params" );
                const Json::Value& value_msg = Core::JsonHelper::GetValue( value_params, "message" );
                const Json::Value& value_title = Core::JsonHelper::GetValue( value_params, "title" );
                const Json::Value& value_btnText = Core::JsonHelper::GetValue( value_params, "btn_text" );
                _msgText = Core::JsonHelper::ToString( value_msg );
                _msgTitle = Core::JsonHelper::ToString( value_title, "Error" );
                _msgBtnText = Core::JsonHelper::ToString( value_btnText, "Ok" );
                return true;
            }
            return false;
        }
    }
}