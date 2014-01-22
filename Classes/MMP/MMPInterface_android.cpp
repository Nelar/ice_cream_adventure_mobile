//
//  MMPInterface_android.cpp
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 25.10.13.
//
//

#include "MMPInterface.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

namespace Core
{
    std::string MMPInterface::GetIsFirstTimeRunning()
    {
        return "";
    }
    
    std::string MMPInterface::GetSystemVersion()
    {
        return "";
    }
    
    std::string MMPInterface::GetDevice()
    {
        return "";
    }
    
    std::string MMPInterface::GetIP()
    {
        return "";
    }
    
    std::string MMPInterface::GetNet()
    {  
        return "no";
    }
    
    std::string MMPInterface::GetIsPushEnabled()
    {
        return "";
    }
    
    std::string MMPInterface::GetIsJailbroken()
    {
        return "false";
    }
    
}

#endif