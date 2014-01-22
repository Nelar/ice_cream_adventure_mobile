#ifndef __ICEMOTION_STREAK_H__
#define __ICEMOTION_STREAK_H__

#include "CCProtocols.h"
#include "textures/CCTexture2D.h"
#include "ccTypes.h"
#include "base_nodes/CCNode.h"

using namespace cocos2d;

class IceMotionStreak : public CCNodeRGBA, public CCTextureProtocol
{
public:
    IceMotionStreak();
    virtual ~IceMotionStreak();

    /** creates and initializes a motion streak with fade in seconds, minimum segments, stroke's width, color, texture filename */
    static IceMotionStreak* create(float fade, float minSeg, float stroke, ccColor3B color, const char* path);
    /** creates and initializes a motion streak with fade in seconds, minimum segments, stroke's width, color, texture */
    static IceMotionStreak* create(float fade, float minSeg, float stroke, ccColor3B color, CCTexture2D* texture);

    /** initializes a motion streak with fade in seconds, minimum segments, stroke's width, color and texture filename */
    bool initWithFade(float fade, float minSeg, float stroke, ccColor3B color, const char* path);
    /** initializes a motion streak with fade in seconds, minimum segments, stroke's width, color and texture  */
    bool initWithFade(float fade, float minSeg, float stroke, ccColor3B color, CCTexture2D* texture);

    /** color used for the tint */
    void tintWithColor(ccColor3B colors);

    /** Remove all living segments of the ribbon */
    void reset();

    /** Override super methods */
    virtual void setPosition(const CCPoint& position);
    virtual void draw();
    virtual void update(float delta);

    /* Implement interfaces */
    virtual CCTexture2D* getTexture(void);
    virtual void setTexture(CCTexture2D *texture);
    virtual void setBlendFunc(ccBlendFunc blendFunc);
    virtual ccBlendFunc getBlendFunc(void);
    virtual GLubyte getOpacity(void);
    virtual void setOpacity(GLubyte opacity);
    virtual void setOpacityModifyRGB(bool bValue);
    virtual bool isOpacityModifyRGB(void);

    /** When fast mode is enabled, new points are added faster but with lower precision */
    inline bool isFastMode() { return m_bFastMode; }
    inline void setFastMode(bool bFastMode) { m_bFastMode = bFastMode; }

    inline bool isStartingPositionInitialized() { return m_bStartingPositionInitialized; }
    inline void setStartingPositionInitialized(bool bStartingPositionInitialized) 
    { 
        m_bStartingPositionInitialized = bStartingPositionInitialized; 
    }
protected:
    bool m_bFastMode;
    bool m_bStartingPositionInitialized;
private:
    /** texture used for the motion streak */
    CCTexture2D* m_pTexture;
    ccBlendFunc m_tBlendFunc;
    CCPoint m_tPositionR;

    float m_fStroke;
    float m_fFadeDelta;
    float m_fMinSeg;

    unsigned int m_uMaxPoints;
    unsigned int m_uNuPoints;
    unsigned int m_uPreviousNuPoints;

    /** Pointers */
    CCPoint* m_pPointVertexes;
    float* m_pPointState;

    // Opengl
    ccVertex2F* m_pVertices;
    GLubyte* m_pColorPointer;
    ccTex2F* m_pTexCoords;
};

#endif
