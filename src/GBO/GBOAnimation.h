#ifndef __GBOAnimation__
#define __GBOAnimation__

#include <vector>
#include <string>

#include "Math3D/Matrix4.h"
#include "Math3D/Vector3.h"
#include "Math3D/Quaternion.h"

struct TGBAHeader
{
    char magic[4];
    int version;    
};

class TKeyFrameData
{
public:
    std::vector<float> data;
};

class TGBOJointInfo
{
public:
    TGBOJointInfo();
    TGBOJointInfo(const std::string& name, int parent);
    ~TGBOJointInfo();
    
protected:
public:
    Vector3 position;
    Vector3 orientation;
    std::string name;
    int parentId;
};


class TGBAFrameData
{
public:
    Matrix4 transform;
};

class TAnimationItem
{
public:
    std::string target;
    std::vector<TGBAFrameData> data;
};

class TGBOAnimation
{
public:
    TGBOAnimation();
    ~TGBOAnimation();
    void Load(const char* filename);
    bool LoadAscii(const char* filename);
    
    void ParseHierarchy(FILE* fp);
    Quaternion BuildQuaternion(float qx, float qy, float qz);
    
    std::vector<TGBOJointInfo> jointInfos;
    std::vector<TAnimationItem> animationItems;
    int numJoints;
    float frameRate;
    int numAnimatedComponents;
    std::string target;
};


#endif