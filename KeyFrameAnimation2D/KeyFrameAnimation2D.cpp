#include "KeyFrameAnimation2D.h"
#include "../SpriteAnimation/Sprite.h"


#include <cassert>
#include <math.h>

namespace KeyFrameAnimation2D
{

TKeyFrame::TKeyFrame()
:	time(0.0f)
,	x(0.0f)
,	y(0.0f)
,	rotation(0.0f)
,	scaleX(1.0f)
,	scaleY(1.0f)
{
	color = TColor::White;
}

TKeyFrame::TKeyFrame(float _time, float _x, float _y, float _rotation, float _scaleX, float _scaleY, const TColor& _color)
:	time(_time)
,	x(_x)
,	y(_y)
,	scaleX(_scaleX)
,	scaleY(_scaleY)
,	rotation(_rotation)
{
	color = _color;
}


TKeyFrameAnimation2D::TKeyFrameAnimation2D()
:	mSprite(NULL)
{	
	mPlayMode = TKeyFrameAnimation2D::PLAY_LOOPED;	
	mIsPlaying = false;
	mPlayedTime = 0.0f;


	mCurrentFrame.rotation = 0.0f;
	mCurrentFrame.scaleX = 1.0f;
	mCurrentFrame.scaleY = 1.0f;
	mCurrentFrame.time = 0.0;
	mCurrentFrame.x = 0.0f;
	mCurrentFrame.y = 0.0f;
}

TKeyFrameAnimation2D::~TKeyFrameAnimation2D()
{
	// actually no need
	mSprite = NULL;
}

	

void TKeyFrameAnimation2D::SetSprite(TSprite* sprite)
{
	mSprite = sprite;
}

bool TKeyFrameAnimation2D::IsPlaying() const
{
	return mIsPlaying;
}

void TKeyFrameAnimation2D::Play(float time)
{
	mPlayedTime = time;
	mIsPlaying = true;	
}

void TKeyFrameAnimation2D::Stop()
{
	mIsPlaying = false;
}

TKeyFrameAnimation2D::TPlayMode TKeyFrameAnimation2D::GetPlayMode() const
{
	return mPlayMode;
}

void TKeyFrameAnimation2D::SetPlayMode(TKeyFrameAnimation2D::TPlayMode mode)
{
	mPlayMode = mode;
}

void TKeyFrameAnimation2D::GotoAndStop(float time)
{
	mPlayedTime = time;
	mIsPlaying = false;
}



void TKeyFrameAnimation2D::AddKeyFrame(const TKeyFrame & frame)
{
	mKeyFrames.push_back(frame);
}

void TKeyFrameAnimation2D::Update(float dt)
{
	if( mKeyFrames.empty() )
		return;

	if( IsPlaying() )
		mPlayedTime += dt;
	

	float sampleTime = mPlayedTime;
	if( mPlayMode == TKeyFrameAnimation2D::PLAY_LOOPED )
	{
		int last = (int)mKeyFrames.size() - 1;
		const float totalFrameTime = mKeyFrames[last].time; 
		if( mPlayedTime > totalFrameTime )
			sampleTime = fmod(mPlayedTime, totalFrameTime);
	}
	else if( mPlayMode == TKeyFrameAnimation2D::PLAY_ONCE_AND_STOP )
	{
		int last = (int)mKeyFrames.size() - 1;
		const float totalFrameTime = mKeyFrames[last].time; 
		if( mPlayedTime > totalFrameTime )
			sampleTime = totalFrameTime;
	}


	int frame = 0;
	for(size_t j=1;j<mKeyFrames.size();++j)
	{
		const TKeyFrame& keyFrame0 = mKeyFrames[j-1];
		const TKeyFrame& keyFrame1 = mKeyFrames[j];
		float time0 = keyFrame0.time;
		float time1 = keyFrame1.time;
		if( sampleTime >= time0 && sampleTime <= time1 )
		{
			float t1 = (sampleTime - time0);
			float t0 = time1 - sampleTime;
			float tt = time1 - time0;
			// normalize the t0 and t1
			if( tt > 0.0f )
			{ 
				t0 /= tt;
				t1 /= tt;
			}


			frame = (int)j;

			mCurrentFrame.time = sampleTime;
			mCurrentFrame.x = t0 * keyFrame0.x + t1 * keyFrame1.x;
			mCurrentFrame.y = t0 * keyFrame0.y + t1 * keyFrame1.y;
			mCurrentFrame.scaleX = t0 * keyFrame0.scaleX + t1 * keyFrame1.scaleX;			
			mCurrentFrame.scaleY = t0 * keyFrame0.scaleY + t1 * keyFrame1.scaleY;			
			mCurrentFrame.rotation = t0 * keyFrame0.rotation + t1 * keyFrame1.rotation;
			mCurrentFrame.color.r = t0 * keyFrame0.color.r + t1 * keyFrame1.color.r;
			mCurrentFrame.color.g = t0 * keyFrame0.color.g + t1 * keyFrame1.color.g;
			mCurrentFrame.color.b = t0 * keyFrame0.color.b + t1 * keyFrame1.color.b;
			mCurrentFrame.color.a = t0 * keyFrame0.color.a + t1 * keyFrame1.color.a;
			break;
		}
	}	
}


void TKeyFrameAnimation2D::Render()
{
	if( ! mSprite )
		return;
	if( ! mIsPlaying )
		return;
	float x = mCurrentFrame.x; 
	float y = mCurrentFrame.y; 
	float scaleX = mCurrentFrame.scaleX; 
	float scaleY = mCurrentFrame.scaleY; 
	float rotation = mCurrentFrame.rotation;

	RenderRotatedSprite(mSprite, x, y, rotation, scaleX, scaleY, mCurrentFrame.color);
}

TKeyFrameAnimationGroup2D::TKeyFrameAnimationGroup2D()
:	mName("no name")
{
}

TKeyFrameAnimationGroup2D::~TKeyFrameAnimationGroup2D()
{
	for(size_t j=0;j<mAnimations.size();++j)
	{
		if( mAnimations[j] )
			delete mAnimations[j];
	}
	mAnimations.clear();
}

const std::string & TKeyFrameAnimationGroup2D::GetName() const
{
	return mName;
}

void TKeyFrameAnimationGroup2D::SetName(const std::string & name)
{
	mName = name;
}

void TKeyFrameAnimationGroup2D::AddAnimation(TKeyFrameAnimation2D* animation)
{
	mAnimations.push_back(animation);
}

TKeyFrameAnimation2D* TKeyFrameAnimationGroup2D::GetAnimation(int idx) const
{
	assert( 0<=idx && idx <(int)mAnimations.size());
	return mAnimations[idx];
}

void TKeyFrameAnimationGroup2D::Render()
{
	for(size_t j=0;j<mAnimations.size();++j)
		mAnimations[j]->Render();	
}

void TKeyFrameAnimationGroup2D::Update(float dt)
{
	for(size_t j=0;j<mAnimations.size();++j)
		mAnimations[j]->Update(dt);	
}

void TKeyFrameAnimationGroup2D::Play(float time)
{
	for(size_t j=0;j<mAnimations.size();++j)
		mAnimations[j]->Play(time);	
}

void TKeyFrameAnimationGroup2D::Stop()
{
	for(size_t j=0;j<mAnimations.size();++j)
		mAnimations[j]->Stop();	
}

void TKeyFrameAnimationGroup2D::SetPlayMode(TKeyFrameAnimation2D::TPlayMode mode)
{
	for(size_t j=0;j<mAnimations.size();++j)
		mAnimations[j]->SetPlayMode(mode);	
}


void TKeyFrameAnimationGroup2D::SetSprite(TSprite* sprite)
{
	for(size_t j=0;j<mAnimations.size();++j)
		mAnimations[j]->SetSprite(sprite);	

}

float TKeyFrameAnimationGroup2D::GetTotalFrameTime() const
{
	float time = 0.0f;
	for(size_t j=0;j<mAnimations.size();++j)
	{
		const float thisTime = mAnimations[j]->GetTotalFrameTime();
		if( thisTime > time )
			time = thisTime;	
	}
	return time;		
}

void TKeyFrameAnimationGroup2D::GotoAndStop(float time)
{
	for(size_t j=0;j<mAnimations.size();++j)
	{
		mAnimations[j]->GotoAndStop(time);
	}
}
	
}