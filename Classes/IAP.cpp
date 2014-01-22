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

#include "MMPInterface.h"
#include "Options.h"

using namespace MarketingPlatform;
using namespace Core;

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
    std::vector<char> *buffer = response->getResponseData();   
    std::string str = std::string(buffer->begin(), buffer->end());
    
    rapidjson::Document d;
    d.Parse<0>(str.c_str());
    
    string code = d["code"].GetString();

    if(code == "COMPLETED")
    {
        string productIdentifier = response->getHttpRequest()->getTag();
        if(IAP::sharedInstance().provideContentForProductIdentifier)
        {
            IAP::sharedInstance().provideContentForProductIdentifier(productIdentifier);
        }
    }
    else if(code == "CANCEL")
    {
        //wrong params
        //do nothing
    }
    else if(code == "WAIT")
    {
        //call again
    }else if(code == "ERROR")
    {
        //do nothing
    }else{
        //strange
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

-(void)provideContentForProductIdentifier:(NSString *) productIdentifier
{
    if(IAP::sharedInstance().provideContentForProductIdentifier)
    {
        IAP::sharedInstance().provideContentForProductIdentifier(string([productIdentifier UTF8String]));
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
    
    string productIdentifier = [transaction.payment.productIdentifier UTF8String];
    
    string url = "https://payments.ddestiny.ru/mobile/ios/";
    string currency  = [[product.priceLocale localeIdentifier] UTF8String];
    string packet    = [[[NSString alloc]initWithData:transaction.transactionReceipt encoding:NSUTF8StringEncoding] UTF8String];
    string base64packet = base64_encode(reinterpret_cast<const unsigned char*>(packet.c_str()), packet.length());
    string project   = "com.destinygroup.icecreamadventure";
    string password  = "tPRVWTFA0tCmD9pAh0h7WFXi3cDXBd3s";
    string signature = password + ";" + currency + ";" + base64packet + ";" + project + ";" + password;
    string md5signature = md5(signature);
    string requestData = "project=" + project + "&packet=" + base64packet + "&currency=" + currency + "&signature=" + md5signature;
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
    
    string subCurrency = currency.substr(currency.find('=')+1, currency.size());
    
    Core::MMPInterface::Instance()->PurchaseMade(price, productIdentifier.c_str(), subCurrency, flagPay);
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
    //[[NSNotificationCenter defaultCenter] addObserver:listener selector:@selector(provideContentForProductIdentifier:) name:IAPHelperProductPurchasedNotification object:nil];
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
