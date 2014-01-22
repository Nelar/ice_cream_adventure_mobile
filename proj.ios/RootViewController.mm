//
//  FourPicsOneWordAppController.h
//  FourPicsOneWord
//
//  Created by Philipp Mayevski on 7/16/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//

#import "RootViewController.h"

#include "cocos2d.h"


@implementation RootViewController

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if ((self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
 
*/
// Override to allow orientations other than the default portrait orientation.
// This method is deprecated on ios6
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    return true;
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {
 /*   CGSize s;
    if (UIInterfaceOrientationIsLandscape(UIApplication.sharedApplication.statusBarOrientation)) {
          s = CGSizeMake(std::max<float>(UIScreen.mainScreen.bounds.size.width, UIScreen.mainScreen.bounds.size.height), std::min<float>(UIScreen.mainScreen.bounds.size.width, UIScreen.mainScreen.bounds.size.height));
        } else {
              s = CGSizeMake(std::min<float>(UIScreen.mainScreen.bounds.size.width, UIScreen.mainScreen.bounds.size.height), std::max<float>(UIScreen.mainScreen.bounds.size.width, UIScreen.mainScreen.bounds.size.height));
            }
    
    float scale = [UIScreen mainScreen].scale;
    
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    director->getOpenGLView()->setFrameSize(s.width*scale, s.height*scale);
    director->getOpenGLView()->setDesignResolutionSize(s.width*scale, s.height*scale, kResolutionShowAll);
    cocos2d::CCApplication::sharedApplication()->applicationWillChangeOrientation();*/
}

// For ios6, use supportedInterfaceOrientations & shouldAutorotate instead
- (NSUInteger) supportedInterfaceOrientations{
#ifdef __IPHONE_6_0
    return UIInterfaceOrientationMaskAll;
#endif
}

- (BOOL)prefersStatusBarHidden { return YES; }

- (BOOL) shouldAutorotate {
    return YES;
}

- (void)didReceiveMemoryWarning {
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end
