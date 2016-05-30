#include "SlowColladaRenderer.h"

#include "ColladaModel.h"
#include "ColladaNode.h"
#include "ColladaSkin.h"
#include "ColladaMesh.h"
#include "ColladaTriangle.h"
#include "ColladaGeometry.h"
#include "ColladaController.h"
#include "ColladaScene.h"
#include "ColladaVisualScene.h"
#include "ColladaInstanceController.h"
#include "ColladaInstanceGeometry.h"
#include "ColladaInstanceVisualScene.h"
#include "ColladaMorph.h"
#include "ColladaMaterial.h"

#include "ColladaBindMaterial.h"
#include "ColladaInstanceMaterial.h"
#include "ColladaInstanceEffect.h"
#include "ColladaEffect.h"
#include "ColladaProfileCommon.h"
#include "ColladaTechnique.h"
#include "ColladaLambert.h"
#include "ColladaPhong.h"
#include "ColladaNewParam.h"
#include "ColladaSurface.h"
#include "ColladaSampler2D.h"

#include "Core/GremlinsFramework.h"

#include "Graphics/FlatRenderer.h"
#include "Graphics/Graphics.h"

#include <map>




Vector3 OrthogonalVector(const Vector3 & v)
{
    Vector3 ret;
    if( fabs(v.x) <= fabs(v.y) && fabs(v.x) <= fabs( v.z) )
    {
        ret = Vector3( v.x, - v.z, v.y );
    }
    else
        if( fabs(v.y) <= fabs(v.z) && fabs(v.y) <= fabs( v.x) )
        {
            ret = Vector3( - v.z, v.y, v.x );
        }
        else
        {
            ret = Vector3( - v.y, v.x, v.z );
        }
    Vector3 n = v.crossProduct( ret ).normalisedCopy();
    ret = n.crossProduct( v ).normalisedCopy();
    return ret;
}


void RenderCylinder(const Vector3 & pt0, const Vector3 & pt1, const Vector3 & N, float r)
{
    
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    Vector3 p0 = pt0;
    Vector3 p1 = pt1;
    
    Vector3 U = (p1 - p0).normalisedCopy();
    Vector3 V = N.crossProduct(U);
    V.normalise();
    
    const int count =  32;
    float angle0 = 0.0f;
    float angle_inc = 2.0f * Math::PI / count;
    
    // glBegin(GL_QUADS);
    
    for(int k=0;k<count;++k)
    {
        float angle1 = angle0 + angle_inc;
        Vector3 pp0 = p0 + r * (N * cos(angle0) + V * sin(angle0));
        Vector3 pp1 = p1 + r * (N * cos(angle0) + V * sin(angle0));
        Vector3 pp2 = p1 + r * (N * cos(angle1) + V * sin(angle1));
        Vector3 pp3 = p0 + r * (N * cos(angle1) + V * sin(angle1));
        angle0 += angle_inc;
        
        float u0 = 0.0f;
        float v0 = 0.0f;
        float u1 = 1.0f;
        float v1 = 1.0f;
        
        
        
        TFlatRenderer::TVertexData ptx0(pp0.x, pp0.y, pp0.z, u0, v0);
        TFlatRenderer::TVertexData ptx1(pp1.x, pp1.y, pp1.z, u1, v0);
        TFlatRenderer::TVertexData ptx2(pp2.x, pp2.y, pp2.z, u1, v1);
        TFlatRenderer::TVertexData ptx3(pp3.x, pp3.y, pp3.z, u0, v1);
        
        renderer->RenderTriangle(ptx0,ptx1,ptx2);
        renderer->RenderTriangle(ptx0,ptx2,ptx3);
        
        Vector3 n = (pp2 - pp0).crossProduct((pp1-pp0)).normalisedCopy();
        /*ptx0.nx = n.x;
         ptx0.ny = n.y;
         ptx0.nz = n.z;
         
         ptx1.nx = n.x;
         ptx1.ny = n.y;
         ptx1.nz = n.z;
         
         ptx2.nx = n.x;
         ptx2.ny = n.y;
         ptx2.nz = n.z;
         
         ptx3.nx = n.x;
         ptx3.ny = n.y;
         ptx3.nz = n.z;*/
        
        
        //RenderTriangle(ptx0,ptx1,ptx2);
        //RenderTriangle(ptx0,ptx2,ptx3);
        
        
        //glNormal3f(n.x, n.y, n.z);	glVertex3f(pp0.x, pp0.y, pp0.z);
        //glNormal3f(n.x, n.y, n.z);	glVertex3f(pp1.x, pp1.y, pp1.z);
        //glNormal3f(n.x, n.y, n.z);	glVertex3f(pp2.x, pp2.y, pp2.z);
        //glVertex3f(pp3.x, pp3.y, pp3.z);
        
        
        
    }
    //glEnd();
    
    renderer->Flush();
}


