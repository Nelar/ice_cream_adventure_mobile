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

const char* version();
const char* idfa();
void goToLink(const char* link);

void sendlocalNotification(float second);
void removeAllNotification();
const char* getCountry();
bool getNetworkStatus();

#endif /* defined(__IceCream__utils__) */
