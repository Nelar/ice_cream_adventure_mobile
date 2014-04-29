//
//  cFacebook.cpp
//  IceCreamAdventure
//
//  Created by Nelar on 01.11.13.
//
//

#import <FacebookSDK/FacebookSDK.h>
#import "ImageFetcher.h"

#include "cFacebook.h"
#include "cGlobal.h"
#include "cocos2d.h"
#include "MainMenuScene.h"
#include "SBJSON/SBJson.h"
#include "Options.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "NotificationTypes.h"
#include <string>
#include "nMMP.h"

#include "GameMapLayer.h"
#include "GameScene.h"
#include "cGlobal.h"
#include <Parse/Parse.h>

using namespace std;
static const unsigned long long kuFBAppID = 602727043099486;
string fbAppId = "602727043099486";

const int maxScoreUser = 1000;

cFacebook::cFacebook()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    workingDirectory = string([documentsDirectory UTF8String]);
    scoresSize = 0;
    for (int i = 0; i < 105; i++)
        myScores.push_back(0);
}

void cFacebook::login()
{
    if(sessionIsOpened())
    {
        if(!loggedInSuccessfully)
        {
            //that flag is used to call onLogin callback only once
            loggedInSuccessfully = true;
            if (GlobalsPtr->iceCreamScene == Menu)
            {
                MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                layer->facebookButtonHide();
                OptionsPtr->setFacebookConnection(true);
                OptionsPtr->save();
            }
        }
        getScores();
        return;
    }
        NSArray *permissions = [[NSArray alloc] initWithObjects:
                                @"email",
                                @"publish_actions",
                                nil];
        
        [PFFacebookUtils logInWithPermissions:permissions block:^(PFUser *user, NSError *error) {
            if (!user) {
                NSLog(@"Uh oh. The user cancelled the Facebook login.");
                if (GlobalsPtr->iceCreamScene == Menu)
                {
                    MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                    layer->closeLoading();
                }
            } else if (user.isNew) {
                isLogin = true;
                NSLog(@"User signed up and logged in through Facebook!");
                [FBRequestConnection startForMeWithCompletionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
                    if (!error) {
                        // Store the current user's Facebook ID on the user
                        [[PFUser currentUser] setObject:[result objectForKey:@"id"]
                                                 forKey:@"fbId"];
                        [[PFUser currentUser] saveInBackground];
                        
                        PFInstallation *installation = [PFInstallation currentInstallation];
                        [installation setObject:[result objectForKey:@"id"] forKey:@"fbId"];
                        [installation saveInBackground];
                    }
                }];
                if (![PFFacebookUtils isLinkedWithUser:user]) {
                    [PFFacebookUtils linkUser:user permissions:nil block:^(BOOL succeeded, NSError *error) {
                        if (succeeded) {
                            NSLog(@"Woohoo, user logged in with Facebook!");
                        }
                    }];
                }
                
                [[PFUser currentUser] saveInBackground];
                
                getScores();
                
                OptionsPtr->setFacebookConnection(true);
                OptionsPtr->save();
                
                if (GlobalsPtr->iceCreamScene == Menu)
                {
                    MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                    layer->facebookButtonHide();
                    OptionsPtr->setFacebookConnection(true);
                    OptionsPtr->save();
                }

            } else {
                isLogin = true;
                [FBRequestConnection startForMeWithCompletionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
                    if (!error) {
                        // Store the current user's Facebook ID on the user
                        [[PFUser currentUser] setObject:[result objectForKey:@"id"]
                                                 forKey:@"fbId"];
                        [[PFUser currentUser] saveInBackground];
                        
                        PFInstallation *installation = [PFInstallation currentInstallation];
                        [installation setObject:[result objectForKey:@"id"] forKey:@"fbId"];
                        [installation saveInBackground];
                    }
                }];
                
                if (![PFFacebookUtils isLinkedWithUser:user]) {
                    [PFFacebookUtils linkUser:user permissions:nil block:^(BOOL succeeded, NSError *error) {
                        if (succeeded) {
                            NSLog(@"Woohoo, user logged in with Facebook!");
                        }
                    }];
                }
                
                [[PFUser currentUser] saveInBackground];
                
                getScores();
                OptionsPtr->setFacebookConnection(true);
                OptionsPtr->save();
                
                if (GlobalsPtr->iceCreamScene == Menu)
                {
                    MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                    layer->facebookButtonHide();
                    OptionsPtr->setFacebookConnection(true);
                    OptionsPtr->save();
                }
            }
        }];
}

void cFacebook::logout()
{
    [FBSession.activeSession closeAndClearTokenInformation];
    [[PFFacebookUtils session] closeAndClearTokenInformation];
    friendsScores.clear();
    messages.clear();
    myScores.clear();
}

void cFacebook::loginWithInvite()
{
    if(sessionIsOpened())
    {
        if(!loggedInSuccessfully)
        {
            //that flag is used to call onLogin callback only once
            loggedInSuccessfully = true;
            if (GlobalsPtr->iceCreamScene == Menu)
            {
                MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                layer->facebookButtonHide();
                OptionsPtr->setFacebookConnection(true);
                OptionsPtr->save();
            }
        }
        return;
    }
    NSArray *permissions = [[NSArray alloc] initWithObjects:
                            @"email",
                            @"publish_actions",
                            nil];
    
    [PFFacebookUtils logInWithPermissions:permissions block:^(PFUser *user, NSError *error) {
        if (!user) {
            NSLog(@"Uh oh. The user cancelled the Facebook login.");
            if (GlobalsPtr->iceCreamScene == Menu)
            {
                MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                layer->closeLoading();
            }
        } else if (user.isNew) {
            isLogin = true;
            NSLog(@"User signed up and logged in through Facebook!");
            [FBRequestConnection startForMeWithCompletionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
                if (!error) {
                    // Store the current user's Facebook ID on the user
                    [[PFUser currentUser] setObject:[result objectForKey:@"id"]
                                             forKey:@"fbId"];
                    [[PFUser currentUser] saveInBackground];
                    
                    PFInstallation *installation = [PFInstallation currentInstallation];
                    [installation setObject:[result objectForKey:@"id"] forKey:@"fbId"];
                    [installation saveInBackground];
                }
            }];
            if (![PFFacebookUtils isLinkedWithUser:user]) {
                [PFFacebookUtils linkUser:user permissions:nil block:^(BOOL succeeded, NSError *error) {
                    if (succeeded) {
                        NSLog(@"Woohoo, user logged in with Facebook!");
                    }
                }];
            }
            
            [[PFUser currentUser] saveInBackground];
            
            getScores();
            OptionsPtr->setFacebookConnection(true);
            OptionsPtr->save();
            
            inviteFriends();
            
            if (GlobalsPtr->iceCreamScene == Menu)
            {
                MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                layer->facebookButtonHide();
                OptionsPtr->setFacebookConnection(true);
                OptionsPtr->save();
            }
            
        } else {
            isLogin = true;
            [FBRequestConnection startForMeWithCompletionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
                if (!error) {
                    // Store the current user's Facebook ID on the user
                    [[PFUser currentUser] setObject:[result objectForKey:@"id"]
                                             forKey:@"fbId"];
                    [[PFUser currentUser] saveInBackground];
                    
                    PFInstallation *installation = [PFInstallation currentInstallation];
                    [installation setObject:[result objectForKey:@"id"] forKey:@"fbId"];
                    [installation saveInBackground];
                }
            }];
            
            if (![PFFacebookUtils isLinkedWithUser:user]) {
                [PFFacebookUtils linkUser:user permissions:nil block:^(BOOL succeeded, NSError *error) {
                    if (succeeded) {
                        NSLog(@"Woohoo, user logged in with Facebook!");
                    }
                }];
            }
            
            [[PFUser currentUser] saveInBackground];
            
            getScores();
            OptionsPtr->setFacebookConnection(true);
            OptionsPtr->save();
            
            inviteFriends();
            
            if (GlobalsPtr->iceCreamScene == Menu)
            {
                MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                layer->facebookButtonHide();
                OptionsPtr->setFacebookConnection(true);
                OptionsPtr->save();
            }
        }
    }];
}

bool cFacebook::sessionIsOpened()
{
    if ([FBSession activeSession].isOpen == YES)
        return true;
    else
        return false;
        
    return [FBSession activeSession].isOpen;
}

void cFacebook::createFBSession()
{
}

