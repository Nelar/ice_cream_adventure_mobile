//
//  IAPHelper.m
//  In App Rage
//
//  Created by Ray Wenderlich on 9/5/12.
//  Copyright (c) 2012 Razeware LLC. All rights reserved.
//

// 1
#import "IAPHelper.h"
#import <StoreKit/StoreKit.h>


// 2
@interface IAPHelper () <SKProductsRequestDelegate, SKPaymentTransactionObserver>

- (void)provideTransactionForValidation:(SKPaymentTransaction *)transaction;
- (void)cancelTransactionForValidation:(SKPaymentTransaction *)transaction;
@end

// 3
@implementation IAPHelper {
    SKProductsRequest * _productsRequest;
    RequestProductsCompletionHandler _completionHandler;
    
    NSSet * _productIdentifiers;
    NSMutableSet * _purchasedProductIdentifiers;
    
}

-(void)dealloc
{
    [_productIdentifiers release];
    [super dealloc];
}

- (id)initWithProductIdentifiers:(NSSet *)productIdentifiers {
    
    if ((self = [super init])) {
        // Store product identifiers
        _productIdentifiers = [NSMutableSet setWithSet:productIdentifiers];
        [_productIdentifiers retain];
        
        // Add self as transaction observer
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    }
    return self;
}

- (void)requestProductsWithCompletionHandler:(RequestProductsCompletionHandler)completionHandler {
    
    
    // 1
    _completionHandler = nil;
    if(completionHandler)
    {
        _completionHandler = [completionHandler copy];
    }

    
    // 2
    _productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:[_productIdentifiers copy]];
    _productsRequest.delegate = self;
    [_productsRequest start];
    
}

- (BOOL)productPurchased:(NSString *)productIdentifier {
    return [_purchasedProductIdentifiers containsObject:productIdentifier];
}

- (void)buyProduct:(SKProduct *)product {
    
    //NSLog(@"Buying %@...", product.productIdentifier);
    
    SKPayment * payment = [SKPayment paymentWithProduct:product];
    [[SKPaymentQueue defaultQueue] addPayment:payment];
    
}

#pragma mark - SKProductsRequestDelegate

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    
    //NSLog(@"Loaded list of products...");
    _productsRequest = nil;
    
  
    NSArray * skProducts = response.products;
    /*
    for (SKProduct * skProduct in skProducts) {
        NSLog(@"Found product: %@ %@ %0.2f",
              skProduct.productIdentifier,
              skProduct.localizedTitle,
              skProduct.price.floatValue);
    }
     */
    
    if(_completionHandler)
    {
        _completionHandler(YES, skProducts);
        _completionHandler = nil;
    }
    
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    
    //NSLog(@"Failed to load list of products.");
    _productsRequest = nil;
    
    if(_completionHandler)
    {
        _completionHandler(NO, nil);
        _completionHandler = nil;
    }
}

#pragma mark SKPaymentTransactionOBserver

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction * transaction in transactions) {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
            default:
                break;
        }
    };
}

- (void)completeTransaction:(SKPaymentTransaction *)transaction {
    //[self provideContentForProductIdentifier:transaction.payment.productIdentifier];
    [self provideTransactionForValidation:transaction];
    
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

- (void)restoreTransaction:(SKPaymentTransaction *)transaction {
    //NSLog(@"restoreTransaction...");
    
    [self provideContentForProductIdentifier:transaction.originalTransaction.payment.productIdentifier];
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

- (void)failedTransaction:(SKPaymentTransaction *)transaction {
    
    NSLog(@"failedTransaction...");
    if (transaction.error.code == SKErrorPaymentCancelled)
    {
        [self cancelTransactionForValidation:transaction];
    }
    
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
}

- (void)provideContentForProductIdentifier:(NSString *)productIdentifier {
    [[NSNotificationCenter defaultCenter] postNotificationName:IAPHelperProductPurchasedNotification object:productIdentifier userInfo:nil];
}

- (void)cancelTransactionForValidation:(SKPaymentTransaction *)transaction {
    [[NSNotificationCenter defaultCenter] postNotificationName:IAPHelperProductPurchasedNotification object:transaction userInfo:transaction];
}

- (void)provideTransactionForValidation:(SKPaymentTransaction *)transaction {
    
    [[NSNotificationCenter defaultCenter] postNotificationName:IAPHelperProductPurchasedNotification object:transaction userInfo:nil];
}

- (void)restoreCompletedTransactions {
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

@end