TSlowColladaRenderer::TSlowColladaRenderer(TColladaModel* model)
{
    drawJointOnly = true;
    
    colladaModel = model;
    
    const std::string root = TGremlinsFramework::GetInstance()->GetAssetRoot();
    
    TTextureInfo info2 = TTextureManager::GetInstance().GetTexture((root + "media/textures/white.png").c_str());
    whiteTexture = info2.TextureID;
    
}

void TSlowColladaRenderer::SetIsDrawJointOnly(bool value)
{
    drawJointOnly = value;
}

void TSlowColladaRenderer::RenderTriangles()
{
    
    if( !colladaModel )
        return;
    if( colladaModel->geometrys.empty())
        return;
    
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    
    TColladaVisualScene* visualScene = colladaModel->visualScenes.empty() ? NULL : colladaModel->visualScenes[0];
    
    std::map<TColladaNode*, int> mapmap;
    for(size_t c=0;c<colladaModel->controllers.size();++c)
    {
        TColladaController* controller = colladaModel->controllers[c];
        TColladaSkin* skin = controller->skin;
        TColladaMorph* morph = controller->morph;
        TColladaGeometry* geometry = NULL;
        
        
        std::string gid;
        if( skin )
        {
            gid = TColladaParserUtils::SkipStartChar(skin->source);
            geometry = colladaModel->FindGeometryById(gid);
        }
        else if( morph )
        {
            gid = TColladaParserUtils::SkipStartChar(morph->source);
            geometry = colladaModel->FindGeometryById(gid);
        }
        
        if( ! geometry )
            continue;
    
        TColladaVertexWeights* vertexWeights = NULL;
        TColladaSource* weightSource = NULL;
        TColladaSource* jointNameSource = NULL;
    
        if( skin )
        {
            vertexWeights = skin->vertexWeights;
            weightSource = skin->GetWeightSource();
            jointNameSource = skin->GetJointNameSource();
        
            assert( jointNameSource );
        }

    
        TColladaMesh* mesh = geometry->mesh;
    
    
        
    for(int j=0;j<mesh->triangles.size();++j)
    {
        
        TColladaTriangle* triangle = mesh->triangles[j];
        // skip the #
        size_t length = triangle->vertexSource.length();
        std::string s = triangle->vertexSource.substr(1, length-1);
        
        TColladaSource* vertexSource = mesh->FindVertexSourceById(s);
        
        assert( vertexSource );
        length = mesh->texCoordSource.length();
        s = mesh->texCoordSource.substr(1, length-1);
        TColladaSource* textureSource = mesh->FindSourceById(s);
        
        int offset = 0;
        
        int startIndex = offset;
        
       // int uvStartIndex = 0;
        
        
        
        for(int v=0;v<triangle->triangleCount;++v)
        {
            TFlatRenderer::TVertexData data[3];
         
            for(int k=0;k<3;++k)
            {
                int stride = (int)triangle->inputs.size();
                
                int vv0 = k;
                int i0 = (vv0 + startIndex) * stride;
                
                int vertexID = triangle->p[i0]; // x, y, z
                
                float u0 = 0.0f;
                float v0 = 0.0f;
                
               
                if( textureSource )
                {
                    if( triangle->UseTextureIndex() )
                    {
                        int texOffset = 2;
                        int t0 = texOffset + (vv0 + startIndex) * stride;
                    
                        int uu0 = triangle->p[t0] * textureSource->stride;
                    
                        u0 = textureSource->values[uu0];
                        v0 = textureSource->values[uu0+1];
                    }
                    else
                    {
                        int uv0 = triangle->p[i0] * textureSource->stride;
                        u0 = textureSource->values[uv0];
                        v0 = textureSource->values[uv0+1];
                    }
                }
                
                Vector3 p0;
                p0.x = vertexSource->values[3*vertexID];
                p0.y = vertexSource->values[3*vertexID+1];
                p0.z = vertexSource->values[3*vertexID+2];
                
                Vector4 p = Vector4(p0.x, p0.y, p0.z, 1.0f);
                
                Vector4 vvv0(p0.x, p0.y, p0.z, 1.0f);
                
                
                if( skin )
                {
                    p = Vector4::ZERO;
                    int weightCount0 = vertexWeights->vcount[vertexID];
                    int jointOffset = vertexWeights->startJointIndices[vertexID];
                    
                    float sum = 0.0f;
                    //int jointCount = (int)visualScene->jointNodes.size();
                    
                    for(int w=0;w<weightCount0;++w)
                    {
                        int jointIndex = vertexWeights->v[jointOffset];
                       // assert( jointIndex<jointCount );
                        
                        Matrix4 inverseBindPose = skin->invBindPoses[jointIndex];
                        
                        std::string jointName = jointNameSource->nameArray->names[jointIndex];
                        
                        TColladaNode* joint = visualScene->GetJointNodeBySID(jointName);
                        
                        
                        mapmap[joint] = 1;
                        
                        assert( joint );
                        
                        
                        int weightOffset = vertexWeights->v[jointOffset + 1];
                        
                        float weight = weightSource->values[weightOffset];
                        
                        assert( weight>=0.0f && weight<=1.0f);
                        
                        Vector4 vvv0(p0.x, p0.y, p0.z, 1.0f);
                        p += joint->worldTransform * inverseBindPose * vvv0 * weight;
                        
                        sum += weight;
                        
                        jointOffset += 2;
                    }
                }
                
                
                data[k] = TFlatRenderer::TVertexData(p.x, p.y, p.z, u0, v0);
            }
            
            startIndex += 3;// * triangle->inputs.size();
            renderer->RenderTriangle(data[0], data[1], data[2]);
                                     
           // uvStartIndex += nPoly;
        }
        
    }
        
    }
    renderer->Flush();

}