void cFacebook::getUserData()
{
    [[FBRequest requestForMe]
     startWithCompletionHandler:
     ^(FBRequestConnection *connection, NSDictionary<FBGraphUser> *result, NSError *error)
     {

     if (!error && result)
     {
        fbid = [result.id longLongValue];
        name = [[[NSString alloc] initWithString:result.first_name] UTF8String];
        createAvatarFromFBID(fbid);
        string name = [result.first_name UTF8String];
        string idStr = [result.id UTF8String];
        map<string, string> userData;
        userData.insert(pair<string, string>("name", name));
        userData.insert(pair<string, string>("facebook", idStr));
        MMPPtr->facebookConnected(idStr, name);
        MMPPtr->userIdentified(idStr);
     }
     }];
}

string& cFacebook::getAvatar()
{
    return avatarFileName;
}

bool cFacebook::createAvatarFromFBID(unsigned long long uID)
{
    if (!sessionIsOpened())
        return false;
    NSString *resourceAddress = [[NSString alloc] initWithFormat:@"https://graph.facebook.com/%llu/picture?width=256&height=256", uID];
    
    ImageFetcher* fetcher = [[ImageFetcher alloc] init];
    [fetcher fetchImageWithUrl:resourceAddress andCompletionBlock:^(UIImage *image)
     {
     if (image == nil) {
        return false;
     }
     NSData *fileData = UIImagePNGRepresentation(image);
     NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
     NSString *documentsDirectory = [paths objectAtIndex:0];
     avatarFileName = string([documentsDirectory UTF8String]) + "/" + "avatar.png";
     BOOL didWrite = [fileData writeToFile:[NSString stringWithCString:avatarFileName.c_str() encoding:[NSString defaultCStringEncoding]] atomically:YES];
     return true;
    }];
    return false;
}

string& cFacebook::getWorkDirectory()
{
    return workingDirectory;
}

bool cFacebook::createAvatarFromFBIDForFriend(string fuid)
{
    if (!sessionIsOpened())
        return false;
    
    NSString *resourceAddress = [[NSString alloc] initWithFormat:@"https://graph.facebook.com/%@/picture?width=256&height=256", [NSString stringWithUTF8String:fuid.c_str()]];
    ImageFetcher* fetcher = [[ImageFetcher alloc] init];
    [fetcher fetchImageWithUrl:resourceAddress andCompletionBlock:^(UIImage *image)
     {
     if (image == nil) {
     return false;
     }
     NSData *fileData = UIImagePNGRepresentation(image);
     NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
     NSString *documentsDirectory = [paths objectAtIndex:0];
     
     char buf[255];
     sprintf(buf, "%s.png", fuid.c_str());
     string fileName = string([documentsDirectory UTF8String]) + "/" + buf;
     BOOL didWrite = [fileData writeToFile:[NSString stringWithCString:fileName.c_str() encoding:[NSString defaultCStringEncoding]] atomically:YES];
     return true;
     }];
    return false;
}

void cFacebook::requestWritePermissions()
{
    return;
    if (!sessionIsOpened())
        return;
    // We need to request write permissions from Facebook
    if (!bHaveRequestedPublishPermissions)
    {
        NSArray *permissions = [[NSArray alloc] initWithObjects:
                                @"publish_actions", nil];
        
        [[FBSession activeSession] requestNewPublishPermissions:permissions defaultAudience:FBSessionDefaultAudienceFriends completionHandler:^(FBSession *session, NSError *error)
         {
            NSLog(@"Reauthorized with publish permissions.");
            if (error)
            {
                NSLog(@"Error: %@", error);
            }
            else
            {
                NSLog(@"NotError: %@", error);
            }
         }];
        
        bHaveRequestedPublishPermissions = true;
    }
}

void cFacebook::sendScore(const int nScore)
{
    if (!sessionIsOpened())
        return;
    // Make sure we have write permissions
    requestWritePermissions();
    
    NSMutableDictionary* params =   [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                     [NSString stringWithFormat:@"%d", nScore], @"score",
                                     nil];
    
    NSLog(@"Fetching current score");
    
    // Get the score, and only send the updated score if it's highter
    [FBRequestConnection startWithGraphPath:[NSString stringWithFormat:@"%llu/scores?fields=score,user", fbid] parameters:params HTTPMethod:@"GET" completionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
     
     if (result && !error)
     {
        int nCurrentScore = [[[[result objectForKey:@"data"] objectAtIndex:0] objectForKey:@"score"] intValue];
        NSLog(@"Current score is %d", nCurrentScore);
     
        if (nScore > nCurrentScore) {
            NSLog(@"Posting new score of %d", nScore);
            [FBRequestConnection startWithGraphPath:[NSString stringWithFormat:@"%llu/scores?fields=score,user", fbid] parameters:params HTTPMethod:@"POST" completionHandler:^(FBRequestConnection *connection, id result, NSError *error)
             {
                NSLog(@"Score posted");
             }
            ];
        }
        else
        {
            NSLog(@"Existing score is higher - not posting new score");
        }
     }
     else
     {
            NSLog(@"Error: %@", error);
            int a = 0;
     }
     }];
    
    // Send our custom OG
//    sendOG();
}

void cFacebook::completeLevel(const int nScore)
{
    if (!sessionIsOpened())
        return;
    NSString* level = [NSString stringWithFormat:@"Level %d", nScore];
    NSMutableDictionary<FBGraphObject> *action = [FBGraphObject graphObject];
    action[@"level"] = [FBGraphObject openGraphObjectForPostWithType:@"icecreamadventure:level"
                                                               title:level
                                                               image:@"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png"
                                                                 url:[NSString stringWithUTF8String:ITUNES_LINK]
                                                         description:@""];
    
    [FBRequestConnection startForPostWithGraphPath:@"me/icecreamadventure:complete"
                                       graphObject:action
                                 completionHandler:^(FBRequestConnection *connection,
                                                     id result,
                                                     NSError *error) {
                                     MMPPtr->contentShared();
     }];
}

void cFacebook::boughtItem(string item)
{
    if (!sessionIsOpened())
        return;
    requestWritePermissions();
    NSMutableDictionary<FBGraphObject> *action = [FBGraphObject graphObject];
    action[@"item"] = [FBGraphObject openGraphObjectForPostWithType:@"icecreamadventure:item"
                                            title:[NSString stringWithUTF8String:item.c_str()]
                                            image:@"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png"
                                              url:[NSString stringWithUTF8String:ITUNES_LINK]
                                      description:@""];;
    
    [FBRequestConnection startForPostWithGraphPath:@"me/icecreamadventure:bougth"
                                       graphObject:action
                                 completionHandler:^(FBRequestConnection *connection,
                                                     id result,
                                                     NSError *error) {
                                     MMPPtr->contentShared();
     }];
}

void cFacebook::completeStage(string stage)
{
    if (!sessionIsOpened())
        return;
    requestWritePermissions();
    NSMutableDictionary<FBGraphObject> *action = [FBGraphObject graphObject];
    action[@"stage"] = [FBGraphObject openGraphObjectForPostWithType:@"icecreamadventure:stage"
                                                               title:[NSString stringWithUTF8String:stage.c_str()]
                                                               image:@"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png"
                                                                 url:[NSString stringWithUTF8String:ITUNES_LINK]
                                                         description:@""];;
    
    [FBRequestConnection startForPostWithGraphPath:@"me/icecreamadventure:complete"
                                       graphObject:action
                                 completionHandler:^(FBRequestConnection *connection,
                                                     id result,
                                                     NSError *error) {
                                     MMPPtr->contentShared();
     }];
}

void cFacebook::completeTutorial(string item)
{
    if (!sessionIsOpened())
        return;
    requestWritePermissions();
    NSMutableDictionary<FBGraphObject> *action = [FBGraphObject graphObject];
    action[@"tutorial"] = [FBGraphObject openGraphObjectForPostWithType:@"icecreamadventure:tutorial"
                                                                  title:[NSString stringWithUTF8String:item.c_str()]
                                                                  image:@"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png"
                                                                    url:[NSString stringWithUTF8String:ITUNES_LINK]
                                                            description:@""];;
    
    [FBRequestConnection startForPostWithGraphPath:@"me/icecreamadventure:complete"
                                       graphObject:action
                                 completionHandler:^(FBRequestConnection *connection,
                                                     id result,
                                                     NSError *error) {
                                     MMPPtr->contentShared();
     }];
}

