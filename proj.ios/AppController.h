//
//  FourPicsOneWordAppController.h
//  FourPicsOneWord
//
//  Created by Philipp Mayevski on 7/16/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//

#define APP_HANDLED_URL @"APP_HANDLED_URL"
#import <FacebookSDK/FacebookSDK.h>

@class RootViewController;


@interface AppController : UIResponder <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate,UIApplicationDelegate, FBLoginViewDelegate, UIAlertViewDelegate > {
    UIWindow *window;
    RootViewController    *viewController;
}

-(void) orientationDidChanged:(NSNotification*)notification;

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) RootViewController *viewController;
@property (nonatomic, assign) BOOL appUsageCheckEnabled;

@end