Vector4 TSlowColladaRenderer::CalculateSkinnedPos(TColladaVisualScene* visualScene, TColladaSkin *skin, int vertexID, const Vector4& vertex)
{
    if( !skin )
        return vertex;
    
    TColladaVertexWeights*  vertexWeights = skin->vertexWeights;
    TColladaSource* weightSource = skin->GetWeightSource();
    TColladaSource* jointNameSource = skin->GetJointNameSource();
    
    Vector4 p = Vector4::ZERO;
    int weightCount = vertexWeights->vcount[vertexID];
    int jointOffset = vertexWeights->startJointIndices[vertexID];
    
    float sum = 0.0f;
    //int jointCount = (int)visualScene->jointNodes.size();
    
    
    int weightOffset = vertexWeights->v[jointOffset + 1];
    
    if( weightCount == 0 )
        return vertex;
    for(int w=0;w<weightCount;++w)
    {
        int jointIndex = vertexWeights->v[jointOffset];
        weightOffset = vertexWeights->v[jointOffset + 1];
        
        //assert( jointIndex<jointCount );
        
        Matrix4 inverseBindPose = skin->invBindPoses[jointIndex];
        
        std::string jointName = jointNameSource->nameArray->names[jointIndex];
        
        TColladaNode* joint = visualScene->GetJointNodeBySID(jointName);
        assert( joint );
        
        
        float weight = weightSource->values[weightOffset];
        
        assert( weight>=0.0f && weight<=1.0f);
        
        p += joint->worldTransform * inverseBindPose * skin->bindShapeMatrix * vertex * weight;
        
        sum += weight;
        
        jointOffset += 2;
    }
    
    assert( sum>=0.9f && sum<=1.01f );
    

    return p;
}