void cFacebook::setPointsInLevel(int points, int level)
{
    if (!sessionIsOpened())
        return;

    if (myScores[level] >= points)
        return;
    else
        myScores[level] = points;
    
    requestWritePermissions();
    
    char pointsBuf[64];
    sprintf(pointsBuf, "%d in %d", points, level);
    
    NSLog(@"activeSession isOpen: %d", [[FBSession activeSession] isOpen]);
    NSLog(@"session perms: %@", [[FBSession activeSession] permissions]);
    
    NSMutableDictionary<FBGraphObject> *action = [FBGraphObject graphObject];
    action[@"points"] = [FBGraphObject openGraphObjectForPostWithType:@"icecreamadventure:points"
                                                                title:[NSString stringWithUTF8String:pointsBuf]
                                                                image:@"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png"
                                                                  url:[NSString stringWithUTF8String:ITUNES_LINK]
                                                          description:@""];
    
    NSString* graphPath = [NSString stringWithFormat:@"%llu/icecreamadventure:got", fbid];
    
    [FBRequestConnection startForPostWithGraphPath:@"me/icecreamadventure:got"
                                       graphObject:action
                                 completionHandler:^(FBRequestConnection *connection,
                                                     id result,
                                                     NSError *error)
     {
        if (error)
        {
            NSLog(@"ERROR:%@    %@     %@", [error localizedDescription], [error localizedFailureReason], [error localizedRecoveryOptions]);
        }
        else
        {
            NSLog(@"GOT POINTS");
        }
     }];
}

void cFacebook::requestHelp(string name)
{
    NSMutableDictionary<FBGraphObject> *action = [FBGraphObject graphObject];
    action[@"help"] = [FBGraphObject openGraphObjectForPostWithType:@"icecreamadventure:help"
                                            title:[NSString stringWithUTF8String:name.c_str()]
                                            image:@"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png"
                                              url:[NSString stringWithUTF8String:ITUNES_LINK]
                                      description:@""];
    
    [FBRequestConnection startForPostWithGraphPath:@"me/icecreamadventure:request"
                                       graphObject:action
                                 completionHandler:^(FBRequestConnection *connection,
                                                     id result,
                                                     NSError *error) {
                                     if (error)
                                     {
                                         NSLog(@"ERROR");
                                     }
                                     else
                                     {
                                         NSLog(@"GOT POINTS");
                                     }
                                 }];
}

void cFacebook::getScores()
{
    if (!sessionIsOpened())
        return;
    
    friendsScores.clear();
    
    isLoad = false;
    [FBRequestConnection startWithGraphPath:@"me/objects/icecreamadventure:points"
                          completionHandler:^(FBRequestConnection *connection,
                                              id result,
                                              NSError *error)
    {
                              if (error)
                              {
                                  NSLog(@"ERROR");
                              }
                              else
                              {
                                  NSMutableDictionary<FBGraphObject> *action  = result;
                                  NSMutableDictionary* data = [action objectForKey:@"data"];
                                  NSString* str = [data JSONRepresentation];
                                  
                                  rapidjson::Document d;
                                  d.Parse<0>([str UTF8String]);
                                  for (rapidjson::SizeType i = 0; i < d.Size(); i++)
                                  {
                                      rapidjson::Value& json = d[i];
                                      const char* title = json["title"].GetString();
                                      string str = title;
                                      string score = str.substr(0, str.find_first_of(" "));
                                      string level = str.substr(str.find_last_of(" "), str.size());
                                      myScores[atoi(level.c_str())] = atoi(score.c_str());
                                      NSLog(@"NO ERROR");
                                  }
                                  NSLog(@"NO ERROR");
                              }
    }
     ];
    
    getUserData();    
    getFriendScores();
}

void cFacebook::checkNotification()
{
    if (!sessionIsOpened())
        return;
    
    [[FBRequest requestForGraphPath:@"me?fields=apprequests"]
     startWithCompletionHandler:
     ^(FBRequestConnection *connection,
       NSDictionary *result,
       NSError *error)
     {
         // If we received a result with no errors...
         if(error)
         {
             NSLog(@"%@", error);
             return;
         }
         else
         {
             NSMutableDictionary *resultData = [result objectForKey:@"apprequests"];
             NSMutableDictionary *resultData2 = [resultData objectForKey:@"data"];
             NSString* str = [resultData2 JSONRepresentation];
             
             if (str == nil)
                 return;
             
             rapidjson::Document d;
             d.Parse<0>([str UTF8String]);
             for (rapidjson::SizeType i = 0; i < d.Size(); i++)
             {
                 rapidjson::Value& json = d[i];
                 if (string(json["application"]["id"].GetString()) == fbAppId)
                 {
                     sRequestData request;
                     request.id = json["id"].GetString();
                     request.from = json["from"]["id"].GetString();
                     request.fromName = json["from"]["name"].GetString();
                     request.from = json["from"]["id"].GetString();
                     
                     string notifType = json["data"].GetString();
                     
                     if (notifType.find("'0'") != string::npos)
                        request.type = HELP_ME;
                     else if (notifType.find("'1'") != string::npos)
                        request.type = HELPED_YOU;
                     else if (notifType.find("'2'") != string::npos)
                         request.type = INVITED_YOU;
                     else if (notifType.find("'3'") != string::npos)
                         request.type = ACCEPTED_YOUR_INVITE;
                     
                     bool flagReq = true;
                     for (int n = 0; n < OptionsPtr->appRequests.size(); n++)
                     {
                         if (OptionsPtr->appRequests[n].id == request.id)
                             flagReq = false;
                     }
                     
                     if (flagReq)
                         OptionsPtr->appRequests.push_back(request);
                 }
             }
         }
     }];
}

void cFacebook::getFriendScores()
{
    if (!sessionIsOpened())
        return;
    [[FBRequest requestForGraphPath:@"me/friends?fields=name,devices,installed"]
     startWithCompletionHandler:
     ^(FBRequestConnection *connection,
       NSDictionary *result,
       NSError *error)
     {
         // If we received a result with no errors...
         if(error)
         {
             NSLog(@"%@", error);
             return;
         }
         else
         {
            NSMutableDictionary *resultData = [result objectForKey:@"data"];
            NSString* str = [resultData JSONRepresentation];
            rapidjson::Document d;
            d.Parse<0>([str UTF8String]);
             
            if (d.Size() == 0)
            {
                isLoad = true;
            }
            for (rapidjson::SizeType i = 0; i < d.Size(); i++)
            {
                rapidjson::Value& json = d[i];
                if (!json["installed"].IsNull())
                {
                    if (json["installed"].GetBool())
                    {
                        const char* title = json["id"].GetString();
                        const char* name = json["name"].GetString();
                        getFriendScore(title, name);
                    }
                }
                NSLog(@"NO ERROR");
            }
            if (GlobalsPtr->isLoadMap)
            {
                if (GlobalsPtr->iceCreamScene == Map)
                {
                    GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                    layer->updateFacebook();
                }
                else if (GlobalsPtr->iceCreamScene == Game)
                {
                    GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                    layer->afterGetScores();
                }
                else if (GlobalsPtr->iceCreamScene == MainMenu)
                {
                    MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                    layer->playWithLoading(NULL);
                }
                GlobalsPtr->isLoadMap = false;
            }
            else
            {
                if (GlobalsPtr->iceCreamScene == Map)
                {
                    GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                    layer->updateFacebook();
                }
                else if (GlobalsPtr->iceCreamScene == Game)
                {
                    GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                    layer->updateFacebook();
                }
                else if (GlobalsPtr->iceCreamScene == MainMenu)
                {
                    MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                    layer->closeLoading();
                }
            }

            NSLog(@"NO ERROR");
         }
     }];
}

void cFacebook::getFriendScore(const char *uid, const char* name)
{
    if (!sessionIsOpened())
        return;
    
    string path = string("/") + uid + string("/icecreamadventure:got");
    [FBRequestConnection startWithGraphPath:[NSString stringWithFormat:[NSString stringWithUTF8String:path.c_str()]] parameters:nil HTTPMethod:@"GET" completionHandler:^(FBRequestConnection *connection, id result, NSError *error)
     {
         if (error)
         {
             NSLog(@"ERROR");
         }
         else
         {
             NSMutableDictionary<FBGraphObject> *action  = result;
             NSMutableDictionary* data = [action objectForKey:@"data"];
             NSString* str = [data JSONRepresentation];
             
             rapidjson::Document d;
             d.Parse<0>([str UTF8String]);
             
             if (!d.Size())
             {
                 isLoad = true;
                 return;
             }
             
             rapidjson::SizeType idx = 0;
             
             Leaderboard scoreFriend;
             scoreFriend.name = d[idx]["from"]["name"].GetString();
             scoreFriend.uid = d[idx]["from"]["id"].GetString();
             
             createAvatarFromFBIDForFriend(scoreFriend.uid);

             for (int i = 0; i < 105; i++)
                 scoreFriend.scores.push_back(0);

             for (rapidjson::SizeType i = 0; i < d.Size(); i++)
             {
                 rapidjson::Value& json = d[i];
                 const char* title = json["data"]["points"]["title"].GetString();
                 string str = title;
                 string score = str.substr(0, str.find_first_of(" "));
                 string level = str.substr(str.find_last_of(" "), str.size());
                 scoreFriend.scores[atoi(level.c_str()) - 1] = atoi(score.c_str());
                 NSLog(@"NO ERROR");
                 isLoad = true;
             }
             friendsScores.push_back(scoreFriend);
         }
     }
     ];

}

