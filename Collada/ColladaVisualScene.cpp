#include "ColladaVisualScene.h"

#include "ColladaAsset.h"
#include "ColladaNode.h"
#include "ColladaInstanceController.h"
#include "ColladaInstanceGeometry.h"

#include "tinyxml.h"

#include <queue>

TColladaVisualScene::TColladaVisualScene()
{
    asset = NULL;
}

TColladaVisualScene::~TColladaVisualScene()
{
    if( asset )
    {
        delete asset;
        asset = NULL;
    }
}

TColladaBase* TColladaVisualScene::Parse(TiXmlElement* xml)
{
    const char* id = xml->Attribute("id");
    this->id = id ? std::string(id) : "";
    
    
    TiXmlElement* nodeElement = xml->FirstChildElement("node");
    for(;nodeElement;nodeElement=nodeElement->NextSiblingElement("node"))
    {
        nodeId = -1;
        TColladaNode* node = ParseNode(nodeElement, -1);
        node->Parse(nodeElement);
        rootNodes.push_back(node);
    }
    
    
    for(int j=0;j<(int)rootNodes.size();++j)
    {
        std::vector<TColladaNode*> treeNodes;
        CollectAllNodesInTreeLevelOrder(rootNodes[j], treeNodes);
        for(int k=0;k<(int)treeNodes.size();++k)
        {
            allNodes.push_back(treeNodes[k]);
            if( treeNodes[k]->IsJointNode() )
            {
                jointNodes.push_back(treeNodes[k]);
            }
        }
    }   
    
    
    return this;
}

int TColladaVisualScene::CollectAllNodesInTreeLevelOrder(TColladaNode* root, std::vector<TColladaNode*>& tree) const
{
    std::queue<TColladaNode*> q;
    q.push(root);
    while( !q.empty() )
    {
        TColladaNode* node = q.front();
        q.pop();
        tree.push_back(node);
        for(int j=0;j<(int)node->childs.size();++j)
            q.push(node->childs[j]);
    }
    return (int)tree.size();
}



TColladaNode* TColladaVisualScene::ParseNode(TiXmlElement* nodeElement, int parentId)
{
    const char* id = nodeElement->Attribute("id");
    const char* name = nodeElement->Attribute("name");
    const char* type = nodeElement->Attribute("type");
    const char* sid = nodeElement->Attribute("sid");
    
    printf("NODE:%s %s %s\n", id, name, type);
    TColladaNode* node = new TColladaNode();
    if( id )
        node->id = std::string(id);
    if( type )
        node->type = type;
    if( sid )
        node->sid = std::string(sid);
    
    node->nodeId = nodeId;
    node->parentId = parentId;
    node->name = std::string(name);
    ++ nodeId;
    
    
    
    ParseNodeTransform(node, nodeElement);
    
    
    
    TiXmlElement* instanceControllerElement = nodeElement->FirstChildElement("instance_controller");
    for(;instanceControllerElement;instanceControllerElement=instanceControllerElement->NextSiblingElement("instance_controller"))
    {
        TColladaInstanceController* controller = new TColladaInstanceController();
        controller->Parse(instanceControllerElement);
        node->instanceControllers.push_back(controller);
    }
    
    TiXmlElement* instanceGeometryElement = nodeElement->FirstChildElement("instance_geometry");
    for(;instanceGeometryElement;instanceGeometryElement=instanceGeometryElement->NextSiblingElement("instance_geometry"))
    {
        TColladaInstanceGeometry* geometry = new TColladaInstanceGeometry(node);
        geometry->Parse(instanceGeometryElement);
        node->instanceGeometrys.push_back(geometry);
    }
    
    TiXmlElement* childNodeElement = nodeElement->FirstChildElement("node");
    for(;childNodeElement;childNodeElement=childNodeElement->NextSiblingElement("node"))
    {
        TColladaNode* childNode = ParseNode(childNodeElement, node->nodeId);
        node->AddChild(childNode);
    }
    return node;
}




