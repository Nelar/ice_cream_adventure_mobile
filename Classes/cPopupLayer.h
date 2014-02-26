#ifndef __POPUP_SCENE_H__
#define __POPUP_SCENE_H__

#include "cocos2d.h"
#include <vector>

using namespace std;
using namespace cocos2d;

enum ePopupColor
{
    GreenPopup,
    PinkPopup,
    GrayPopup
};

enum eBoosterPopupType
{
    CrystalPopBoot,
    BombPopBoot,
    FishPopBoot,
    MovesPopBoot,
    TimePopBoot,
    ChupaPopBoot
};


class PopupLayer : public cocos2d::CCLayer
{
public:
    
    virtual ~PopupLayer();
	virtual bool init();

	CREATE_FUNC(PopupLayer);
    
    void popup(char* title, char* text, char* buttonText, ePopupColor popupColor,
               CCObject* npSelectorTarget, SEL_CallFuncN nselector,
               CCObject* ncloseSelectorTarget, SEL_CallFuncN ncloseselector);
    
    void popupBoosterInApp(char* title, char* text, ePopupColor popupColor,
                           eBoosterPopupType typeBoo,
               CCObject* npSelectorTarget, SEL_CallFuncN nselector,
               CCObject* ncloseSelectorTarget, SEL_CallFuncN ncloseselector);
    
    void popupPost(char* title, char* text, char* buttonText, ePopupColor popupColor,
                   eBoosterPopupType typeBoo,
               CCObject* npSelectorTarget, SEL_CallFuncN nselector,
               CCObject* ncloseSelectorTarget, SEL_CallFuncN ncloseselector);
    
    
    virtual void changeOrientation();
    
    
    void loading(char* closeLoading);
    void closeLoading();
    
    void closePopup(CCObject* pSender);
    void buttonClick(CCObject* pSender);
    
private:
    
	bool lock;
    
    CCObject* pSelectorTarget;
    SEL_CallFuncN selector;
    
    CCObject* closeTarget;
    SEL_CallFuncN closeSelector;
    
    CCLayerColor* background;
    CCSprite* plate;
    
    float popupScale;
    
    CCSprite* butUp;
    CCLabelTTF* labelTTF;
    
    bool isPopup = false;
};

#endif