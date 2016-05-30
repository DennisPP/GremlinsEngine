#include "GMOModel.h"

#include "Graphics/FlatRenderer.h"

#include <map>

TGMOTriangle::TGMOTriangle()
{
    v0 = v1 = v2 = -1;
}

TGMOTriangle::TGMOTriangle(int _v0, int _v1, int _v2)
{
    v0 = _v0;
    v1 = _v1;
    v2 = _v2;
    
}

TGMOMesh::TGMOMesh()
{
    vertices = NULL;
    triangles = NULL;
    
    
    isVisible = true;
}

TGMOMesh::~TGMOMesh()
{
    if( vertices )
    {
        delete [] vertices;
        vertices = NULL;
    }
    if( triangles )
    {
        delete [] triangles;
        triangles = NULL;
    }
}

TGMONode::TGMONode()
{
    parent = -1;
    parentNode = NULL;
}


void TGMONode::UpdateTransform(const Matrix4& t)
{
    worldTransform = t * transform;
    for(int j=0;j<(int)childs.size();++j)
    {
        //assert( childs[j]->parentId == this->nodeId );
        childs[j]->UpdateTransform(worldTransform);
    }
}


void TGMONode::AddChild(TGMONode* ch)
{
    ch->parentNode = this;
    childs.push_back(ch);
}


TGMOAnimationItem::TGMOAnimationItem()
{
    frameRate = 30.0f;
}

TGMOModel::TGMOModel()
{
    playedTime = 0.0f;
}
TGMOModel::~TGMOModel()
{
}


void TGMOModel::Load(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    
    
    TGMOHeader header;
    fread(&header, sizeof(header), 1, fp);
    
    
    int nodeCount = 0;
    fread(&nodeCount, sizeof(int), 1, fp);
    
    for(int j=0;j<nodeCount;++j)
    {
        TGMONode* node = new TGMONode();
        int nameLength = 0;
        fread(&nameLength, sizeof(int), 1, fp);
        assert( nameLength <= 255 );
        char buffer[256];
        memset(buffer, 0 , 256);
        fread(buffer, nameLength, 1, fp);
        node->name = std::string(buffer);
        
        fread(&node->parent, sizeof(int), 1, fp);
        
         //float transform[16];
        fread(node->transform._m, sizeof(float), 16, fp);
        node->worldTransform = node->transform;
       
        /*
        node->transform = Matrix4(transform[0], transform[1], transform[2], transform[3],
                                  transform[4], transform[5], transform[6], transform[7],
                                  transform[8], transform[9], transform[10], transform[11],
                                  transform[12], transform[13], transform[14], transform[15]
                                  );
        */
        nodes.push_back(node);
    }
    for(int j=0;j<nodes.size();++j)
    {
        if( nodes[j]->parent >= 0)
        {
            int pid = nodes[j]->parent;
            TGMONode* parentNode = nodes[pid];
            parentNode->AddChild(nodes[j]);
        }
    }
        
    
   
    std::vector<std::string> instanceID;
    std::vector<std::string> instanceName;
    
    int instanceCount = 0;
    fread(&instanceCount, sizeof(int), 1, fp);
    for(int ins=0;ins<instanceCount;++ins)
    {
        int nodeIDLength = 0;
        char buffer[256];
        memset(buffer, 0 , 256);
        fread(&nodeIDLength, sizeof(int), 1, fp);
        fread(buffer, nodeIDLength, 1, fp);
        std::string id = std::string(buffer);
        
        int nameLength = 0;
        fread(&nameLength, sizeof(int), 1, fp);
        assert( nameLength <= 255 );
        fread(buffer, nameLength, 1, fp);
        std::string name = std::string(buffer);
        instanceID.push_back(id);
        instanceName.push_back(name);
    }
    
    int meshCount = 0;
    fread(&meshCount, sizeof(int), 1, fp);
    for(int m=0;m<meshCount;++m)
    {
        TGMOMesh* mesh = new TGMOMesh();
        
        int nameLength = 0;
        fread(&nameLength, sizeof(int), 1, fp);
        assert( nameLength <= 255 );
        char buffer[256];
        memset(buffer, 0 , 256);
        fread(buffer, nameLength, 1, fp);
        mesh->name = std::string(buffer);
        
        
        int vertexCount = 0;
        fread(&vertexCount, sizeof(int), 1, fp);
        
        mesh->vertexCount = vertexCount;
        mesh->vertices = new TGMOVertex[vertexCount];
        for(int j=0;j<vertexCount;++j)
        {
            TGMOVertex vertex;
            assert( sizeof(TGMOVertex) == 20 );
            fread(&vertex, sizeof(TGMOVertex), 1, fp);
            mesh->vertices[j] = vertex;
        }
        
        
        int triangleCount = 0;
        fread(&triangleCount, sizeof(int), 1, fp);
        mesh->triangleCount = triangleCount;
        mesh->triangles = new TGMOTriangle[triangleCount];
        fread(mesh->triangles, sizeof(TGMOTriangle), triangleCount, fp);
        
        
        meshes.push_back(mesh);
        
    }
    
    for(int j=0;j<instanceName.size();++j)
    {
        int node = FindNodeByName(instanceID[j]);
        int mesh = FindMeshByName(instanceName[j]);
        assert(node >= 0);
        assert(mesh >= 0);
        
        TMeshInstance* ins = new TMeshInstance();
        ins->mesh = meshes[mesh];
        ins->node = nodes[node];
        
        meshInstances.push_back(ins);
        
    }
    int animationCount = 0;
    fread(&animationCount, sizeof(int), 1, fp);
    
    for(int j=0;j<animationCount;++j)
    {
        TGMOAnimationItem* item = new TGMOAnimationItem();
        int nameLength = 0;
        fread(&nameLength, sizeof(int), 1, fp);
        assert( nameLength <= 255 );
        char buffer[256];
        memset(buffer, 0 , 256);
        fread(buffer, nameLength, 1, fp);
        item->target = std::string(buffer);
        
        
        int targetNodeId = FindNodeByName(item->target);
        assert( targetNodeId >= 0 );
        
        
        
        int frameCount = 0;
        fread(&frameCount, sizeof(int), 1, fp);
        for(int f=0;f<frameCount;++f)
        {
            Matrix4 transform;
            fread(transform._m , sizeof(float), 16, fp);
            TGMOFrameData data;
            data.transform = transform;
            item->data.push_back(data);
        }
        animationItems.push_back(item);
    }
    
    fclose(fp);
}