void cFacebook::unlockNewLevel(int level)
{
    if (!sessionIsOpened())
        return;
    requestWritePermissions();
    char levelBuf[64];
    sprintf(levelBuf, "%d", level);
    
    NSMutableDictionary<FBGraphObject> *action = [FBGraphObject graphObject];
    action[@"new_level"] = [FBGraphObject openGraphObjectForPostWithType:@"icecreamadventure:new_level"
                                                                   title:[NSString stringWithUTF8String:levelBuf]
                                                                   image:@"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png"
                                                                     url:[NSString stringWithUTF8String:ITUNES_LINK]
                                                             description:@""];;
    
    [FBRequestConnection startForPostWithGraphPath:@"me/icecreamadventure:unlocked"
                                       graphObject:action
                                 completionHandler:^(FBRequestConnection *connection,
                                                     id result,
                                                     NSError *error) {
                                     MMPPtr->contentShared();
     }];
}

void cFacebook::sendOG()
{
    if (!sessionIsOpened())
        return;
    FBRequest* newAction = [[FBRequest alloc]initForPostWithSession:[FBSession activeSession] graphPath:[NSString stringWithUTF8String:ITUNES_LINK] graphObject:nil];
    
    FBRequestConnection* conn = [[FBRequestConnection alloc] init];
    
    [conn addRequest:newAction completionHandler:^(FBRequestConnection *connection, id result, NSError *error) {
     if(error) {
        NSLog(@"Sending OG Story Failed: %@", result[@"id"]);
     return;
     }
     
     NSLog(@"OG action ID: %@", result[@"id"]);
     }];
    [conn start];
}

void cFacebook::sendLife(string fromId)
{
    PFQuery *pushQuery = [PFInstallation query];
    [pushQuery whereKey:@"fbId" equalTo:[NSString stringWithUTF8String:fromId.c_str()]];
    PFPush *push = [[PFPush alloc] init];
    [push setQuery:pushQuery];
    
    string message = name + ": " + "I have life for you";
    
    NSDictionary *data = @{
                           @"alert": [NSString stringWithUTF8String:message.c_str()],
                           @"notif": @"1",
                           @"fb":[NSString stringWithUTF8String:fromId.c_str()]
                           };
    [push setData:data];
    [push sendPushInBackground];
    
    PFObject *exchanger = [PFObject objectWithClassName:@"Exchanger"];
    exchanger[@"fbId"] = [NSString stringWithUTF8String:fromId.c_str()];
    exchanger[@"senderFbId"] = [NSString stringWithFormat:@"%llu", FacebookPtr->fbid];
    exchanger[@"message"] = @"1";
    [exchanger saveInBackground];
}

void cFacebook::onNotificationRecieved(const char * fromName, const char * fromId, const char * jsonData){
    cout << "RECIEVED FB NOTIFICATION" <<endl;
    
    rapidjson::Document d;
    d.Parse<0>(jsonData);
    
    NotificationType nt = (NotificationType)atoi(d["notification_type"].GetString());
    
    switch (nt) {
        case HELP_ME:
        {
            vector<string> to;
            to.push_back(string(fromId));
            
            rapidjson::GenericStringBuffer<rapidjson::UTF8<> > buffer;
            rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<> > > writer(buffer);
            
            writer.StartObject();
            writer.String("notification_type");
            writer.String(to_string((int)HELPED_YOU).c_str());
            writer.EndObject();
            
            string data = string("{") + string("'notification_type':")+ string("'1'}");
            
            sendRequest("Here are some life for you!", "Help for a friend", data, to);
        }
        break;
            
        case HELPED_YOU:
        {
            //get some coins for being helped
            isLife = true;
        }
        break;
            
        case INVITED_YOU:
        {
            vector<string> to;
            to.push_back(string(fromId));
            
            rapidjson::GenericStringBuffer<rapidjson::UTF8<> > buffer;
            rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<> > > writer(buffer);
            
            writer.StartObject();
            writer.String("notification_type");
            writer.String(to_string((int)ACCEPTED_YOUR_INVITE).c_str());
            writer.EndObject();
            
            string data = string("{") + string("'notification_type':")+ string("'3'}");
            
            sendRequest("Here are some boosters for you!", "Thanks for inviting me", data, to);
            
            //get some coins for being invited
        }
            break;
            
        case ACCEPTED_YOUR_INVITE:
        {
            //get 250 coins for inviting a friend
            isBooster = true;
        }
        break;
    }
};

void cFacebook::processIncomingURL(char *targetURL)
{
    NSURL * url = [NSURL URLWithString:[NSString stringWithUTF8String:targetURL]];
    
    NSRange range = [url.query rangeOfString:@"notif" options:NSCaseInsensitiveSearch];
    OptionsPtr->setCrystalCount(100);
    OptionsPtr->save();
    // If the url's query contains 'notif', we know it's coming from a notification - let's process it
    if(url.query && range.location != NSNotFound)
    {
        processIncomingRequest(targetURL);
    }
}