void TSlowColladaRenderer::RenderPolyList(TColladaVisualScene* visualScene, TColladaPolyList* polylist, TColladaMesh* mesh, TColladaSkin* skin, const Matrix4& transform)
{
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    TColladaPolyList* list = polylist;
    list->IsValid();
    
    // skip the #
    size_t length = list->vertexSource.length();
    std::string s = list->vertexSource.substr(1, length-1);
    
    TColladaSource* vs = mesh->FindVertexSourceById(s);
    
    TColladaSource* textureSource = NULL;
    assert( vs );
    if( !list->texCoordSource.empty() )
    {
        length = list->texCoordSource.length();
        s = list->texCoordSource.substr(1, length-1);
        textureSource = mesh->FindSourceById(s);
    }
    
    int offset = 0;
    int startIndex = offset;
    
    for(int v=0;v<list->vcount.size();++v)
    {
        int vertexCount = list->vcount[v];
        assert( vertexCount>=3 );
        
        TFlatRenderer::TVertexData data[3];
        
        // divide the polygon to triangles
        for(int k=2;k<vertexCount;++k)
        {
            int stride = list->stride;
            
            int vv[3];
            vv[0] = 0;
            vv[1] = k - 1;
            vv[2] = k;
            
            for(int vi=0;vi<3;++vi)
            {
                int i = (vv[vi]) * stride + startIndex;
                int vertexID = list->p[i];
                int ii = vertexID * 3; // x, y, z
                Vector3 p0(vs->values[ii], vs->values[1+ii], vs->values[2+ii]);
                
                float u0 = 0.0f;
                float v0 = 0.0f;
                
                if( textureSource )
                {
                    int t0 = list->textureOffset + (vv[vi]) * stride + startIndex;
                    int uu0 = list->p[t0] * 2; // 2 = u + v
                    u0 = textureSource->values[uu0];
                    v0 = textureSource->values[uu0+1];
                }
                
                Vector4 p = Vector4(p0.x, p0.y, p0.z, 1.0f);                
                if( skin )
                {
                    Vector4 vertex(p0.x, p0.y, p0.z, 1.0f);
                    p = CalculateSkinnedPos(visualScene, skin, vertexID, vertex);
                }
                else // HACK non-skinned animation????
                {
                    p = transform * p;
                }
                p0 = Vector3(p.x, p.y, p.z);
                
                data[vi] = TFlatRenderer::TVertexData(p0.x, p0.y, p0.z, u0, v0);
            }
            renderer->RenderTriangle(data[0], data[1], data[2]);
        }
        
        startIndex += vertexCount * list->stride; // vertex, normal , texture
        
        
    }
}


TColladaImage* TSlowColladaRenderer::FindImageFromBindMaterial(TColladaBindMaterial* bindMaterial)
{
    TColladaInstanceMaterial* instanceMaterial = bindMaterial->instanceMaterial;
    if( ! instanceMaterial )
        return NULL;
    
    std::string target = instanceMaterial->target;
    target = target.substr(1); // skip the #
    TColladaMaterial* material = colladaModel->FindMaterialById(target);
    if( ! material )
        return NULL;

    for(int k=0;k<(int)material->instanceEffects.size();++k)
    {
        TColladaInstanceEffect* instanceEffect = material->instanceEffects[k];
        std::string effectUrl = instanceEffect->url;
        effectUrl = effectUrl.substr(1); // skip the #
        
        TColladaEffect* effect = colladaModel->FindEffectById(effectUrl);
        TColladaProfileCommon* profileCommon  = effect->profileCommon;
        
        if( profileCommon->technique )
        {
            std::string imageId;
            TColladaLambert* lambert = effect->profileCommon->technique->lambert;
            if( lambert )
            {
                std::string initFrom = "";
                for(int p=0;p<(int)profileCommon->newParams.size();++p)
                {
                    TColladaNewParam* newParam = profileCommon->newParams[p];
                    if( newParam->sid == profileCommon->technique->lambert->diffuseTexture )
                        initFrom = newParam->sampler2D->source;
                }
                for(int p=0;p<(int)profileCommon->newParams.size();++p)
                {
                    TColladaNewParam* newParam = profileCommon->newParams[p];
                    if( newParam->sid == initFrom )
                    {
                        imageId = newParam->surface->initFrom;
                    }
                }
            }
            else if( effect->profileCommon->technique->phong )
            {
                imageId = effect->profileCommon->technique->phong->diffuseTexture;
            }
            
            TColladaImage* image = colladaModel->FindImageById(imageId);
            if( image )
                return image;
        }
    }
    return NULL;
}


