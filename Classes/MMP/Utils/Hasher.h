//
//  Created by Alexander Simakhin
//

#pragma once

#include <string>

namespace Core
{
    
        typedef int HashId;

        void MurmurHash3_x86_32( const void* key, int len, uint32_t seed, void* out );

        void MurmurHash3_x86_128( const void* key, int len, uint32_t seed, void* out );

        void MurmurHash3_x64_128( const void* key, int len, uint32_t seed, void* out );

        HashId GetHash( const std::string& val );

}
