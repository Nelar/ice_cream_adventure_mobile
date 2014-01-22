#ifndef __SHADER_SPRITE_H__
#define __SHADER_SPRITE_H__

#include "cocos2d.h"
using namespace cocos2d;

class SpriteBlur : public CCSprite
{
public:
	~SpriteBlur();
	void setBlurSize(float f);
	bool initWithTexture(CCTexture2D* texture, const CCRect&  rect);
	void draw();
	void initProgram();
	void listenBackToForeground(CCObject *obj);

	static SpriteBlur* create(const char *pszFileName);
	static SpriteBlur* createWithTexture(CCTexture2D *pTexture);

	CCPoint blur_;
	GLfloat    sub_[4];

	GLuint    blurLocation;
	GLuint    subLocation;
};

#endif

