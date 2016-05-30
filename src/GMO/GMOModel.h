#ifndef __GMOModel__H__
#define __GMOModel__H__

#include "Math3D/Matrix4.h"
#include "Math3D/Vector3.h"

#include <string>


typedef struct TGMOHeader
{
    char magic[4];
    int version;    
} TGMOHeader;

class TGMOVertex
{
public:
    float x, y, z;
    float u, v;
};


class TGMOTriangle
{
public:
    TGMOTriangle();
    TGMOTriangle(int v0, int v1, int v2);
    
    int v0, v1, v2;
};

class TGMOMesh
{
public:
    TGMOMesh();
    ~TGMOMesh();
    
    std::string name;
    bool isVisible;
    int vertexCount;
    int triangleCount;
    TGMOVertex* vertices;
    TGMOTriangle* triangles;
};

class TGMONode
{
public:
    TGMONode();
    void AddChild(TGMONode* ch);
    void UpdateTransform(const Matrix4& t = Matrix4::IDENTITY);
    std::string name;
    int parent;
    Matrix4 transform;
    Matrix4 worldTransform;
    
    TGMONode* parentNode;
    std::vector<TGMONode*> childs;
};

class TMeshInstance
{
public:
    TGMONode* node;
    TGMOMesh* mesh;
};


class TGMOFrameData
{
public:
    Matrix4 transform;
};

class TGMOAnimationItem
{
public:
    TGMOAnimationItem();
    std::string target;
    std::vector<TGMOFrameData> data;
    float frameRate;
};


class TGMOModel
{
public:
    TGMOModel();
    ~TGMOModel();
    void Load(const char* file);
    void Render();
    
    void Update(float dt);
    
    int FindMeshByName(const std::string& name) const;
    int FindNodeByName(const std::string& name) const;

    std::vector<TGMONode*> nodes;    
    std::vector<TGMOMesh*> meshes;
    
    std::vector<TMeshInstance*> meshInstances;
    std::vector<TGMOAnimationItem*> animationItems;
    
    
    float playedTime;
};


#endif