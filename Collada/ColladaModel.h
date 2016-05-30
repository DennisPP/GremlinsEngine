#ifndef __ColladaModel__
#define __ColladaModel__


#include "Math3D/Vector3.h"
#include "Math3D/Vector4.h"

#include "Math3D/Matrix4.h"


#include "ColladaBase.h"
#include "ColladaSource.h"
#include "ColladaAnimation.h"

#include "Core/SmartPointer.h"



#include <map>
#include <vector>
#include <string>


// Reference:https://www.khronos.org/files/collada_spec_1_4.pdf

/*
Definitions related to skinning in COLLADA:
• Bind shape (or base mesh): The vertices of the mesh referred to by the source attribute of the
<skin> element.
• Joints: Nodes specified by sid in the <source> referred to by the <input> (unshared) element
with semantic="JOINT". The sid s are typically stored in a <Name_array> where one name
represents one sid (node). Upon instantiation of a skin controller, the <skeleton> elements define
where to start the sid lookup. The joint matrices can be obtained at runtime from these nodes.
• Weights: Values in the <source> referred to by the <input> (unshared) element with
semantic="WEIGHT". Typically stored in a <float_array> and taken one floating-point
number at a time. The <vertex_weights> element describes the combination of joints and
weights used by the skin.
• Inverse bind matrix: Values in the <source> element referred to by the <input> (unshared)
element with semantic="INV_BIND_MATRIX". Typically stored in a <float_array> taken 16
floating-point numbers at a time. The <joints> element associates the joints to their inverse bind
matrices.
• Bind shape matrix: A single matrix that represents the transform of the bind shape before skinning.
*/
/*
Here is an example of a more complete <vertex_weights> element. Note that the <vcount> element
says that the first vertex has 3 bones, the second has 2, etc. Also, the <v> element says that the first
vertex is weighted with weights[0] towards the bind shape, weights[1] towards bone 0, and
weights[2] towards bone 1:
<skin>
<source id="joints"/>
<source id="weights"/>
<vertex_weights count="4">
<input semantic="JOINT" source="#joints"/>
<input semantic="WEIGHT" source="#weights"/>
<vcount>3 2 2 3</vcount>
<v>
-1 0 0 1 1 2
-1 3 1 4
-1 3 2 4
-1 0 3 1 2 2
</v>
*/

class TiXmlElement;

class TColladaImage;
class TColladaInput;
class TColladaSkin;
class TColladaNode;
class TColladaMesh;
class TColladaGeometry;
class TColladaMaterial;
class TColladaAsset;
class TColladaController;
class TColladaVisualScene;
class TColladaScene;
class TColladaEffect;
class TColladaInstanceController;
class TColladaInstanceGeometry;

class TColladaModel
{
public:
    TColladaModel();
    ~TColladaModel();
    void LoadModel(const char* filename);
    void Update(float dt);
    static std::string SkipStartChar(const std::string& s);
    
    
    void ParseNodeTransform(TColladaNode* node, TiXmlElement* nodeElement);
    TColladaNode* ParseNode(TiXmlElement* nodeElement, int parentId=-1);
    
    
    TColladaSource* FindSourceById(const std::string& id);
    TColladaSource* FindVertexSourceById(const std::string& id);
    
    TColladaAsset* GetAsset() const;
    
    TColladaGeometry* FindGeometryById(const std::string& id) const;
    
    TColladaVisualScene* FindVisualSceneById(const std::string& id) const;
    TColladaController* FindControllerById(const std::string& id) const;
    TColladaMaterial* FindMaterialById(const std::string& id) const;
    TColladaEffect* FindEffectById(const std::string& id) const;
    TColladaImage* FindImageById(const std::string& id) const;
    
    float GetMaxAnimationTime() const;
    
    
    
    bool GetInverseBindPoseByJointName(const std::string& name, Matrix4& invBindPose);
    
    
    void CollectInstanceController(std::vector<TColladaInstanceController*>& controllers) const;
    void CollectInstanceGeometry(std::vector<TColladaInstanceGeometry*>& gemotrys) const;
    void CollectAnimations(std::vector<TColladaAnimation*>& animations) const;
    
  protected:
    
    float mPlayedTime;
    
public:
    TColladaScene* scene; // 0 or 1
    TColladaAsset* asset;
    std::vector<TColladaNode*> jointNodes;
    std::vector<TColladaVisualScene*> visualScenes;
    std::vector<TColladaController*> controllers;
    std::vector<TColladaSource*> sources;
    std::vector<TColladaGeometry*> geometrys;
    std::vector<TColladaMaterial*> materials;
    std::vector<TColladaImage*> images;
    std::vector<TColladaEffect*> effects;
    
    std::map<TColladaImage*, unsigned int> textures;
    std::string loadingPath;
    
protected:
public:
    std::vector<TColladaAnimation*> animations; // this only store root level animations
    
    
    
    
    // implementation
    int nodeId;    
};


#endif