void cFacebook::processIncomingRequest(char *targetURL)
{
    NSURL * url = [NSURL URLWithString:[NSString stringWithUTF8String:targetURL]];
    
    // Extract the notification id
    NSArray *pairs = [url.query componentsSeparatedByString:@"&"];
    NSMutableDictionary *queryParams = [[NSMutableDictionary alloc] init];
    for (NSString *pair in pairs)
    {
        NSArray *kv = [pair componentsSeparatedByString:@"="];
        NSString *val = [[kv objectAtIndex:1]
                         stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        
        [queryParams setObject:val forKey:[kv objectAtIndex:0]];
        
    }
    
    NSString *requestIDsString = [queryParams objectForKey:@"request_ids"];
    NSArray *requestIDs = [requestIDsString componentsSeparatedByString:@","];
    
    //get the request data from fb
    FBRequest *req = [[FBRequest alloc] initWithSession:[FBSession activeSession] graphPath:[requestIDs objectAtIndex:0]];
    
    [req startWithCompletionHandler:^(FBRequestConnection *connection, id result, NSError *error)
     {
         if (!error)
         {
             if ([result objectForKey:@"from"])
             {
                 NSString *fromName = [[result objectForKey:@"from"] objectForKey:@"name"];
                 NSString *fromId = [[result objectForKey:@"from"] objectForKey:@"id"];
                 
                 if([result objectForKey:@"data"]){
                     NSString * data = [result objectForKey:@"data"];
                     //yay we have our json data
                     
                     onNotificationRecieved( [fromName UTF8String], [fromId UTF8String] , [data UTF8String]);
                 }
                 
             }
             
         }
         
     }];
}

void cFacebook::inviteFriends()
{
    if (!sessionIsOpened())
        return;
    
    NSMutableArray * nonUserFriends = [[NSMutableArray alloc]init];
    
    [[FBRequest requestForGraphPath:@"me/friends?fields=name,installed"]
     startWithCompletionHandler:
     ^(FBRequestConnection *connection,
       NSDictionary *result,
       NSError *error)
     {
         // If we received a result with no errors...
         if (!error && result)
         {
             NSArray *resultData = [result objectForKey:@"data"];
             
             if ([resultData count] > 0)
             {
                 for (NSDictionary *friendObject in resultData)
                 {
                     if (![friendObject objectForKey:@"installed"])
                     {
                         [nonUserFriends addObject:[friendObject objectForKey:@"id"]];
                     }
                 }
             }
             
             NSMutableDictionary* params = [NSMutableDictionary dictionaryWithObjectsAndKeys:nil];
             
             if ([nonUserFriends count] > 0)
             {
                 // We add them to the suggest friend param of the dialog
                 NSString *selectIDsStr = [nonUserFriends componentsJoinedByString:@","];
                 [params setObject:selectIDsStr forKey:@"suggestions"];
             }
             
             rapidjson::GenericStringBuffer<rapidjson::UTF8<> > buffer;
             rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<> > > writer(buffer);
             
             writer.StartObject();
             writer.String("notification_type");
             writer.String(to_string((int)INVITED_YOU).c_str());
             writer.EndObject();
             
             string dataStr = string("{") + string("'notification_type':")+ string("'2'}");
             
             [params setObject:[NSString stringWithUTF8String:dataStr.c_str()] forKey:@"data"];
             
             
             FBFrictionlessRecipientCache *friendCache = [[FBFrictionlessRecipientCache alloc] init];
             [friendCache prefetchAndCacheForSession:nil];
             
             [FBWebDialogs presentRequestsDialogModallyWithSession:nil
                                                           message:@"I invite you to play Ice Cream Adventure. You'll get a bonus, if you install :)"
                                                             title:@"Invite friends"
                                                        parameters:params
                                                           handler:^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
                                                               
                                                               if (error) {
                                                                   // Error launching the dialog or sending the request.
                                                                   NSLog(@"Error sending request.");
                                                               } else {
                                                                   if (result == FBWebDialogResultDialogNotCompleted) {
                                                                       // User clicked the "x" icon
                                                                       NSLog(@"User canceled request.");
                                                                   } else {
                                                                       // Handle the send request callback
                                                                       NSArray *pairs = [[resultURL query] componentsSeparatedByString:@"&"];
                                                                       NSMutableDictionary *urlParams = [[NSMutableDictionary alloc] init];
                                                                       for (NSString *pair in pairs) {
                                                                           NSArray *kv = [pair componentsSeparatedByString:@"="];
                                                                           NSString *val =
                                                                           [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                                                                           urlParams[kv[0]] = val;
                                                                       }
                                                                       
                                                                       if (![urlParams valueForKey:@"request"]) {
                                                                           // User clicked the Cancel button
                                                                           NSLog(@"User canceled request.");
                                                                       } else {
                                                                           // User clicked the Send button
                                                                           NSString *requestID = [urlParams valueForKey:@"request"];
                                                                           NSLog(@"Request ID: %@", requestID);
                                                                           
                                                                           OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                                                                           OptionsPtr->save();
                                                                       }
                                                                   }
                                                               }
                                                           }
                                                       friendCache:friendCache];
             
         }
     }];
    
}

void cFacebook::sendRequest(string message, string title, string jsonData, vector<string> to, bool onlyMobileUsers)
{
    if (!sessionIsOpened())
        return;
    //no need for write permissions, as we're invoking a web dialog
    
    NSMutableArray *deviceFilteredFriends = [[NSMutableArray alloc] init];
    
    
    [[FBRequest requestForGraphPath:@"me/friends?fields=name,devices,installed"]
     startWithCompletionHandler:
     ^(FBRequestConnection *connection,
       NSDictionary *result,
       NSError *error)
     {
         // If we received a result with no errors...
         if(error)
         {
             NSLog(@"%@", error);
             return;
         }
         
         if (result)
         {
             NSArray *resultData = [result objectForKey:@"data"];
             
             if ([resultData count] > 0)
             {
                 for (NSDictionary *friendObject in resultData)
                 {
                     if ([friendObject objectForKey:@"installed"])
                     {
                        [deviceFilteredFriends addObject: [friendObject objectForKey:@"id"]];
                     }
                 }
             }
             
             NSMutableDictionary* params = [NSMutableDictionary dictionaryWithObjectsAndKeys:nil];
             
             if ([deviceFilteredFriends count] > 0)
             {
                 // We add them to the suggest friend param of the dialog
                 NSString *selectIDsStr = [deviceFilteredFriends componentsJoinedByString:@","];
                 [params setObject:selectIDsStr forKey:@"suggestions"];
                 
             }else if (to.size() > 0){
                 //if @to isn't empty, add @to instead
                 
                 string _to = "";
                 for(int i = 0; i < to.size(); ++i)
                 {
                     string userId = to[i];
                     
                     _to += userId;
                     
                     if(to.size() > 1 && i < to.size() - 1)
                     {
                         _to += ",";
                     }
                 }
                 
                 [params setObject:[NSString stringWithUTF8String:_to.c_str()] forKey:@"to"];
             }else{
                 //if no To and no friends with installed app
                 inviteFriends();
                 return;
             }
             
             //add custom data
             if(jsonData != "")
             {
                 [params setObject:[NSString stringWithUTF8String:jsonData.c_str()] forKey:@"data"];
             }
             
             FBFrictionlessRecipientCache *friendCache = [[FBFrictionlessRecipientCache alloc] init];
             [friendCache prefetchAndCacheForSession:nil];
             
             [FBWebDialogs presentRequestsDialogModallyWithSession:nil
                                                           message:[NSString stringWithUTF8String:message.c_str()]
                                                             title:[NSString stringWithUTF8String:title.c_str()]
                                                        parameters:params
                                                           handler:^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
                                                               
                                                               if (error) {
                                                                   // Error launching the dialog or sending the request.
                                                                   NSLog(@"Error sending request.");
                                                               } else {
                                                                   if (result == FBWebDialogResultDialogNotCompleted) {
                                                                       // User clicked the "x" icon
                                                                       NSLog(@"User canceled request.");
                                                                   } else {
                                                                       // Handle the send request callback
                                                                       NSArray *pairs = [[resultURL query] componentsSeparatedByString:@"&"];
                                                                       NSMutableDictionary *urlParams = [[NSMutableDictionary alloc] init];
                                                                       for (NSString *pair in pairs) {
                                                                           NSArray *kv = [pair componentsSeparatedByString:@"="];
                                                                           NSString *val =
                                                                           [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                                                                           urlParams[kv[0]] = val;
                                                                       }
                                                                       
                                                                       if (![urlParams valueForKey:@"request"]) {
                                                                           // User clicked the Cancel button
                                                                           NSLog(@"User canceled request.");
                                                                       } else {
                                                                           // User clicked the Send button
                                                                           NSString *requestID = [urlParams valueForKey:@"request"];
                                                                           NSLog(@"Request ID: %@", requestID);
                                                                       }
                                                                   }
                                                               }
                                                           }
                                                       friendCache:friendCache];
             
         }
     }];
}

void cFacebook::sendFilteredRequest(const int nScore)
{
    if (!sessionIsOpened())
        return;
    // Okay, we're going to filter our friends by their device, we're looking for friends with an iPhone or iPad
    
    // We're going to place these friends into this container
    NSMutableArray *deviceFilteredFriends = [[NSMutableArray alloc] init];
    
    // We request a list of our friends' names and devices
    [[FBRequest requestForGraphPath:@"me/friends?fields=name,devices"]
     startWithCompletionHandler:
     ^(FBRequestConnection *connection,
       NSDictionary *result,
       NSError *error)
     {
     // If we received a result with no errors...
     if (!error && result)
     {
     // Get the result
     NSArray *resultData = [result objectForKey:@"data"];
     
     // Check we have some friends. If the player doesn't have any friends, they probably need to put down the demo app anyway, and go outside...
     if ([resultData count] > 0)
     {
     // Loop through the friends returned
     for (NSDictionary *friendObject in resultData)
     {
     // Check if devices info available
     if ([friendObject objectForKey:@"devices"])
     {
     // Yep, we know what devices this friend has.. let's extract them
     NSArray *deviceData = [friendObject objectForKey:@"devices"];
     
     // Loop through the list of devices this friend has...
     for (NSDictionary *deviceObject in deviceData)
     {
     // Check if there is a device match, in this case we're looking for iOS
     if ([@"iOS" isEqualToString: [deviceObject objectForKey:@"os"]])
     {
     // If there is a match, add it to the list - this friend has an iPhone or iPad. Hurrah!
     [deviceFilteredFriends addObject: [friendObject objectForKey:@"id"]];
     break;
     }
     }
     }
     }
     }
     
     // Now we have a list of friends with an iOS device, we can send requests to them
     
     // We create our parameter dictionary as we did before
     NSMutableDictionary* params = [NSMutableDictionary dictionaryWithObjectsAndKeys:nil];
     
     // We have the same list of suggested friends
     NSArray *suggestedFriends = [[NSArray alloc] initWithObjects:
                                  @"695755709", @"685145706", @"569496010", @"7963306",
                                  nil];
     
     
     // Of course, not all of our suggested friends will have iPhones or iPads - we need to filter them down
     NSMutableArray *validSuggestedFriends = [[NSMutableArray alloc] init];
     
     // So, we loop through each suggested friend
     for (NSString* suggestedFriend in suggestedFriends)
     {
     // If they are on our device filtered list, we know they have an iOS device
     if ([deviceFilteredFriends containsObject:suggestedFriend])
     {
     // So we can call them valid
     [validSuggestedFriends addObject:suggestedFriend];
     }
     }
     
     // If at least one of our suggested friends had an iOS device...
     if ([deviceFilteredFriends count] > 0)
     {
     // We add them to the suggest friend param of the dialog
     NSString *selectIDsStr = [validSuggestedFriends componentsJoinedByString:@","];
     [params setObject:selectIDsStr forKey:@"suggestions"];
     }
     
     FBFrictionlessRecipientCache *friendCache = [[FBFrictionlessRecipientCache alloc] init];
     [friendCache prefetchAndCacheForSession:nil];
     
     [FBWebDialogs presentRequestsDialogModallyWithSession:nil
                                                   message:[NSString stringWithFormat:@"I just smashed %d friends! Can you beat it?", nScore]
                                                     title:nil
                                                parameters:params
                                                   handler:^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
      if (error) {
      NSLog(@"Error sending request.");
      } else {
      if (result == FBWebDialogResultDialogNotCompleted) {
      NSLog(@"User canceled request.");
      } else {
      NSLog(@"Request Sent.");
      }
      }}
                                               friendCache:friendCache];
     
     }
     }];
}
bool cFacebook::beginGamePost()
{
    if (!sessionIsOpened())
        return false;
    
    NSString *linkURL = [NSString stringWithUTF8String:ITUNES_LINK];
    NSString *pictureURL = @"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png";
    
    // Prepare the native share dialog parameters
    FBShareDialogParams *shareParams = [[FBShareDialogParams alloc] init];
    shareParams.link = [NSURL URLWithString:linkURL];
    shareParams.name = @"Ice Cream Adventure!";
    shareParams.caption= @"Ice Cream Adventure!";
    shareParams.picture= [NSURL URLWithString:pictureURL];
    shareParams.description = [NSString stringWithUTF8String:CCLocalizedString("BEGIN_POST", NULL)];
    
    if ([FBDialogs canPresentShareDialogWithParams:shareParams]){
        
        [FBDialogs presentShareDialogWithParams:shareParams
                                    clientState:nil
                                        handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
                                            if (error) {
                                                if ([FBErrorUtility shouldNotifyUserForError:error]) {
                                                    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Facebook-Error!" message:[FBErrorUtility userMessageForError:error] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
                                                    [alert show];
                                                }
                                            }
                                            else if (results[@"completionGesture"])
                                            {
                                                if ([results[@"completionGesture"] isEqualToString:@"cancel"])
                                                {
                                                    NSLog(@"User canceled story publishing.");
                                                } else if ([results[@"completionGesture"] isEqualToString:@"post"])
                                                {
                                                    NSLog(@"Story published.");
                                                    OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                                                    OptionsPtr->save();
                                                    if (GlobalsPtr->iceCreamScene == Game)
                                                    {
                                                        GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                                                        layer->updateBoosters();
                                                    }
                                                    MMPPtr->contentShared();
                                                }
                                            }
                                            else
                                            {
                                                NSLog(@"Usr");
                                            }
                                        }];
        
    }else {
        
        // Prepare the web dialog parameters
        NSDictionary *params = @{
                                 @"name" : shareParams.name,
                                 @"caption" : shareParams.caption,
                                 @"description" : shareParams.description,
                                 @"picture" : pictureURL,
                                 @"link" : linkURL
                                 };
        
        // Invoke the dialog
        [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                               parameters:params
                                                  handler:
         ^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
             if (error) {
                 NSLog(@"Error publishing story.");
             } else {
                 if (result == FBWebDialogResultDialogNotCompleted) {
                     NSLog(@"User canceled story publishing.");
                 } else{
                     NSArray *pairs = [[resultURL query] componentsSeparatedByString:@"&"];
                     NSMutableDictionary *urlParams = [[NSMutableDictionary alloc] init];
                     for (NSString *pair in pairs) {
                         NSArray *kv = [pair componentsSeparatedByString:@"="];
                         NSString *val =
                         [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                         urlParams[kv[0]] = val;
                     }
                     
                     if (![urlParams valueForKey:@"post_id"]) {
                         // User cancelled.
                         NSLog(@"User cancelled.");
                     } else {
                         // User clicked the Share button
                         OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                         OptionsPtr->save();
                         if (GlobalsPtr->iceCreamScene == Game)
                         {
                             GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                             layer->updateBoosters();
                         }
                         MMPPtr->contentShared();
                     }
                 }
             }
             }];
    }
}

