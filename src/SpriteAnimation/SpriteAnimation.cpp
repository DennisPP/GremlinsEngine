#include "SpriteAnimation.h"



#include <cassert>

#include <math.h>


//------------------------------------------------------------------------------
TSpriteFrame::TSpriteFrame()
{
}

//------------------------------------------------------------------------------
TSpriteFrame::~TSpriteFrame()
{
}

//------------------------------------------------------------------------------
void TSpriteFrame::SetDuration(float duration)
{
	mDuration = duration;
}

//------------------------------------------------------------------------------
TSpriteAnimation::TSpriteAnimation()
:	mX(0.0f)
,	mY(0.0f)
,	mAngle(0.0f)
,	mIsPlaying(false)
,	mPlayedTime(0.0f)
,	mTotalTime(1.0f)
,	mIsFlipped(false)
{
}

//------------------------------------------------------------------------------
TSpriteAnimation::~TSpriteAnimation()
{
}

//------------------------------------------------------------------------------
void TSpriteAnimation::SetFlipped(bool flipped)
{
	mIsFlipped = flipped;
}

//------------------------------------------------------------------------------
bool TSpriteAnimation::IsFlipped() const
{
	return mIsFlipped;
}

//------------------------------------------------------------------------------
void TSpriteAnimation::SetAngle(float angle)
{
	mAngle = angle;
}

//------------------------------------------------------------------------------
float TSpriteAnimation::GetAngle() const
{
	return mAngle;
}


//------------------------------------------------------------------------------
void TSpriteAnimation::SetPosition(float x, float y)
{
	mX = x;
	mY = y;
}

//------------------------------------------------------------------------------
void TSpriteAnimation::AddSpriteFrame(const TSpriteFrame & frame)
{
	mSpriteFrames.push_back(frame);
}

//------------------------------------------------------------------------------
void TSpriteAnimation::SetTotalTime(float totalTime)
{
	assert( totalTime >= 0.0f );
	
	mTotalTime = totalTime;

	if( mSpriteFrames.empty() )
		return;

	float duration = totalTime / mSpriteFrames.size();
	for(size_t j=0;j<mSpriteFrames.size();++j)
	{
		mSpriteFrames[j].mDuration = duration;
	}
}

//------------------------------------------------------------------------------
void TSpriteAnimation::Render()
{
	if( ! mIsPlaying || mSpriteFrames.empty() )
		return;

	
	float t = fmod(mPlayedTime,mTotalTime);

	int frameCount = (int)mSpriteFrames.size();

	int frame = (int) (t * frameCount / mTotalTime);

	if( frame >= frameCount )
		frame = frameCount - 1;
	
	//TQuad2DRenderer & renderer = TQuad2DRenderer::GetInstance();
	//renderer.BindTexture(mSpriteFrames[frame].mSprite.mQuad.mTexture);
	//renderer.RenderQuad(&mSpriteFrames[frame].mSprite.mQuad, mX, mY, mAngle, mIsFlipped);

	RenderRotatedSprite(&mSpriteFrames[frame].mSprite, mX, mY, mAngle, mIsFlipped? -1.0f : 1.0f, 1.0f);
}

//------------------------------------------------------------------------------
void TSpriteAnimation::StartAnimation()
{
	mIsPlaying = true;
	mPlayedTime = 0.0f;
}

//------------------------------------------------------------------------------
void TSpriteAnimation::SetPlaying(bool playing)
{
	mIsPlaying = playing;
}

//------------------------------------------------------------------------------
bool TSpriteAnimation::IsPlaying() const
{
	return mIsPlaying;
}

//------------------------------------------------------------------------------
void TSpriteAnimation::Tick(float dt)
{
	if( ! mIsPlaying )
		return;

	

	mPlayedTime += dt;
}

//------------------------------------------------------------------------------