int TGMOModel::FindMeshByName(const std::string& name) const
{
    for(int j=0;j<(int)meshes.size();++j)
    {
        if( meshes[j]->name == name )
            return j;
    }
    return -1;
}

int TGMOModel::FindNodeByName(const std::string& name) const
{
    for(int j=0;j<(int)nodes.size();++j)
    {
        if( nodes[j]->name == name )
            return j;
    }
    return -1;
}


void TGMOModel::Update(float dt)
{
    
    
    playedTime += dt;
    if( animationItems.empty() )
        return;
    
    TGMOAnimationItem* animation = animationItems[0];
    
    int frameCount = (int)animationItems[0]->data.size(); // HACK
    assert( frameCount > 0 );
    float totalTime = (1.0f / animation->frameRate) * frameCount;
    
    
    if( playedTime > totalTime )
        playedTime = fmod(playedTime, totalTime);
    
    
    float t = fmod(playedTime, totalTime);
    t /= totalTime;
    
    assert( t>=0.0f && t<=1.0f );
    
    int frame0 = (int)(t * frameCount);
    
    if( frame0 >= frameCount )
        frame0 = 0;
    
    int frame1 = frame0 + 1;
    if( frame1 >= frameCount )
        frame1 = frameCount-1;
    
    
    for(int anim=0;anim<(int)animationItems.size();++anim)
    {
        TGMOAnimationItem* item = animationItems[anim];
        
        Matrix4 transform = item->data[frame0].transform;
        int nodeId = FindNodeByName(item->target); // TODO: map the target in loading time
        assert( nodeId >= 0 );
        nodes[nodeId]->transform = transform;
    }
    
    
    for(size_t j=0;j<nodes.size();++j)
    {
        if( ! nodes[j]->parentNode )
            nodes[j]->UpdateTransform();
    }
}


void TGMOModel::Render()
{
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    for(int m=0;m<(int)meshInstances.size();++m)
    {
        TGMOMesh* mesh = meshInstances[m]->mesh;
        TGMONode* node = meshInstances[m]->node;
        if( mesh->name.substr(0, 6) == std::string("damage") )
            continue;
        
        if( mesh->name.substr(0, 6) == std::string("attack") )
            continue;
        
        
        //renderer->SetWorldTransform(node->worldTransform);
        for(int t=0;t<mesh->triangleCount;++t)
        {
            TGMOTriangle& triangle = mesh->triangles[t];
            int v0 = triangle.v0;
            int v1 = triangle.v1;
            int v2 = triangle.v2;
            
            TGMOVertex& vertex0 = mesh->vertices[v0];
            TGMOVertex& vertex1 = mesh->vertices[v1];
            TGMOVertex& vertex2 = mesh->vertices[v2];
            
            Vector4 p0( vertex0.x, vertex0.y, vertex0.z, 1.0f);
            Vector4 p1( vertex1.x, vertex1.y, vertex1.z, 1.0f);
            Vector4 p2( vertex2.x, vertex2.y, vertex2.z, 1.0f);
            
            p0 = node->worldTransform * p0;
            p1 = node->worldTransform * p1;
            p2 = node->worldTransform * p2;
            
                        
            TFlatRenderer::TVertexData data0(p0.x, p0.y, p0.z, vertex0.u, vertex0.v);
            TFlatRenderer::TVertexData data1(p1.x, p1.y, p1.z, vertex1.u, vertex1.v);
            TFlatRenderer::TVertexData data2(p2.x, p2.y, p2.z, vertex2.u, vertex2.v);
            
            renderer->RenderTriangle(data0, data1, data2);
        }
        renderer->Flush();
    }
}

