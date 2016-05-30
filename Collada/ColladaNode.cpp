#include "ColladaNode.h"

#include "ColladaInstanceController.h"

#include "tinyxml.h"

#include <queue>


TColladaNode::TColladaNode() : TColladaBase()
{
    //parentId = -1;
    parentNode = NULL;
}

TColladaNode::~TColladaNode()
{
    for(size_t j=0;j<childs.size();++j)
    {
        if( childs[j])
            delete childs[j];
    }
    childs.clear();
}


void TColladaNode::AddChild(TColladaNode* child)
{
    child->parentId = nodeId;
    child->parentNode = this;
    childs.push_back(child);
}


TColladaNode::TColladaNode(const Matrix4& _transform, int pid)
{
    transform = _transform;
    parentId = pid;
}


bool TColladaNode::IsRootNode() const
{
    return parentNode == NULL;
}

bool TColladaNode::IsJointNode() const
{
    return type == std::string("JOINT");
}


void TColladaNode::UpdateTransform(const Matrix4& t)
{
    worldTransform = t * transform;
    for(int j=0;j<(int)childs.size();++j)
    {
        //assert( childs[j]->parentId == this->nodeId );
        childs[j]->UpdateTransform(worldTransform);
    }
}

bool TColladaNode::IsValid() const
{
    std::queue<const TColladaNode*> q;
    q.push(this);
    while( q.empty())
    {
        const TColladaNode* front = q.front();
        if( front->type !=std::string("JOINT"))
            return false;
        
        q.pop();
        for(int j=0;j<(int)childs.size();++j)
        {
            q.push(childs[j]);
        }
    }
    return true;
}

void TColladaNode::Dump(int depth)
{
    const char space = ' ';
    for(int j=0;j<depth;++j)
    {
        printf("%c", space);
    }
    printf("%s\n", name.c_str());
    
    for(int j=0;j<childs.size();++j)
    {
        childs[j]->Dump(1+depth);
    }
}

int TColladaNode::TreeSize() const
{
    int count = 1;
    for(int j=0;j<(int)childs.size();++j)
        count += childs[j]->TreeSize();
    return count;
}

