#ifndef __SlowColladaRenderer__
#define __SlowColladaRenderer__


#include "Math3D/Vector3.h"
#include "Math3D/Vector4.h"

#include "Math3D/Matrix4.h"

class TColladaModel;
class TColladaMesh;
class TColladaSkin;
class TColladaPolyList;
class TColladaVisualScene;
class TColladaBindMaterial;
class TColladaImage;


Vector3 OrthogonalVector(const Vector3 & v);
void RenderCylinder(const Vector3 & pt0, const Vector3 & pt1, const Vector3 & N, float r);

class TSlowColladaRenderer
{
public:
    TSlowColladaRenderer(TColladaModel* model);
    void Update(float dt);
    void Render();
    void RenderJoints();
    void RenderSkinnedMesh();
    void RenderBindPoses();
    void RenderPositions();
    void SetIsDrawJointOnly(bool value);
    
    
    
    Vector4 CalculateSkinnedPos(TColladaVisualScene* scene, TColladaSkin* skin, int vertexID, const Vector4& vertex);
    void RenderTriangles();
    void RenderPolyLists();
    void RenderPolyList(TColladaVisualScene* scene, TColladaPolyList* polylist, TColladaMesh* mesh, TColladaSkin* skin
                        , const Matrix4& transform);
    
    void RenderStaticMesh();
    
    
    TColladaImage* FindImageFromBindMaterial(TColladaBindMaterial* bindMaterial);
    

    TColladaModel* colladaModel;
    
    unsigned int whiteTexture;
    
    bool drawJointOnly;
    
    
    
    
};


#endif