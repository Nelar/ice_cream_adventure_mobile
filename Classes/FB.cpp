#include "FB.h"
#include "cocos2d.h"
#include <FacebookSDK/FacebookSDK.h>


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
/*
 Это их требования:
 В 4пикс нужна еще одна кнопка - попросить помощи у друзей в файсбуке. Работающая так же и дающая по 60 монеток от каждого кто откликнулся на помощь. Если друг не имеет 4пикс он получает сообщение об ее установке и награждении за это, а тот кто отправил запрос получает 250 монеток за установку другом.
 Если человек у которого попросили помощь авторизован в ФБ, то при входе он получает немного монеток, а попросивший помощь получает 60 монеток.
 Если человек не авторизован, то все по сценарию выше, скачал, установил, получил бонус, а пригласивший 250 монет
 */


static const unsigned int long long kuFBAppID = 196146440562710;

void FB::postToTimeline(string link, string name, string caption, string picture, string pictureURL, string description)
{
    //we don't need write permissions to post to timeline, because user MUST authenticate this
    
    NSString * _link         = [NSString stringWithUTF8String:link.c_str()];
    NSURL    * _linkURL      = [NSURL URLWithString:_link];
    NSString * _name         = [NSString stringWithUTF8String:name.c_str()];
    NSString * _caption      = [NSString stringWithUTF8String:caption.c_str()];
    NSString * _picture      = @"http://www.friendsmash.com/images/logo_large.jpg"; //[NSString stringWithFormat:@"%@%@", [NSString stringWithUTF8String:pictureURL.c_str()],[NSString stringWithUTF8String:localImageName.c_str()]];
    NSURL    * _pictureURL   = [NSURL URLWithString: _picture];
    NSString * _descrription = [NSString stringWithUTF8String:description.c_str()];
    
    
    // Prepare the native share dialog parameters
    FBShareDialogParams *shareParams = [[FBShareDialogParams alloc] init];
    shareParams.link        = _linkURL;
    shareParams.name        = _name;
    shareParams.caption     = _caption;
    shareParams.picture     = _pictureURL;
    shareParams.description = _descrription;
    
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
                                 @"name"        : shareParams.name,
                                 @"caption"     : shareParams.caption,
                                 @"description" : shareParams.description,
                                 @"picture"     : _picture,
                                 @"link"        : _link
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
                 }
             }}];
    }
}

void FB::createNewSession()
{
    FBSession* session = [[FBSession alloc] init];
    [FBSession setActiveSession: session];
}

void FB::login()
{
    if(sessionIsOpen())
    {
        if(!loggedInSuccessfully)
        {
            //that flag is used to call onLogin callback only once
            loggedInSuccessfully = true;
            onLogin(true);
        }
        return;
    }
    
    NSArray *permissions = [[NSArray alloc] initWithObjects:
                            @"email",
                            nil];
    
    // Attempt to open the session. If the session is not open, show the user the Facebook login UX
    [FBSession openActiveSessionWithReadPermissions:permissions allowLoginUI:YES completionHandler:^(FBSession *session,
                                                                                                      FBSessionState status,
                                                                                                      NSError *error)
     {
         // Did something go wrong during login? I.e. did the user cancel?
         if (status == FBSessionStateClosedLoginFailed || status == FBSessionStateCreatedOpening) {
             // If so, just send them round the loop again
             logout();
             createNewSession();
             
             loggedInSuccessfully = false;
             
             onLogin(false);
         }
         else
         {
             if(!loggedInSuccessfully)
             {
                 //that flag is used to call onLogin callback only once
                 loggedInSuccessfully = true;
                 onLogin(true);
             }

         }
         
     }];
}


void FB::processIncomingURL(char *targetURL)
{
    NSURL * url = [NSURL URLWithString:[NSString stringWithUTF8String:targetURL]];
  
    NSRange range = [url.query rangeOfString:@"notif" options:NSCaseInsensitiveSearch];
    
    // If the url's query contains 'notif', we know it's coming from a notification - let's process it
    if(url.query && range.location != NSNotFound)
    {
        processIncomingRequest(targetURL);
    }
}

void FB::processIncomingRequest(char *targetURL)
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

void FB::inviteFriends()
{
    if(!loggedInSuccessfully)
    {
        login();
        return;
    }
    
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
             
            [params setObject:[NSString stringWithUTF8String:buffer.GetString()] forKey:@"data"];
             
             
             FBFrictionlessRecipientCache *friendCache = [[FBFrictionlessRecipientCache alloc] init];
             [friendCache prefetchAndCacheForSession:nil];
             
             [FBWebDialogs presentRequestsDialogModallyWithSession:nil
                                                           message:@"I invite you to play 4Pics: New Episode. You'll get a bonus, if you install :)"
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
                                                                       }
                                                                   }
                                                               }
                                                           }
                                                       friendCache:friendCache];
             
         }
     }];

}

void FB::sendRequest(string message, string title, string jsonData, vector<string> to, bool onlyMobileUsers)
{
    if(!loggedInSuccessfully)
    {
        login();
        return;
    }
    
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
                     if ([friendObject objectForKey:@"devices"] && [friendObject objectForKey:@"installed"])
                     {
                         NSArray *deviceData = [friendObject objectForKey:@"devices"];
                         
                         for (NSDictionary *deviceObject in deviceData)
                         {
                             if(!onlyMobileUsers){
                                 [deviceFilteredFriends addObject: [friendObject objectForKey:@"id"]];
                                 break;
                             }
                             else if ([@"iOS" isEqualToString: [deviceObject objectForKey:@"os"]] || [@"Android" isEqualToString: [deviceObject objectForKey:@"os"]])
                             {
                                 [deviceFilteredFriends addObject: [friendObject objectForKey:@"id"]];
                                 break;
                             }
                         }
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

bool FB::sessionStateCreatedTokenLoaded()
{
    return [FBSession activeSession].state == FBSessionStateCreatedTokenLoaded;
}

bool FB::sessionIsOpen(){
    return [FBSession activeSession].isOpen;
}

void FB::logout()
{
    [[FBSession activeSession] closeAndClearTokenInformation];
    [FBSession setActiveSession:nil];
    onLogout();
}

void FB::requestWritePermissions()
{
    NSArray *permissions = [[NSArray alloc] initWithObjects:
                            @"publish_actions", nil];
    
    [[FBSession activeSession] requestNewPublishPermissions:permissions defaultAudience:FBSessionDefaultAudienceFriends completionHandler:^(FBSession *session, NSError *error) {
        NSLog(@"Reauthorized with publish permissions.");
    }];
}

