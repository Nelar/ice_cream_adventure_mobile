//
//  IAP.h
//  FourPicsOneWord
//
//  Created by Phil Maevskiy on 9/5/13.
//
//

#ifndef __FourPicsOneWord__IAP__
#define __FourPicsOneWord__IAP__

#include "cocos2d.h"
#include <iostream>
using namespace std;

#include "HttpClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

typedef cocos2d::extension::CCHttpRequest  HttpRequest;
typedef cocos2d::extension::CCHttpResponse HttpResponse;
typedef cocos2d::extension::CCHttpClient   HttpClient;

class IAPProduct
{
public:
    string localizedDescription;
    string localizedTitle;
    float price;
    string priceLocale;
    string productIdentifier;
};

class IAP
{
public:
    static IAP& sharedInstance(){
        static IAP instance;
        return instance;
    }
    
    vector<IAPProduct *>products;
    
    function<void(string)> provideContentForProductIdentifier;
    function<void()> productsRecieved;
    function<void()> cancelProductPayment;
    
    void initWithProductIdentifiers(vector<string> productIdentifiers);
    void requestProducts();
    void buyProduct(string productIdentifier);
    bool productPurchased(string productIdentifier);
    void restoreCompletedTransactions();
    void validateReciept(string url, string requestData, string productIdentifier);
    
    void validationServerResponse(HttpClient * client, HttpResponse * response);
    
    ~IAP();
    
private:
    IAP();              
    IAP(IAP const&);
    void operator=(IAP const&);
};

#endif /* defined(__FourPicsOneWord__IAP__) */