bool cFacebook::endTutorial()
{
    if (!sessionIsOpened())
        return false;
    NSString *linkURL = [NSString stringWithUTF8String:ITUNES_LINK];
    NSString *pictureURL = @"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png";
    
    // Prepare the native share dialog parameters
    FBShareDialogParams *shareParams = [[FBShareDialogParams alloc] init];
    shareParams.link = [NSURL URLWithString:linkURL];
    shareParams.name = @"Ice Cream Adventure!";
    shareParams.caption= @"Ice Cream Adventure!";
    shareParams.picture= [NSURL URLWithString:pictureURL];
    shareParams.description = @"I graduated from the Ice Cream Adventure! Now I am waiting for a sweet adventure! Most come in the game and go along with me!";
    
    if ([FBDialogs canPresentShareDialogWithParams:shareParams]){
        
        [FBDialogs presentShareDialogWithParams:shareParams
                                    clientState:nil
                                        handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
                                            if (error) {
                                                if ([FBErrorUtility shouldNotifyUserForError:error]) {
                                                    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Facebook-Error!" message:[FBErrorUtility userMessageForError:error] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
                                                    [alert show];
                                                }
                                            }
                                            else if (results[@"completionGesture"])
                                            {
                                                if ([results[@"completionGesture"] isEqualToString:@"cancel"])
                                                {
                                                    NSLog(@"User canceled story publishing.");
                                                } else if ([results[@"completionGesture"] isEqualToString:@"post"])
                                                {
                                                    NSLog(@"Story published.");
                                                    OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                                                    OptionsPtr->save();
                                                    if (GlobalsPtr->iceCreamScene == Game)
                                                    {
                                                        GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                                                        layer->updateBoosters();
                                                    }
                                                    MMPPtr->contentShared();
                                                }
                                            }
                                            else
                                            {
                                                NSLog(@"Usr");
                                            }
                                        }];
        
    }else {
        
        // Prepare the web dialog parameters
        NSDictionary *params = @{
                                 @"name" : shareParams.name,
                                 @"caption" : shareParams.caption,
                                 @"description" : shareParams.description,
                                 @"picture" : pictureURL,
                                 @"link" : linkURL
                                 };
        
        // Invoke the dialog
        [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                               parameters:params
                                                  handler:
         ^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
             if (error) {
                 NSLog(@"Error publishing story.");
             } else {
                 if (result == FBWebDialogResultDialogNotCompleted) {
                     NSLog(@"User canceled story publishing.");
                 } else{
                     NSArray *pairs = [[resultURL query] componentsSeparatedByString:@"&"];
                     NSMutableDictionary *urlParams = [[NSMutableDictionary alloc] init];
                     for (NSString *pair in pairs) {
                         NSArray *kv = [pair componentsSeparatedByString:@"="];
                         NSString *val =
                         [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                         urlParams[kv[0]] = val;
                     }
                     
                     if (![urlParams valueForKey:@"post_id"]) {
                         // User cancelled.
                         NSLog(@"User cancelled.");
                     } else {
                         // User clicked the Share button
                         OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                         OptionsPtr->save();
                         if (GlobalsPtr->iceCreamScene == Game)
                         {
                             GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                             layer->updateBoosters();
                         }
                         MMPPtr->contentShared();
                     }
                 }
             }}];
    }
}

