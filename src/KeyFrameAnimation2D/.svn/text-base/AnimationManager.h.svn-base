#ifndef _FireEngine_AnimationManager_h_
#define _FireEngine_AnimationManager_h_

#include <vector>
#include <string>

namespace KeyFrameAnimation2D
{

class TKeyFrameAnimationGroup2D;

class TAnimationItem
{
	friend class TAnimationManager;
private:
	TAnimationItem(const std::string& fileName, const std::string& animationName, TKeyFrameAnimationGroup2D* animation);
	~TAnimationItem();
	const std::string mFileName;
	const std::string mAnimationGroupName;

	TKeyFrameAnimationGroup2D* mAnimationGroup;
};

inline TAnimationItem::TAnimationItem(const std::string& fileName, const std::string& animationName, TKeyFrameAnimationGroup2D* animation)
:	mFileName(fileName)
,	mAnimationGroupName(animationName)
,	mAnimationGroup(animation)
{
}


	

class TAnimationManager
{
public:
	TAnimationManager();
	~TAnimationManager();

	TKeyFrameAnimationGroup2D* GetAnimation(const std::string& fileName, const std::string& animationName);
	
	std::vector<TAnimationItem*> mAnimationGroups;
};

}

#endif