void TSlowColladaRenderer::RenderPolyLists()
{
    if( ! colladaModel || ! colladaModel->scene )
        return;
    
    
    
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    
    
    std::map<TColladaMesh*, int> rendererMeshs;
    
    
    std::map<TColladaNode* , int> mapmap;
    
    TColladaVisualScene* visualScene = colladaModel->visualScenes[0];
    
    int treeCount = 0;
    for(int j=0;j<(int)visualScene->rootNodes.size();++j)
    {
        assert( visualScene->rootNodes[j]->IsRootNode() );
        visualScene->rootNodes[j]->UpdateTransform();
        ++ treeCount;
    }
    
    std::vector<TColladaInstanceController*> instanceControllers;
    colladaModel->CollectInstanceController(instanceControllers);
    std::vector<TColladaInstanceGeometry*> instanceGeometrys;
    
    colladaModel->CollectInstanceGeometry(instanceGeometrys);
    
    for(size_t m=0;m<instanceGeometrys.size();++m)
    {
        std::string geometryId = instanceGeometrys[m]->url;
        Matrix4 transform = instanceGeometrys[m]->GetParentNode()->worldTransform;
        
        TColladaImage* image = FindImageFromBindMaterial(instanceGeometrys[m]->bindMaterial);
        std::map<TColladaImage*, unsigned int>::iterator it = colladaModel->textures.find(image);
        if( it != colladaModel->textures.end() )
        {
            renderer->BindTexture(it->second);
        }

        
        
        geometryId = geometryId.substr(1); // skip the #
        TColladaGeometry* geometry = colladaModel->FindGeometryById(geometryId);
        TColladaMesh* mesh = geometry->mesh;
        if( ! mesh )
            continue;
        for(int j=0;j<mesh->polylists.size();++j)
        {
            TColladaPolyList* list = mesh->polylists[j];
            list->IsValid();
            RenderPolyList(visualScene, list, mesh, NULL, transform);
        }
    }
    
    
    for(size_t c=0;c<(int)instanceControllers.size();++c)
    {
        std::string controllerID = instanceControllers[c]->url;
        controllerID = controllerID.substr(1); // skip the #
        TColladaController* controller = colladaModel->FindControllerById(controllerID);
        assert( controller );
        
        if( instanceControllers[c]->skeletons.empty())
            continue;
        
        assert( instanceControllers[c]->skeletons.size() > 0 );
        std::string skeleton = instanceControllers[c]->skeletons[0];
        skeleton = skeleton.substr(1); // skip the #
        
        TColladaNode* rootNode = visualScene->FindNodeById(skeleton);
        assert( rootNode );
        
        
        TColladaImage* image = FindImageFromBindMaterial(instanceControllers[c]->bindMaterial);
        std::map<TColladaImage*, unsigned int>::iterator it = colladaModel->textures.find(image);
        if( it != colladaModel->textures.end() )
        {
            renderer->BindTexture(it->second);
        }        
        
        TColladaSkin* skin = controller->skin;
        TColladaMorph* morph = controller->morph;
        
        TColladaSource* morphTargetSource = NULL;
        TColladaSource* morphWeightSource = NULL;
        
        TColladaGeometry* geometry = NULL;
        if( skin )
        {
            std::string gid = TColladaParserUtils::SkipStartChar(skin->source);
            geometry = colladaModel->FindGeometryById(gid);
        }
        if( morph )
        {
            
            morphTargetSource = morph->GetMorphTargetSource();
            morphWeightSource = morph->GetMorphWeightSource();
            
            assert( morphTargetSource->idRefArray && !morphTargetSource->idRefArray->ids.empty());
            std::string targetGeometryId = morphTargetSource->idRefArray->ids[0];
            // std::string gid = TColladaParserUtils::SkipStartChar(morph->source);
            geometry = colladaModel->FindGeometryById(targetGeometryId);
        }
        
        if( ! geometry )
            continue;
        
        TColladaVertexWeights* vertexWeights = NULL;
        TColladaSource* weightSource = NULL;
        TColladaSource* jointNameSource = NULL;
        
        
        if( skin )
        {
            vertexWeights = skin->vertexWeights;
            weightSource = skin->GetWeightSource();
            jointNameSource = skin->GetJointNameSource();
        }
        
        // polygon lists
#if 1
        
        TColladaMesh* mesh = geometry->mesh;
        
        if( rendererMeshs.find(mesh) == rendererMeshs.end() )
            rendererMeshs[mesh] = 1;
        else
            rendererMeshs[mesh] ++;
        
        
        for(int j=0;j<mesh->polylists.size();++j)
        {
            TColladaPolyList* list = mesh->polylists[j];
            list->IsValid();
            RenderPolyList(visualScene, list, mesh, skin, Matrix4::IDENTITY);
        }
    }
    
    renderer->Flush();
    
#endif
    
}


