#ifndef _SClipNode_H
#define _SClipNode_H

//Based on http://www.learn-cocos2d.com/2011/01/cocos2d-gem-clippingnode/


/** Restricts (clips) drawing of all children to a specific region. */

#include "cocos2d.h"


namespace   cocos2d {
    
    class ClipNode : public CCNode
    {
                
        CCRect scissorRect; //in pixels
        CCRect clippingRegionInNodeCoordinates;
    public:
        //
        ClipNode();
        ~ClipNode();
       
        void setClippingRegion( CCRect clipRect);
       
        CC_SYNTHESIZE(bool,clipsToBounds,ClipsToBounds);
        
        //Overloading C++ methods
        
      	virtual void visit(void);
              
        void setScale(float newscale);
        
      

    };


}//namespace   cocos2d 
#endif