bool cFacebook::unlockStage(int stage)
{
    if (!sessionIsOpened())
        return false;
    
    NSString *linkURL = [NSString stringWithUTF8String:ITUNES_LINK];
    NSString *pictureURL = @"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png";
    
    // Prepare the native share dialog parameters
    FBShareDialogParams *shareParams = [[FBShareDialogParams alloc] init];
    shareParams.link = [NSURL URLWithString:linkURL];
    shareParams.name = @"Ice Cream Adventure!";
    shareParams.caption= @"Ice Cream Adventure!";
    shareParams.picture= [NSURL URLWithString:pictureURL];
    shareParams.description = [NSString stringWithUTF8String:CCLocalizedString("UNLOCK_STAGE_POST", NULL)];
    
    if ([FBDialogs canPresentShareDialogWithParams:shareParams]){
        
        [FBDialogs presentShareDialogWithParams:shareParams
                                    clientState:nil
                                        handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
                                            if (error) {
                                                if ([FBErrorUtility shouldNotifyUserForError:error]) {
                                                    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Facebook-Error!" message:[FBErrorUtility userMessageForError:error] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
                                                    [alert show];
                                                }
                                            }
                                            else if (results[@"completionGesture"])
                                            {
                                                if ([results[@"completionGesture"] isEqualToString:@"cancel"])
                                                {
                                                    NSLog(@"User canceled story publishing.");
                                                } else if ([results[@"completionGesture"] isEqualToString:@"post"])
                                                {
                                                    NSLog(@"Story published.");
                                                    OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                                                    OptionsPtr->save();
                                                    if (GlobalsPtr->iceCreamScene == Map)
                                                    {
                                                        GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                                                        layer->updateMenuBooster();
                                                    }
                                                    MMPPtr->contentShared();
                                                    char buf[255];
                                                    sprintf(buf, "unlock%d", stage);
                                                    CCUserDefault::sharedUserDefault()->setBoolForKey(buf, true);
                                                    CCUserDefault::sharedUserDefault()->flush();
                                                }
                                            }
                                            else
                                            {
                                                NSLog(@"Usr");
                                            }
                                        }];
        
    }else {
        
        // Prepare the web dialog parameters
        NSDictionary *params = @{
                                 @"name" : shareParams.name,
                                 @"caption" : shareParams.caption,
                                 @"description" : shareParams.description,
                                 @"picture" : pictureURL,
                                 @"link" : linkURL
                                 };
        
        // Invoke the dialog
        [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                               parameters:params
                                                  handler:
         ^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
             if (error) {
                 NSLog(@"Error publishing story.");
             } else {
                 if (result == FBWebDialogResultDialogNotCompleted) {
                     NSLog(@"User canceled story publishing.");
                 } else{
                     NSArray *pairs = [[resultURL query] componentsSeparatedByString:@"&"];
                     NSMutableDictionary *urlParams = [[NSMutableDictionary alloc] init];
                     for (NSString *pair in pairs) {
                         NSArray *kv = [pair componentsSeparatedByString:@"="];
                         NSString *val =
                         [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                         urlParams[kv[0]] = val;
                     }
                     
                     if (![urlParams valueForKey:@"post_id"]) {
                         // User cancelled.
                         NSLog(@"User cancelled.");
                     } else {
                         // User clicked the Share button
                         OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                         OptionsPtr->save();
                         if (GlobalsPtr->iceCreamScene == Map)
                         {
                             GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                             layer->updateMenuBooster();
                         }
                         MMPPtr->contentShared();
                         
                         char buf[255];
                         sprintf(buf, "unlock%d", stage);
                         CCUserDefault::sharedUserDefault()->setBoolForKey(buf, true);
                         CCUserDefault::sharedUserDefault()->flush();
                     }
                 }
             }}];
    }
}

bool cFacebook::jubileePost()
{
    if (!sessionIsOpened())
        return false;
    NSString *linkURL = [NSString stringWithUTF8String:ITUNES_LINK];
    NSString *pictureURL = @"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png";
    
    // Prepare the native share dialog parameters
    FBShareDialogParams *shareParams = [[FBShareDialogParams alloc] init];
    shareParams.link = [NSURL URLWithString:linkURL];
    shareParams.name = @"Ice Cream Adventure!";
    shareParams.caption= @"Ice Cream Adventure!";
    shareParams.picture= [NSURL URLWithString:pictureURL];
    shareParams.description = [NSString stringWithUTF8String:CCLocalizedString("HALF_WAY_POST", NULL)];
    
    if ([FBDialogs canPresentShareDialogWithParams:shareParams]){
        
        [FBDialogs presentShareDialogWithParams:shareParams
                                    clientState:nil
                                        handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
                                            if (error) {
                                                if ([FBErrorUtility shouldNotifyUserForError:error]) {
                                                    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Facebook-Error!" message:[FBErrorUtility userMessageForError:error] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
                                                    [alert show];
                                                }
                                            }
                                            else if (results[@"completionGesture"])
                                            {
                                                if ([results[@"completionGesture"] isEqualToString:@"cancel"])
                                                {
                                                    NSLog(@"User canceled story publishing.");
                                                } else if ([results[@"completionGesture"] isEqualToString:@"post"])
                                                {
                                                    NSLog(@"Story published.");
                                                    OptionsPtr->setJubPost();
                                                    OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                                                    OptionsPtr->save();
                                                    if (GlobalsPtr->iceCreamScene == Map)
                                                    {
                                                        GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                                                        layer->updateMenuBooster();
                                                    }
                                                    MMPPtr->contentShared();
                                                }
                                            }
                                            else
                                            {
                                                NSLog(@"Usr");
                                            }
                                        }];
        
    }else {
        
        // Prepare the web dialog parameters
        NSDictionary *params = @{
                                 @"name" : shareParams.name,
                                 @"caption" : shareParams.caption,
                                 @"description" : shareParams.description,
                                 @"picture" : pictureURL,
                                 @"link" : linkURL
                                 };
        
        // Invoke the dialog
        [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                               parameters:params
                                                  handler:
         ^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
             if (error) {
                 NSLog(@"Error publishing story.");
             } else {
                 if (result == FBWebDialogResultDialogNotCompleted) {
                     NSLog(@"User canceled story publishing.");
                 } else{
                     NSArray *pairs = [[resultURL query] componentsSeparatedByString:@"&"];
                     NSMutableDictionary *urlParams = [[NSMutableDictionary alloc] init];
                     for (NSString *pair in pairs) {
                         NSArray *kv = [pair componentsSeparatedByString:@"="];
                         NSString *val =
                         [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                         urlParams[kv[0]] = val;
                     }
                     
                     if (![urlParams valueForKey:@"post_id"]) {
                         // User cancelled.
                         NSLog(@"User cancelled.");
                     } else {
                         // User clicked the Share button
                         OptionsPtr->setJubPost();
                         OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                         OptionsPtr->save();
                         if (GlobalsPtr->iceCreamScene == Map)
                         {
                             GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                             layer->updateMenuBooster();
                         }
                         MMPPtr->contentShared();
                     }
                 }
             }}];
    }
}

bool cFacebook::endGame()
{
    if (!sessionIsOpened())
        return false;
    NSString *linkURL = [NSString stringWithUTF8String:ITUNES_LINK];
    NSString *pictureURL = @"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png";
    
    // Prepare the native share dialog parameters
    FBShareDialogParams *shareParams = [[FBShareDialogParams alloc] init];
    shareParams.link = [NSURL URLWithString:linkURL];
    shareParams.name = @"Ice Cream Adventure!";
    shareParams.caption= @"Ice Cream Adventure!";
    shareParams.picture= [NSURL URLWithString:pictureURL];
    //    string post = "I discovered the secret fairy world Ice Cream passing" + stage +"! Come, and you will find incredible!";
    shareParams.description = [NSString stringWithUTF8String:CCLocalizedString("END_STAGE_POST", NULL)];
    
    if ([FBDialogs canPresentShareDialogWithParams:shareParams]){
        
        [FBDialogs presentShareDialogWithParams:shareParams
                                    clientState:nil
                                        handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
                                            if (error) {
                                                if ([FBErrorUtility shouldNotifyUserForError:error]) {
                                                    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Facebook-Error!" message:[FBErrorUtility userMessageForError:error] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
                                                    [alert show];
                                                }
                                            }
                                            else if (results[@"completionGesture"])
                                            {
                                                if ([results[@"completionGesture"] isEqualToString:@"cancel"])
                                                {
                                                    NSLog(@"User canceled story publishing.");
                                                } else if ([results[@"completionGesture"] isEqualToString:@"post"])
                                                {
                                                    NSLog(@"Story published.");
                                                    OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                                                    OptionsPtr->save();
                                                    MMPPtr->contentShared();
                                                }
                                            }
                                            else
                                            {
                                                NSLog(@"Usr");
                                            }
                                        }];
        
    }else {
        
        // Prepare the web dialog parameters
        NSDictionary *params = @{
                                 @"name" : shareParams.name,
                                 @"caption" : shareParams.caption,
                                 @"description" : shareParams.description,
                                 @"picture" : pictureURL,
                                 @"link" : linkURL
                                 };
        
        // Invoke the dialog
        [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                               parameters:params
                                                  handler:
         ^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
             if (error) {
                 NSLog(@"Error publishing story.");
             } else {
                 if (result == FBWebDialogResultDialogNotCompleted) {
                     NSLog(@"User canceled story publishing.");
                 } else{
                     NSArray *pairs = [[resultURL query] componentsSeparatedByString:@"&"];
                     NSMutableDictionary *urlParams = [[NSMutableDictionary alloc] init];
                     for (NSString *pair in pairs) {
                         NSArray *kv = [pair componentsSeparatedByString:@"="];
                         NSString *val =
                         [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                         urlParams[kv[0]] = val;
                     }
                     
                     if (![urlParams valueForKey:@"post_id"]) {
                         // User cancelled.
                         NSLog(@"User cancelled.");
                     } else {
                         // User clicked the Share button
                         OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                         OptionsPtr->save();
                         MMPPtr->contentShared();
                     }
                 }
             }}];
    }
}