void TSlowColladaRenderer::RenderStaticMesh()
{
    if( ! colladaModel || ! colladaModel->scene )
        return;
    
    
    
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    
    
    std::map<TColladaMesh*, int> rendererMeshs;
    
    //TColladaScene* scene = colladaModel->scene;
    
    TColladaVisualScene* visualScene = colladaModel->visualScenes[0];
    for(int j=0;j<(int)visualScene->rootNodes.size();++j)
    {
        assert( visualScene->rootNodes[j]->IsRootNode() );
            visualScene->rootNodes[j]->UpdateTransform();
    }
    for(size_t c=0;c<colladaModel->controllers.size();++c)
    {
        TColladaController* controller = colladaModel->controllers[c];
        
        if( ! controller )
            continue;
        
        
        TColladaSkin* skin = controller->skin;
        TColladaMorph* morph = controller->morph;
        
        TColladaSource* morphTargetSource = NULL;
        TColladaSource* morphWeightSource = NULL;
        
        TColladaGeometry* geometry = NULL;
        if( skin )
        {
            std::string gid = TColladaParserUtils::SkipStartChar(skin->source);
            geometry = colladaModel->FindGeometryById(gid);
        }
        if( morph )
        {
            
            morphTargetSource = morph->GetMorphTargetSource();
            morphWeightSource = morph->GetMorphWeightSource();
            
            
            
            assert( morphTargetSource->idRefArray && !morphTargetSource->idRefArray->ids.empty());
            std::string targetGeometryId = morphTargetSource->idRefArray->ids[0];
            // std::string gid = TColladaParserUtils::SkipStartChar(morph->source);
            geometry = colladaModel->FindGeometryById(targetGeometryId);
            
            
        }
        
        if( ! geometry )
            continue;
        
        
        
        
        TColladaVertexWeights* vertexWeights = NULL;
        TColladaSource* weightSource = NULL;
        TColladaSource* jointNameSource = NULL;
        
        
        if( skin )
        {
            vertexWeights = skin->vertexWeights;
            weightSource = skin->GetWeightSource();
            jointNameSource = skin->GetJointNameSource();
            
            
            
        }

        
        TColladaMesh* mesh = geometry->mesh;
        
        if( rendererMeshs.find(mesh) == rendererMeshs.end() )
            rendererMeshs[mesh] = 1;
        else
            rendererMeshs[mesh] ++;
        
        for(int j=0;j<mesh->polylists.size();++j)
        {
            TColladaPolyList* list = mesh->polylists[j];
            list->IsValid();
            // skip the #
            size_t length = list->vertexSource.length();
            std::string s = list->vertexSource.substr(1, length-1);
            
            TColladaSource* vs = mesh->FindVertexSourceById(s);
            
            assert( vs );
            length = list->texCoordSource.length();
            s = list->texCoordSource.substr(1, length-1);
            TColladaSource* textureSource = mesh->FindSourceById(s);

            
            std::vector<TTriangleInfo> triangles;
            list->Triangulate(triangles);
            
            for(int t=0;t<(int)triangles.size();++t)
            {
                const TTriangleInfo& info = triangles[t];
                TFlatRenderer::TVertexData data[3];
                for(int k=0;k<3;++k)
                {
                    data[k].x = vs->values[0+info.vertex[k]];
                    data[k].y = vs->values[1+info.vertex[k]];
                    data[k].z = vs->values[2+info.vertex[k]];
                    
                    data[k].u = textureSource->values[0+info.texCoord[k]];
                    data[k].v = textureSource->values[1+info.texCoord[k]];
                }
                renderer->RenderTriangle(data[0], data[1], data[2]);
            }
        }
    }
    renderer->Flush();

}