void TColladaVisualScene::ParseNodeTransform(TColladaNode* node, TiXmlElement* nodeElement)
{
    TiXmlElement* translateElement = nodeElement->FirstChildElement("translate");
    
    Vector3 translate = Vector3::ZERO;
    Vector4 translation(0.0f, 0.0f, 0.0f, 1.0f);
    
    
    Matrix4 transform = Matrix4::IDENTITY;
    
    
    if( translateElement )
    {
        TColladaParserUtils::ParseVector3(translateElement->GetText(), translate);
        translation.x = translate.x;
        translation.y = translate.y;
        translation.z = translate.z;
        
        transform.makeTrans(translate);
    }
    
    
    Quaternion orientation = Quaternion::IDENTITY;
    
    TiXmlElement* rotate = nodeElement->FirstChildElement("rotate");
    for(;rotate;rotate=rotate->NextSiblingElement("rotate"))
    {
        const char* sid = rotate->Attribute("sid");
        if( ! sid )
        {
            Vector4 v;
            TColladaParserUtils::ParseVector4(rotate->GetText(), v);
            
            Quaternion qq0;
            qq0.FromAngleAxis(Math::ToRadian(v.w), Vector3(v.x, v.y, v.z));
            
            qq0.normalise();
            orientation = orientation * qq0;
            
            Matrix3 rot;
            qq0.ToRotationMatrix(rot);
            
            Matrix4 mat = Matrix4(rot);
            
            transform = transform * mat;
        }
        else
        {
        if( 0==strcmp("jointOrientX", sid))
        {
            Vector4 v;
            TColladaParserUtils::ParseVector4(rotate->GetText(), v);
            
            Quaternion qq0;
            qq0.FromAngleAxis(Math::ToRadian(v.w), Vector3::UNIT_X);
            
            qq0.normalise();
            orientation = orientation * qq0;
            
            Matrix3 rot;
            qq0.ToRotationMatrix(rot);
            
            Matrix4 mat = Matrix4(rot);
            
            transform = transform * mat;
            
        }
        if( 0==strcmp("jointOrientY", sid))
        {
            Vector4 v;
            TColladaParserUtils::ParseVector4(rotate->GetText(), v);
            Quaternion qq0;
            qq0.FromAngleAxis(Math::ToRadian(v.w), Vector3::UNIT_Y);
            qq0.normalise();
            orientation = orientation * qq0;
            
            Matrix3 rot;
            qq0.ToRotationMatrix(rot);
            
            Matrix4 mat = Matrix4(rot);
            
            transform = transform * mat;
        }
        if( 0==strcmp("jointOrientZ", sid))
        {
            Vector4 v;
            TColladaParserUtils::ParseVector4(rotate->GetText(), v);
            Quaternion qq0;
            qq0.FromAngleAxis(Math::ToRadian(v.w), Vector3::UNIT_Z);
            qq0.normalise();
            orientation = orientation * qq0;
            
            Matrix3 rot;
            qq0.ToRotationMatrix(rot);
            
            Matrix4 mat = Matrix4(rot);
            
            transform = transform * mat;
        }
#if 0
        if( 0==strcmp("rotateX", sid))
        {
            Vector4 v;
            TColladaParserUtils::ParseVector4(rotate->GetText(), v);
            Quaternion qq0;
            qq0.FromAngleAxis(Math::ToRadian(v.w), Vector3::UNIT_X);
            qq0.normalise();
            orientation = orientation * qq0;
            
            Matrix3 rot;
            qq0.ToRotationMatrix(rot);
            
            Matrix4 mat = Matrix4(rot);
            
            transform = transform * mat;
        }
        if( 0==strcmp("rotateY", sid))
        {
            Vector4 v;
            TColladaParserUtils::ParseVector4(rotate->GetText(), v);
            Quaternion qq0;
            qq0.FromAngleAxis(Math::ToRadian(v.w), Vector3::UNIT_Y);
            qq0.normalise();
            orientation = orientation * qq0;
            
            Matrix3 rot;
            qq0.ToRotationMatrix(rot);
            
            Matrix4 mat = Matrix4(rot);
            
            transform = transform * mat;
        }
        if( 0==strcmp("rotateZ", sid))
        {
            Vector4 v;
            TColladaParserUtils::ParseVector4(rotate->GetText(), v);
            Quaternion qq0;
            qq0.FromAngleAxis(Math::ToRadian(v.w), Vector3::UNIT_Z);
            qq0.normalise();
            orientation = orientation * qq0;
            
            Matrix3 rot;
            qq0.ToRotationMatrix(rot);
            
            Matrix4 mat = Matrix4(rot);
            
            transform = transform * mat;
        }
#endif
    }
    }
    
    orientation.normalise();
    
    node->orientation = orientation;
    node->position = translate;
    
    
    Matrix3 rot;
    orientation.ToRotationMatrix(rot);
    
    
    Matrix4 mm = Matrix4(rot);
    mm.setTrans(translate);
    
    node->transform = transform;
    node->worldTransform = transform;
    
    node->baseTransform = transform;
    
    TiXmlElement* matrix = nodeElement->FirstChildElement("matrix");
    if( matrix )
    {
        std::vector<float> values;
        TColladaParserUtils::ParseFloatArray(matrix->GetText(), values);
        assert( values.size()==16);
        
        int i = 0;
        Matrix4 mat(values[i++], values[i++], values[i++], values[i++],
                    values[i++], values[i++], values[i++], values[i++],
                    values[i++], values[i++], values[i++], values[i++],
                    values[i++], values[i++], values[i++], values[i++]
                    
        
                    );
        
        node->transform = transform * mat;
        node->worldTransform = node->transform;
        node->baseTransform = node->transform;
        
        
    }
}


TColladaNode* TColladaVisualScene::FindNodeById(const std::string& id)
{
    for(size_t j=0;j<allNodes.size();++j)
        if( id == allNodes[j]->id )
            return allNodes[j];
    return NULL;
}

TColladaNode* TColladaVisualScene::GetJointNodeBySID(const std::string& sid) const
{
    for(int j=0;j<(int)allNodes.size();++j)
    {
        if( allNodes[j]->sid == sid && allNodes[j]->type == std::string("JOINT") )
            return allNodes[j];
    }
    //assert( !"ERROR!!!" );
    return NULL;
}