bool cFacebook::endStage(string stage)
{
    if (!sessionIsOpened())
        return false;
    NSString *linkURL = [NSString stringWithUTF8String:ITUNES_LINK];
    NSString *pictureURL = @"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png";
    
    // Prepare the native share dialog parameters
    FBShareDialogParams *shareParams = [[FBShareDialogParams alloc] init];
    shareParams.link = [NSURL URLWithString:linkURL];
    shareParams.name = @"Ice Cream Adventure!";
    shareParams.caption= @"Ice Cream Adventure!";
    shareParams.picture= [NSURL URLWithString:pictureURL];
//    string post = "I discovered the secret fairy world Ice Cream passing" + stage +"! Come, and you will find incredible!";
    shareParams.description = [NSString stringWithUTF8String:CCLocalizedString("END_STAGE_POST", NULL)];
    
    if ([FBDialogs canPresentShareDialogWithParams:shareParams]){
        
        [FBDialogs presentShareDialogWithParams:shareParams
                                    clientState:nil
                                        handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
                                            if (error) {
                                                if ([FBErrorUtility shouldNotifyUserForError:error]) {
                                                    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Facebook-Error!" message:[FBErrorUtility userMessageForError:error] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
                                                    [alert show];
                                                }
                                            }
                                            else if (results[@"completionGesture"])
                                            {
                                                if ([results[@"completionGesture"] isEqualToString:@"cancel"])
                                                {
                                                    NSLog(@"User canceled story publishing.");
                                                } else if ([results[@"completionGesture"] isEqualToString:@"post"])
                                                {
                                                    NSLog(@"Story published.");
                                                    OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                                                    OptionsPtr->save();
                                                    MMPPtr->contentShared();
                                                }
                                            }
                                            else
                                            {
                                                NSLog(@"Usr");
                                            }
                                        }];
        
    }else {
        
        // Prepare the web dialog parameters
        NSDictionary *params = @{
                                 @"name" : shareParams.name,
                                 @"caption" : shareParams.caption,
                                 @"description" : shareParams.description,
                                 @"picture" : pictureURL,
                                 @"link" : linkURL
                                 };
        
        // Invoke the dialog
        [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                               parameters:params
                                                  handler:
         ^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
             if (error) {
                 NSLog(@"Error publishing story.");
             } else {
                 if (result == FBWebDialogResultDialogNotCompleted) {
                     NSLog(@"User canceled story publishing.");
                 } else{
                     NSArray *pairs = [[resultURL query] componentsSeparatedByString:@"&"];
                     NSMutableDictionary *urlParams = [[NSMutableDictionary alloc] init];
                     for (NSString *pair in pairs) {
                         NSArray *kv = [pair componentsSeparatedByString:@"="];
                         NSString *val =
                         [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
                         urlParams[kv[0]] = val;
                     }
                     
                     if (![urlParams valueForKey:@"post_id"]) {
                         // User cancelled.
                         NSLog(@"User cancelled.");
                     } else {
                         // User clicked the Share button
                         OptionsPtr->setBombCount(OptionsPtr->getBombCount() + 1);
                         OptionsPtr->save();
                         MMPPtr->contentShared();
                     }
                 }
             }}];
    }
}

void cFacebook::acceptMessage()
{
    for (int i = 0; i < messages.size(); i++)
    {
        if (messages[i].accept)
        {
            PFQuery* query = [PFQuery queryWithClassName:@"Exchanger"];
            [query getObjectInBackgroundWithId:[NSString stringWithUTF8String:messages[i].id.c_str()] block:^(PFObject *object, NSError *error) {
                if (error)
                {
                    NSLog(@"ERROR");
                }
                else
                {
                    [object deleteInBackground];
                }
            }];
        }
    }
}

void cFacebook::checkMessages()
{
    messages.clear();
    PFQuery* query = [PFQuery queryWithClassName:@"Exchanger"];
    [query whereKey:@"fbId" equalTo:[NSString stringWithFormat:@"%llu", FacebookPtr->fbid]];
    
    [query findObjectsInBackgroundWithBlock:^(NSArray *objects, NSError *error) {
        if (error)
        {
            NSLog(@"ERROR");
        }
        else
        {
            for (int i = 0; i < objects.count; i++)
            {
                GiftMessage message;
                message.accept = false;
                PFObject* obj = [objects objectAtIndex:i];
                
                message.id = [obj.objectId UTF8String];
                message.from = [obj[@"senderFbId"] UTF8String];
                string notStr = [obj[@"message"] UTF8String];

                if (notStr == "0")
                    message.notif = HELP_ME;
                else if (notStr == "1")
                    message.notif = HELPED_YOU;
                
                for (int j = 0; j < friendsScores.size(); j++)
                    if (friendsScores[j].uid == message.from)
                        message.name = friendsScores[j].name;
                
                messages.push_back(message);
            }
            if (messages.size() > 0)
            {
                if (GlobalsPtr->iceCreamScene == Map)
                {
                    GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                    layer->showMessageBoard();
                }
            }
        }
    }];
}

void cFacebook::askLife(string fromId)
{
    PFQuery *pushQuery = [PFInstallation query];
    [pushQuery whereKey:@"fbId" equalTo:[NSString stringWithUTF8String:fromId.c_str()]];
    PFPush *push = [[PFPush alloc] init];
    [push setQuery:pushQuery];
    
    string message = name + ":" + "Help me";
    [push setMessage:[NSString stringWithUTF8String:message.c_str()]];
    [push sendPushInBackground];
    
    string nameForpost;
    
    for (int i = 0; i < friendsScores.size(); i++)
        if (friendsScores[i].uid == fromId)
            requestHelp(friendsScores[i].name);
    
    PFObject *exchanger = [PFObject objectWithClassName:@"Exchanger"];
    exchanger[@"fbId"] = [NSString stringWithUTF8String:fromId.c_str()];
    exchanger[@"senderFbId"] = [NSString stringWithFormat:@"%llu", FacebookPtr->fbid];
    exchanger[@"message"] = @"0";
    [exchanger saveInBackground];
}

void cFacebook::sendBrag(const int nScore)
{
    if (!sessionIsOpened())
        return;
    NSString *linkURL = [NSString stringWithUTF8String:ITUNES_LINK];
    NSString *pictureURL = @"http://i58.fastpic.ru/big/2014/0313/75/917b3a214cddffd909048872a47e5c75.png";
    
    // Prepare the native share dialog parameters
    FBShareDialogParams *shareParams = [[FBShareDialogParams alloc] init];
    shareParams.link = [NSURL URLWithString:linkURL];
    shareParams.name = @"Checkout my Friend Smash greatness!";
    shareParams.caption= @"Come smash me back!";
    shareParams.picture= [NSURL URLWithString:pictureURL];
    shareParams.description =
    [NSString stringWithFormat:@"I just smashed %d friends! Can you beat my score?", nScore];
    
    if ([FBDialogs canPresentShareDialogWithParams:shareParams]){
        
        [FBDialogs presentShareDialogWithParams:shareParams
                                    clientState:nil
                                        handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
         if(error) {
         NSLog(@"Error publishing story.");
         } else if (results[@"completionGesture"] && [results[@"completionGesture"] isEqualToString:@"cancel"]) {
         NSLog(@"User canceled story publishing.");
         } else {
         NSLog(@"Story published.");
         }
         }];
        
    }else {
        
        // Prepare the web dialog parameters
        NSDictionary *params = @{
            @"name" : shareParams.name,
            @"caption" : shareParams.caption,
            @"description" : shareParams.description,
            @"picture" : pictureURL,
            @"link" : linkURL
        };
        
        // Invoke the dialog
        [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                               parameters:params
                                                  handler:
         ^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
         if (error) {
         NSLog(@"Error publishing story.");
         } else {
         if (result == FBWebDialogResultDialogNotCompleted) {
         NSLog(@"User canceled story publishing.");
         } else {
         NSLog(@"Story published.");
            MMPPtr->contentShared();
         }
         }}];
    }
}