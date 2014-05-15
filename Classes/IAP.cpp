//
//  IAP.cpp
//  FourPicsOneWord
//
//  Created by Phil Maevskiy on 9/5/13.
//
//

#include "IAP.h"
#include "MD5.h"
#include "base64.h"

#include "cocos2d.h"

#include "rapidjson/document.h"

#include "Options.h"
#include "nMMP.h"

#include "cFacebook.h"
#import "AppsFlyerTracker.h"
#import <AdSupport/AdSupport.h>
#include "cGlobal.h"
#include "GameScene.h"
#include "GameMapLayer.h"

//
//Cocos2d object to catch responses
//
class HttpCatcher : public cocos2d::CCObject
{
public:
    void validationServerResponse(HttpClient * client, HttpResponse * response);
};

void HttpCatcher::validationServerResponse(HttpClient * client, HttpResponse * response)
{
    if(!response->isSucceed())
    {
        cocos2d::extension::CCHttpClient::getInstance()->send(response->getHttpRequest());
        return;
    }
    std::vector<char> *buffer = response->getResponseData();
    std::string str = std::string(buffer->begin(), buffer->end());
    if(response->getResponseData() == NULL)
        return;
    
    int strFind = str.find("COMPLETED");
    CCLog("%s", str.c_str());
    if(strFind >= 0)
    {
        string productIdentifier = response->getHttpRequest()->getTag();
        if(IAP::sharedInstance().provideContentForProductIdentifier)
        {
            IAP::sharedInstance().provideContentForProductIdentifier(productIdentifier);
        }
        MMPPtr->purchaseMade(productIdentifier.c_str(), GlobalsPtr->currency, GlobalsPtr->price, true);
    }
    else
    {
        string productIdentifier = response->getHttpRequest()->getTag();
        MMPPtr->purchaseMade(productIdentifier.c_str(), GlobalsPtr->currency, GlobalsPtr->price, false);
        if (GlobalsPtr->iceCreamScene == Game)
        {
            GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
            layer->closeLoading();
        }
        else if (GlobalsPtr->iceCreamScene == Map)
        {
            GameMapLayer* layer = ((GameMapLayer*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
            layer->closeLoading();
        }
        
        if (GlobalsPtr->iceCreamScene == Game)
        {
            GameScene* layer = ((GameScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
            layer->cancelPayment();
        }

    }
}


#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#import "IAPHelper.h"

//
//Proxy Objective-C class that captures notifications and sends them to IAP::sharedInstance()
//
@interface IAPEventListener : NSObject

-(void)provideContentForProductIdentifier:(NSString *) productIdentifier;
-(void)validateReciept:(SKPaymentTransaction *)transaction;
-(void)cancelPayment;
-(void)productsRecieved;

@property(nonatomic, retain) NSArray * products;

@end

@implementation IAPEventListener

-(void)provideContentForProductIdentifier:(NSNotification *)notification
{
    if(IAP::sharedInstance().provideContentForProductIdentifier)
    {
        IAP::sharedInstance().provideContentForProductIdentifier(string([((NSString*)notification.object) UTF8String]));
    }
}

-(void)validateReciept:(NSNotification *)notification
{
    if (notification.userInfo != nil) {
        [self cancelPayment];
        return;
    }
    SKPaymentTransaction * transaction = (SKPaymentTransaction*)notification.object;
    
    SKProduct * product = nil;
    for(int i =0; i < [self.products count]; ++i)
    {
        SKProduct * pr = (SKProduct *)[self.products objectAtIndex: i];
        if([pr.productIdentifier isEqualToString:transaction.payment.productIdentifier])
        {
            product = pr;
        }
    }
    
    if(product == nil)return;
    
    MMPPtr->purchaseConfirmed();
    
    string productIdentifier = [transaction.payment.productIdentifier UTF8String];
    string do_test = "do_test";
    
    
    string url = "https://payments.ddestiny.ru/mobile/ios/?";
    string currency  = [[product.priceLocale localeIdentifier] UTF8String];
    string packet    = [[[NSString alloc]initWithData:transaction.transactionReceipt encoding:NSUTF8StringEncoding] UTF8String];
    string base64packet = base64_encode(reinterpret_cast<const unsigned char*>(packet.c_str()), packet.length());
    string project   = "com.destinygroup.icecreamadventure";
    string password  = "tPRVWTFA0tCmD9pAh0h7WFXi3cDXBd3s";
    
    string signature = password + ";" +idfa() + ";;" + currency + ";" + base64packet + ";" + project + ";" + do_test + ";" + password;
//    string signature = password + ";" +idfa() + ";;" + currency + ";" + base64packet + ";" + project + ";" + password;
    string md5signature = md5(signature);
    string requestData = "project=" + project + "&account=" + idfa() + "&test=" + do_test + "&packet=" + base64packet + "&currency=" + currency + "&signature=" + md5signature;
//    string requestData = "project=" + project + "&account=" + idfa() + "&packet=" + base64packet + "&currency=" + currency + "&signature=" + md5signature;
    
    IAP::sharedInstance().validateReciept(url, requestData, productIdentifier);
    
    productIdentifier = [[product productIdentifier] UTF8String];
    
    float price = [[product price] floatValue];
    bool flagPay = false;
    if (productIdentifier == "com.destiny.icecreamadventure.5lives")
        flagPay = OptionsPtr->getFirstPayLife();
    else if (productIdentifier == "com.destiny.icecreamadventure.5moves")
        flagPay = OptionsPtr->getFirstPayMove();
    else if (productIdentifier == "com.destiny.icecreamadventure.hammer")
        flagPay = OptionsPtr->getFirstPayHammer();
    else if (productIdentifier == "com.destiny.icecreamadventure.penguins")
        flagPay = OptionsPtr->getFirstPayFish();
    else if (productIdentifier == "com.destiny.icecreamadventure.stripedandbomb")
        flagPay = OptionsPtr->getFirstPayBomb();
    else if (productIdentifier == "com.destiny.icecreamadventure.superelements")
        flagPay = OptionsPtr->getFirstPayCrystal();
    else if (productIdentifier == "com.destiny.icecreamadventure.superelements")
        flagPay = OptionsPtr->getFirstPayCrystal();
    else
        flagPay = true;
    
    string subCurrency = currency.substr(currency.find('=')+1, currency.size());
    GlobalsPtr->currency = subCurrency;
    GlobalsPtr->price = price;
    
    
    NSString *currencyCode = [product.priceLocale objectForKey:NSLocaleCurrencyCode];
    [AppsFlyerTracker sharedTracker].currencyCode = currencyCode;
    
    
    /*    bool shouldTrack = true;
     if (shouldTrack)
     {
     NSString *eventName = @"purchase";
     if (nil != product)
     {
     NSString *currencyCode = [product.priceLocale objectForKey:NSLocaleCurrencyCode];
     int quantity = transaction.payment.quantity;
     float unitPrice = [product.price floatValue];
     float revenue = unitPrice * quantity;
     MATEventItem *eventItem = [MATEventItem eventItemWithName:product.localizedTitle unitPrice:unitPrice quantity:quantity revenue:revenue attribute1:@"attr1" attribute2:@"attr2" attribute3:@"attr3" attribute4:@"attr4" attribute5:@"attr5"];
     NSArray *arrEventItems = @[ eventItem ];
     float extraRevenue = 0;
     
     if (FacebookPtr->sessionIsOpened())
     [MobileAppTracker setFacebookUserId:[NSString stringWithFormat:@"%llu", FacebookPtr->fbid]];
     
     [MobileAppTracker measureAction:eventName
     eventItems:arrEventItems
     referenceId:transaction.transactionIdentifier
     revenueAmount:extraRevenue
     currencyCode:currencyCode];
     NSLog(@"Transaction event tracked: %@", eventName);
     }
     }*/
    
    MMPPtr->purchaseConfirmed();
}

-(void)productsRecieved{
    
    if(IAP::sharedInstance().productsRecieved)
    {
        IAP::sharedInstance().productsRecieved();
    }
}

-(void)cancelPayment{
    
    if(IAP::sharedInstance().cancelProductPayment)
    {
        IAP::sharedInstance().cancelProductPayment();
    }
    MMPPtr->purchaseCanceled();
}

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}
@end
//
//
//

IAPHelper * helper;
IAPEventListener * listener;
HttpCatcher * catcher;

IAP::IAP()
{
    provideContentForProductIdentifier = NULL;
    productsRecieved = NULL;
    
    catcher = new HttpCatcher();
    helper = [IAPHelper alloc];
    listener = [[IAPEventListener alloc]init];
    listener.products = [NSArray array];
    [[NSNotificationCenter defaultCenter] addObserver:listener selector:@selector(provideContentForProductIdentifier:) name:IAPHelperProductRestoreNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:listener selector:@selector(validateReciept:) name:IAPHelperProductPurchasedNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:listener selector:@selector(cancelPayment:) name:IAPHelperProductPurchasedCancelNotification object:nil];
}

IAP::~IAP()
{
    catcher->release();
    
    helper = nil;
    listener.products = nil;
    listener = nil;
    
    for(int i = 0; i < products.size(); ++i)
    {
        IAPProduct * pr = products[i];
        delete pr;
    }
    products.clear();
}

void IAP::validateReciept(string url, string requestData, string productIdentifier)
{
    HttpRequest * request = new HttpRequest();
    request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpPost);
    request->setUrl(url.c_str());
    request->setRequestData(requestData.c_str(), strlen(requestData.c_str()));

    request->setTag(productIdentifier.c_str());
    request->setResponseCallback(catcher, httpresponse_selector(HttpCatcher::validationServerResponse));
    
    cocos2d::extension::CCHttpClient::getInstance()->send(request);
    request->release();
}

void IAP::initWithProductIdentifiers(vector<string> _productIdentifiers)
{
    NSMutableSet * productIdentifiers = [NSMutableSet set];
    
    for(int i = 0 ; i < _productIdentifiers.size(); ++i)
    {
        string cppStr = _productIdentifiers[i];
        NSString * objcStr = [NSString stringWithUTF8String:cppStr.c_str()];
        [productIdentifiers addObject: objcStr];
    }
    
    
    [helper initWithProductIdentifiers: productIdentifiers];
}

void IAP::requestProducts(){
    
    [helper requestProductsWithCompletionHandler:
     ^(BOOL success, NSArray * _products)
     {
         if (success)
         {  
             products.clear();
             listener.products = [NSArray arrayWithArray:_products];
             
             NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
             [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
             [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
             
             for(int i = 0; i < [listener.products count]; ++i)
             {
                 SKProduct * product = [listener.products objectAtIndex:i];
                 float price = [product.price floatValue];
                 string localizedDescription = string([product.localizedDescription UTF8String]);
                 string localizedTitle = string([product.localizedTitle UTF8String]);
                 
                 [numberFormatter setLocale:product.priceLocale];
                 NSString *formattedLocalizedPrice = [numberFormatter stringFromNumber:product.price];
                 
                 string priceLocale = string([formattedLocalizedPrice UTF8String]);
                 string productIdentifier = string([product.productIdentifier UTF8String]);
                 
                 IAPProduct * pr = new IAPProduct();
                 pr->price = price;
                 pr->localizedDescription = localizedDescription;
                 pr->localizedTitle = localizedTitle;
                 pr->priceLocale = priceLocale;
                 pr->productIdentifier = productIdentifier;
                 products.push_back(pr);
             }
             
             if(productsRecieved) productsRecieved();
         }
     }
     ];
}


void IAP::buyProduct(string productIdentifier)
{
    NSString * _productIdentifier = [NSString stringWithUTF8String:productIdentifier.c_str()];
    
    int k = [listener.products count];
    
    for(int i = 0; i < k; ++i)
    {
        SKProduct * product = (SKProduct *)[listener.products objectAtIndex:i];
        if([_productIdentifier isEqualToString:product.productIdentifier]){
            [helper buyProduct:product];
            MMPPtr->purchaseStarted();
            return;
        }
    }
}


bool IAP::productPurchased(string productIdentifier)
{
    return [helper productPurchased:[NSString stringWithUTF8String:productIdentifier.c_str()]];
}

void IAP::restoreCompletedTransactions()
{
    [helper restoreCompletedTransactions];
}

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//android implementation
#endif