void TSlowColladaRenderer::RenderSkinnedMesh()
{
   
    
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
   
    
    
    RenderTriangles();
    RenderPolyLists();
 
}

void TSlowColladaRenderer::RenderJoints()
{
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();    
    
#if 1
    renderer->BindTexture(whiteTexture);
    int rootCount = 0;
    
 
    for(size_t vs=0;vs<colladaModel->visualScenes.size();++vs)
    {
        TColladaVisualScene* scene = colladaModel->visualScenes[vs];
    
        for(size_t j=0;j<scene->rootNodes.size();++j)
        {
            TColladaNode* rootNode = scene->rootNodes[j];
            assert( rootNode->IsRootNode() );

            rootNode->UpdateTransform();
        
            std::vector<TColladaNode*> treeNodes;
            scene->CollectAllNodesInTreeLevelOrder(rootNode, treeNodes);
            
            for(int n=0;n<(int)treeNodes.size();++n)
            {
                TColladaNode* node = treeNodes[n];
                TColladaNode* parentNode = node->parentNode;
            
                if( node->type != std::string("JOINT") )
                    continue;
            
                if( ! parentNode || parentNode->type!=std::string("JOINT") )
                    continue;
            
            
                //Matrix4 transform = node->transform;
                //assert( node->parentId < (int)j );
     
                //Vector3 position = transform.getTrans();
            
                Vector3 start = parentNode->worldTransform.getTrans();
                Vector3 end = node->worldTransform.getTrans();
            
                Vector3 N = OrthogonalVector((start - end).normalisedCopy()).normalisedCopy();
            
                RenderCylinder(start, end, N, 1.0f);
                
            }
        }
        
    }
    renderer->Flush();
#endif

}

void TSlowColladaRenderer::RenderBindPoses()
{
    
#if 1
    
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    TColladaSkin* skin = colladaModel->controllers[0]->skin;
    renderer->BindTexture(whiteTexture);
    for(int j=0;j<(int)skin->invBindPoses.size();++j)
    {
        Matrix4 bindPose = skin->invBindPoses[j].inverse();
        
        Vector3 t = bindPose.getTrans();
        
        float x = t.x;
        float y = t.y;
        float z = t.z;
        float s = 1.0f;
        Vector3 v(x*s, y*s, z*s);
        
        Vector3 U = 5.0f*Vector3::UNIT_X;
        Vector3 V = 5.0f*Vector3::UNIT_Y;
        
        Vector3 p0 = v - U - V;
        Vector3 p1 = v + U - V;
        Vector3 p2 = v + U + V;
        Vector3 p3 = v - U + V;
        
        TFlatRenderer::TVertexData pt0(p0.x, p0.y, p0.z, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt1(p1.x, p1.y, p1.z, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt2(p2.x, p2.y, p2.z, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt3(p3.x, p3.y, p3.z, 0.0f, 0.0f);
        
        
        renderer->RenderTriangle(pt0, pt1, pt2);
        renderer->RenderTriangle(pt0, pt2, pt3);
        
    }
    renderer->Flush();
#endif
    
}


void TSlowColladaRenderer::Update(float dt)
{

}

void TSlowColladaRenderer::Render()
{
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    for(size_t s=0;s<colladaModel->visualScenes.size();++s)
    {
        TColladaVisualScene* scene = colladaModel->visualScenes[s];
        for(size_t n=0;n<scene->rootNodes.size();++n)
        {
            TColladaNode* node = scene->rootNodes[n];
            assert( node->IsRootNode() );
            node->UpdateTransform();
        }
    }
    
    if( drawJointOnly )
        RenderJoints();
    else
        RenderSkinnedMesh();

}