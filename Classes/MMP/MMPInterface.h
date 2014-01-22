//
//  MMPInterface.h
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 22.10.13.
//
//

#pragma once
#include <Macros.h>
#include <map>

namespace MarketingPlatform
{
    class MMPLib;
    class Banner;
}

namespace Core
{
    class MMPInterface
    {        
        SINGLETON_UNRELEASABLE( MMPInterface )
        
        PROPERTY_GET( MarketingPlatform::MMPLib*, _mmp, MMP )
        PROPERTY_GET( time_t, _sessionStartTime, SessionStartTime )
        PROPERTY_GETREF( std::string, _productSku, ProductSku )
        
    protected:
        
        std::string GetIsFirstTimeRunning();
        std::string GetSystemVersion();
        std::string GetDevice();
        std::string GetIP();
        std::string GetNet();
        std::string GetIsPushEnabled();
        std::string GetIsJailbroken();
        
        std::string CreateTimeAndKey( int eventType );
        std::string CreateTimeAndKeyNoComma( int eventType );
        
        void gaiEvent(std::string category, std::string action);
        void gaiEvent(std::string category, std::string action, std::map<std::string, std::string>& userData);
        
    public:
        
        ~MMPInterface();
        void Init( const std::string& appKey, const std::string& idfa);
        void StartSession();
        void SessionConfirmed();
        void EndSession();
        void ErrorRaised( const std::string& error, const std::string& reason );
        void UserProfileUpdated( const std::map<std::string, std::string>& userData );
        void StoreOpened();
        void PurchaseMade( float price, const char* profuctSku, std::string currency, bool firstBuyOnDevice );
        void VirtualPurchasMade( const std::string& sku );
        void VirtualBalanceChanged( int amount, const std::string& currency );
        void TutorialCompleted();
        void TutorialSkipped();
        void LevelStarted( const std::string& levelId );
        void LevelCompleted( const std::string& levelId );
        void RankAchieved( int rank );
        void GameCompleted();
        void ShareContentClicked( const std::string& mediaName );
        void VisitFanPageClicked( const std::string& mediaName );
        void InviteFriendsClicked();
        void RateAppClicked( const std::string& choice );
        bool LogEvent( const std::string& jsonStr );
        MarketingPlatform::Banner* GetBanner();
    };
}