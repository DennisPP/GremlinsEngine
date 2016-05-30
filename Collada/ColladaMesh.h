#ifndef __ColladaMesh__
#define __ColladaMesh__

#include "ColladaBase.h"
class TColladaSource;
class TColladaVertices;
class TColladaPolyList;
class TColladaTriangle;
class TColladaInput;
class TColladaGeometry;


class TTriangleInfo
{
public:
    int vertex[3];
    int normal[3];
    int texCoord[3];
};

class TColladaVertices : public TColladaBase
{
public:
    
    virtual TColladaBase* Parse(TiXmlElement* xml);
    
    std::string source;
    std::vector<TColladaInput*> inputs;
};

class TColladaPolyList : public TColladaBase
{
public:
    TColladaPolyList();
    bool IsValid() const;
    std::string vertexSource;
    std::string texCoordSource;
    std::string normalSource;
    
    int GetTriangleCount() const;
    
    int Triangulate(std::vector<TTriangleInfo>& triangles) const;
    
    
    //bool HasNormal() const;
    //bool HasTextureCoord() const;
    //bool HasVertex() const;
    
    
    int stride;
    int vertexOffset;
    int normalOffset;
    int textureOffset;   
    
    std::string material;
    std::vector<int> vcount;
    std::vector<int> p;
};



class TColladaMesh : public TColladaBase
{
public:
    TColladaMesh(TColladaGeometry* geometry);
    ~TColladaMesh();
    virtual TColladaBase* Parse(TiXmlElement* xml);
    TColladaSource* FindVertexSourceById(const std::string& id);
    TColladaSource* FindSourceById(const std::string& id);

    
    
    TColladaGeometry* geometry; // parent
    
    
    std::string texCoordSource;

    std::vector<TColladaSource*> sources;
    std::vector<TColladaVertices*> vertices;
    std::vector<TColladaPolyList*> polylists;   // zero or more
    std::vector<TColladaTriangle*> triangles;   // zero or more
    
};

#endif