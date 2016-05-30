#include "GBOJoint.h"


TGBOJoint::TGBOJoint()
{
    parent = NULL;
    parentId = -1;
}

TGBOJoint::~TGBOJoint()
{
}

void TGBOJoint::AddChild(TGBOJoint *child)
{
    child->parent = this;
    childs.push_back(child);
}

void TGBOJoint::UpdateTransform(const Matrix4& t)
{
    worldTransform = t * transform;
    for(int j=0;j<(int)childs.size();++j)
    {
        childs[j]->UpdateTransform(worldTransform);
    }
}


void TGBOJoint::Dump(int depth)
{
    for(int j=0;j<depth;++j)
    {
        printf(" ");
    }
    printf("%s\n", name.c_str());
    for(int ch=0;ch<childs.size();++ch)
    {
        childs[ch]->Dump(1+depth);
    }
}
