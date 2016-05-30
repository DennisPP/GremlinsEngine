#ifndef _MD5MODEL_H_
#define _MD5MODEL_H_


#include <stdio.h>
#include <vector>

#include "Math3D/Vector3.h"
#include "Math3D/Quaternion.h"



// Note:
// http://tfc.duke.free.fr/coding/md5-specs-en.html
// 1. joints positions in .mesh file are already in object space, which is the bind pose of the mesh
// 2. joints animation data in .anim file are in their parent space
// 3. weights position are relative to the joints local space



class TMD5Joint;
class TMD5Animation;
class TMD5JointInfo;

class TMD5Vertex
{
public:
	
	float u, v;
	int startWeight;
	int weightCount;
    
    Vector3 normal;
    Vector3 tangent;
    // vertex position in the bind pose
    Vector3 bindPoseVertexPosition;
};

class TMD5Triangle
{
public:
    TMD5Triangle(){};
	TMD5Triangle(int v0, int v1, int v2);

	int v0, v1, v2;
};

class TMD5Weight
{
public:
	int mJointIndex;
	float mBias;
	Vector3 mPosition; 
};


class TMD5Mesh
{
public:
    TMD5Mesh();
    std::vector<TMD5Vertex> mVertices;
    std::vector<TMD5Triangle> mTriangles;
    std::vector<TMD5Weight> mWegihts;
    void SetVisible(bool v);
    bool IsVisible() const;
    
    bool isVisible;
};



class TMD5Model
{
    friend class TSkinnedMeshRenderer;
    friend class TMD5Renderer;
public:
	TMD5Model();
	~TMD5Model();

	bool LoadMesh(const char* path);
	void SetAnimation(TMD5Animation* animation);
 
    void BlendToAnimaton(TMD5Animation* animation);
    
    
	TMD5Joint* GetRootJoint() const;
    
    TMD5Joint* GetJoint(int idx) const;
    int GetJointCount() const;


	void Render();
    void RenderBindPoseSkeleton();
    void RenderBindPoseVertexPositions();
   void Update(float dt);

    void UpdateJoints();
    
    int GetMeshCount() const;
    TMD5Mesh* GetMesh(int idx);
    
protected:
    void ConvertCoordinateSpace();
    
	void LoadSubMesh(const char* path, FILE* fp);
	void ParseJoints(FILE* fp);
    void PrepareBindPoseVertices();
    void Eval(const TMD5JointInfo& info, int frame, Vector3& pos, Vector3& orientation);
    
    

	static Quaternion BuildQuaternion(float x, float y, float z);
	std::vector<TMD5Joint*> mJoints;
	
    std::vector<TMD5Mesh*> meshes;


	// only pointer
	TMD5Animation* mAnimation;
    TMD5Animation* mBlendAnimation;    
    
    float startBlendTime;
    

	float mPlayedTime;
};


#endif