//
//  TW.cpp
//  FourPicsOneWord
//
//  Created by Phil Maevskiy on 9/6/13.
//
//

#include "TW.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include <Twitter/Twitter.h>
#include <UIKit/UIKit.h>


void TW::tweet(string messageText)
{
    
    if ([TWTweetComposeViewController canSendTweet])
    {
        TWTweetComposeViewController *tweetSheet =
        [[TWTweetComposeViewController alloc] init];
       
        [tweetSheet setInitialText: [NSString stringWithUTF8String:messageText.c_str()] ];
	    [[UIApplication sharedApplication].keyWindow.rootViewController presentModalViewController:tweetSheet animated:YES];
    }
    else
    {
        UIAlertView *alertView = [[UIAlertView alloc]
                                  initWithTitle:@"Sorry"
                                  message:@"You can't send a tweet right now, make sure your device has an internet connection and you have at least one Twitter account setup"
                                  delegate:nil
                                  cancelButtonTitle:@"OK"
                                  otherButtonTitles:nil];
        [alertView show];
    }
}

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//android implementation
#endif
