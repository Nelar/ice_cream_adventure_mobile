//
//  utils.h
//  IceCream
//
//  Created by Nelar on 30.10.13.
//
//

#ifndef __IceCream__utils__
#define __IceCream__utils__

#include <iostream>
#include <string>

const char* version();
const char* iOSversion();
const char* idfa();
const char* idfv();
const char* device();
const char* limitAdTracking();
const char* carrier();
void goToLink(const char* link);

void sendlocalNotification(float second);
void removeAllNotification();
const char* getCountry();
bool getNetworkStatus();
const char* networkStatus();
void alertNetwork();
int networkSignal();
std::string push_enabled();
std::string getIsFirstTimeRunning();

void appsFlyerTrackEvent(const char* event, const char* value);


#endif /* defined(__IceCream__utils__) */
