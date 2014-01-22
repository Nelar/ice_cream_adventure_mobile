//
//  Created by Alexander Simakhin
//

#pragma once
#include <string>
#include <vector>

#include "../TypesDefs.h"

namespace Core
{
        class Convert
        {

        private:

            Convert();

        protected:

        public:
            
            enum TimeComponent
            {
                timeComponentSecond = 0,
                timeComponentMinute = 1,
                timeComponentHour = 2
            };
            
            static float ToFloat( const std::string& var );            
            static int ToInt( const std::string& var );
            static unsigned int ToUInt( const std::string& var );
            static long ToLong( const std::string& var );

            static std::string ToString( float var );
            static std::string ToString( int var );
            static std::string ToString( bool var );
            static std::string ToString( Core::Time var );                                   

        };

}