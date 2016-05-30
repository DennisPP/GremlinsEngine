#ifndef __FireEngine__SpriteAnimation__h__
#define __FireEngine__SpriteAnimation__h__

#include <string>
#include <vector>

#include "Graphics/Quad2D.h"

#include "Sprite.h"

//------------------------------------------------------------------------------
class TSpriteFrame
{
public:
	TSpriteFrame();
	~TSpriteFrame();

	void SetDuration(float duration);
	
	TSprite mSprite;

	float mDuration;
};

//------------------------------------------------------------------------------
class TSpriteAnimation
{
public:
	TSpriteAnimation();
	~TSpriteAnimation();

	void AddSpriteFrame(const TSpriteFrame & frame);

	void Render();

	void SetAngle(float angle);
	float GetAngle() const;

	void SetPosition(float x, float y);

	void SetPlaying(bool playing);
	bool IsPlaying() const;
	void StartAnimation();
	void SetTotalTime(float totalTime);
	void SetFlipped(bool flipped);
	bool IsFlipped() const;
	
	
	void Tick(float dt);
	
protected:
	std::vector<TSpriteFrame> mSpriteFrames;
	float mAngle;
	float mX, mY;
	bool mIsPlaying;
	bool mIsFlipped;
	float mPlayedTime;
	float mTotalTime;
};

//------------------------------------------------------------------------------
#endif