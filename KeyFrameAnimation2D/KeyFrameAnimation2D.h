#ifndef __FireEngine__KeyFrameAnimation2D__h__
#define __FireEngine__KeyFrameAnimation2D__h__

#include <vector>
#include <string>

#include "../Graphics/Color.h"

class TSprite;

namespace KeyFrameAnimation2D
{

struct TKeyFrame
{
	TKeyFrame();
	TKeyFrame(float time, float x, float y, float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, const TColor& color = TColor::White);

	float time;
	float x, y;
	float rotation;
	float scaleX;
	float scaleY;
	TColor color;
};

class TKeyFrameAnimation2D
{
public:
	enum TPlayMode
	{
		PLAY_LOOPED = 0,
		PLAY_ONCE,
		PLAY_ONCE_AND_STOP,
	};

public:
	TKeyFrameAnimation2D();
	~TKeyFrameAnimation2D();

	void SetSprite(TSprite* sprite);
	bool IsPlaying() const;
	void Play(float time = 0.0f);
	void Stop();	
	TPlayMode GetPlayMode() const;
	void SetPlayMode(TPlayMode mode);

	void	Update(float dt);
	void	AddKeyFrame(const TKeyFrame & frame);
	void	Render();
	float GetTotalFrameTime() const;
	float GetPlayedTime() const;
	void GotoAndStop(float time);
	const TKeyFrame& GetCurrentKeyFrame() const;

private:
	std::vector<TKeyFrame> mKeyFrames;
	// interpolated frame;
	TKeyFrame mCurrentFrame;


	float mPlayedTime;
	bool	mIsPlaying;
	TPlayMode mPlayMode;
	

	TSprite* mSprite;
};

inline float TKeyFrameAnimation2D::GetTotalFrameTime() const
{
	if( mKeyFrames.empty() )
		return 0.0f;
	return mKeyFrames[mKeyFrames.size() - 1].time;
}


inline float TKeyFrameAnimation2D::GetPlayedTime() const
{
	return mPlayedTime;
}


class TKeyFrameAnimationGroup2D
{
public:
	TKeyFrameAnimationGroup2D();
	~TKeyFrameAnimationGroup2D();

	void AddAnimation(TKeyFrameAnimation2D* animataion);
	TKeyFrameAnimation2D* GetAnimation(int idx) const;
	void Render();
	void Update(float dt);
	void Play(float time = 0.0f);
	void Stop();
	const std::string & GetName() const;
	void SetName(const std::string & name);
	void SetPlayMode(TKeyFrameAnimation2D::TPlayMode mode);
	void SetSprite(TSprite* sprite);

	float GetTotalFrameTime() const;
	void GotoAndStop(float time);

	
protected:
	std::string mName;
	std::vector<TKeyFrameAnimation2D*>	mAnimations;
};

inline const TKeyFrame& TKeyFrameAnimation2D::GetCurrentKeyFrame() const
{
	return mCurrentFrame;
}

}

#endif