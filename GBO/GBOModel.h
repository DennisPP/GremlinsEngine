#ifndef __GBOModel__
#define __GBOModel__

#include "Math3D/Vector3.h"
#include "Math3D/Quaternion.h"

#include "GBOJoint.h"
#include "GBOAnimation.h"

#include <cassert>

#pragma pack(push)
#pragma pack(1)


struct TGBOHeader
{
    char magic[4];
    int version;
};


class TGBOAnimation;

#pragma pack(1)
class TGBOVertex
{
public:
    Vector3 bindPosePosition;
    Vector3 normal;
    Vector3 tangent;
    float u, v;
    int weightStart;
    int weightCount;
};


class TGBOTriangle
{
public:
    TGBOTriangle();
    TGBOTriangle(int v0, int v1, int v2);
    int v0, v1, v2;
};


class TGBOWeight
{
public:
    TGBOWeight();
    TGBOWeight(int jointIndex, float weight);
    int jointIndex;
    float weight;
};


class TGBOMesh
{
public:
    TGBOMesh();
    std::vector<TGBOVertex> vertices;
    std::vector<TGBOTriangle> triangles;
    std::vector<TGBOWeight> weights;
    unsigned int diffuseTexture;
    unsigned int normapMapTexture;
    bool IsVisible() const;
    void SetVisible(bool v);
    
protected:
    bool isVisible;
};


class TGBOModel
{
public:
    TGBOModel();
    void Load(const char* filename);
    bool LoadAscii(const char* filename);
    void RenderJoints();
    void RenderStaticMesh();
    void LoadAnimation(const char* filename);
    bool SetAnimation(TGBOAnimation* animation);
    
    void Update(float dt);
    void UpdateJoints();
    inline int GetJointCount() const;
    inline TGBOJoint* GetJoint(int idx) const;
    
    void Render();
    
    void Eval(const TGBOJointInfo& info, int frame, Vector3& pos, Vector3& orientation);

    
    void LoadSubMesh(FILE* fp);
    void ParseJoints(FILE* fp);
    
    TGBOJoint* GetJointByName(const std::string& name) const;
    Quaternion BuildQuaternion(float qx, float qy, float qz);
    
    std::vector<TGBOMesh*> meshes;
    std::vector<TGBOJoint*> joints;
    
    float playedTime;
    TGBOAnimation* animation;
};

int TGBOModel::GetJointCount() const
{
    return (int)joints.size();
}

TGBOJoint* TGBOModel::GetJoint(int idx) const
{
    assert( idx>=0 && idx<(int)joints.size() );
    return joints[idx];
}



#pragma pack(pop)
#endif