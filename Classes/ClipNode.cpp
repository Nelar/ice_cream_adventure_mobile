#include "ClipNode.h"
#include "cocos2d.h"

namespace   cocos2d {
    ClipNode::ClipNode(){
    }
    
    ClipNode::~ClipNode(){
    }
    
       void ClipNode::setClippingRegion( CCRect region){
    
        clippingRegionInNodeCoordinates=region;
        
        this->setContentSize(region.size);
            
        // respect scaling but don't convert points to pixels
        scissorRect = region;
    }
    
    void ClipNode::setScale(float newscale){
    
        CCLOG("ClipNode::setScale %f",newscale);
        
        CCNode::setScale(newscale);
        
        // re-adjust the clipping region according to the current scale factor
        setClippingRegion(clippingRegionInNodeCoordinates);
    }
    
    void ClipNode::visit() 
    {
        glEnable(GL_SCISSOR_TEST);
        CCEGLView::sharedOpenGLView()->setScissorInPoints(scissorRect.origin.x, scissorRect.origin.y, scissorRect.size.width, scissorRect.size.height);
        
        if(m_pChildren)
        {
            ccArray *arrayData = m_pChildren->data;
            unsigned int i=0;
            
            // draw children zOrder < 0
            for( ; i < arrayData->num; i++ )
            {
                CCNode *child =  (CCNode*)arrayData->arr[i];
                if ( child->getZOrder() < 0 )
                {
                    child->visit();
                }
                else
                {
                    break;
                }
            }
            
            // this draw
            this->draw();
            
            // draw children zOrder >= 0
            for( ; i < arrayData->num; i++ )
            {
                CCNode* child = (CCNode*)arrayData->arr[i];
                child->visit();
            }
            
        }
        else
        {
            this->draw();
        }
        
        glDisable(GL_SCISSOR_TEST);
    }
        
  
}//namespace   cocos2d 

