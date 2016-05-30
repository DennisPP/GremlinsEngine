#ifndef __ColladaVisualScene__
#define __ColladaVisualScene__


#include "ColladaBase.h"

class TColladaNode;

#include <vector>

class TColladaNode;
class TColladaAsset;
class TColladaVisualScene : public TColladaBase
{
    friend class TColladaModel;
    friend class TSlowColladaRenderer;
    friend class TColladaToGASExporter;
public:
    TColladaVisualScene();
    ~TColladaVisualScene();
    virtual TColladaBase* Parse(TiXmlElement* xml);
    TColladaNode* ParseNode(TiXmlElement* nodeElement, int parentId);
    void ParseNodeTransform(TColladaNode* node, TiXmlElement* nodeElement);
    
    
    TColladaNode* FindNodeById(const std::string& id);
    TColladaNode* GetJointNodeBySID(const std::string& sid) const;
    
    int CollectAllNodesInTreeLevelOrder(TColladaNode* root, std::vector<TColladaNode*>& tree) const;
    
    
    
    TColladaAsset* asset;   // 0 or 1
    
protected:
    
    std::vector<TColladaNode*> rootNodes;
    std::vector<TColladaNode*> allNodes;
    std::vector<TColladaNode*> jointNodes;
    
    int nodeId;
};


#endif