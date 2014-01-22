#include <iostream>
#include <vector>
#include "NotificationTypes.h"
using namespace std;

class FB
{
public:
    static FB * sharedInstance(){
        static FB * instance = new FB();
        return instance;
    }
    
private:
    
    ////////////////
    FB(){};
    FB(FB const&);
    void operator=(FB const&);
    
public:
    bool loggedInSuccessfully;
    
    //callbacks
    function<void(const char *, const char *, const char *)>onNotificationRecieved;
    function<void(bool)>onLogin;
    function<void()>onLogout;
    
    //session management
    void createNewSession();
    bool sessionStateCreatedTokenLoaded();
    bool sessionIsOpen();
    
    void login();
    void logout();
    void requestWritePermissions(); //used internally, whenever we wish to post something automatically
    
    void postToTimeline(string link, string name, string caption, string picture, string pictureURL, string description);
    
    void processIncomingURL(char * targetURL);
    void processIncomingRequest(char *  targetURL);
    void processIncomingFeed(char *  targetURL);
    
    void sendRequest(string message, string title, string jsonData, vector<string> to, bool onlyMobileUsers = false);
    void inviteFriends();
};
