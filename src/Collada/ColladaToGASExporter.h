#ifndef __ColladaToGASExporter__
#define __ColladaToGASExporter__

// Issues
// 1.   Some collada file will list the joints NOT in the parent first order
//      MD5 require joints has to be listed in parent first order, the exporter already take this
//      into account
// 2.   UV in collada use a different list separate from the position, while MD5 bake the UV to the Vertex
#include "ColladaExporter.h"

#include "Math3D/Matrix4.h"

#include <vector>
#include <map>

class TColladaMesh;
class TColladaNode;
class TColladaSkin;

class TColladaAnimation;

class TVertexItem
{
public:
    TVertexItem();
    
    
    float x, y, z, u, v;
    int weightStart;
    int weightCount;
    int vertex;
    int uv;
};


class TAnimTransform
{
public:
    typedef enum TAnimTransformType
    {
        PositionX   = 1,
        PositionY   = 2,
        PositionZ   = 4,
        RotationX   = 8,
        RotationY   = 16,
        RotationZ   = 32,
        Matrix      = 64,
    } TAnimTransformType;
    
    TAnimTransformType type;
    
    union value
    {
        float single;
        float matrix[16];
    } value;
};

class TColladaToGASExporter : public TColladaExporter
{
public:
    virtual bool ExportMesh(TColladaModel* model, const char* path);
    virtual bool ExportAnimation(TColladaModel* model, const char* path);
    
    
    void ExportGBO(TColladaModel* model,const char* filename);
    void ExportGBA(TColladaModel* model,const char* filename);
    void ExportGBAscill(TColladaModel* model, const char* filename);
    void ExportStaticMesh(TColladaModel* model, const char* path);
    
    void ExportGMO(TColladaModel* model, const char* path);
    
    
protected:
    void GetAnimationTransfrom(TColladaAnimation* animation, std::vector<Matrix4>& transforms);
    void GetAnimationTransform(TColladaAnimation* animation, int frame, Matrix4& transform);
    void GetCombinedTransform(const std::vector<TColladaAnimation*>& animations, int frame, Matrix4& transform);
    
    
    int UpdateVertexItem(std::vector<TVertexItem>& vertexItems, TVertexItem& item) const;
    
    
    bool IsAllAnimationTargetDifferentNode(TColladaModel* model) const;
    

    
    void PrepareAnimations(TColladaModel* model, int frame);
    
    bool CanBakeUVToVertexWithoutUsingNewVertex(TColladaMesh* mesh) const;
    
    
    int GetJointInNameID(TColladaNode* joint, TColladaSkin* skin) const;
    
    
protected:
    
    std::map<TColladaNode*, std::vector<TAnimTransform> > nodeTransforms;
};


#endif