//
//  Created by Alexander Simakhin
//

#pragma once

#include "cocoa/CCGeometry.h"
#include <Macros.h>
#include "cocos2d.h"
#include <string>
#include <stdint.h>
#include <list>
#include <map>
#include <vector>
#include <algorithm>

namespace Core
{
    typedef cocos2d::CCSize RectSize;
    typedef cocos2d::CCSize Resolution;
    typedef cocos2d::CCRect Rect;
    
    typedef time_t Time;
    typedef float Second;
    typedef float Minute;
    typedef float Hour;
    typedef float Degree;
    
    typedef std::string DescriptionId;
    typedef int ElementId;
    static const DescriptionId undefinedDescriptionId = "";
    
}

