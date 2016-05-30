#include "ColladaMesh.h"

#include "ColladaInput.h"
#include "ColladaSource.h"
#include "ColladaTriangle.h"

#include "ColladaGeometry.h"

#include "tinyxml.h"

TColladaPolyList::TColladaPolyList()
{
    stride = -1;
    vertexOffset = -1;
    normalOffset = -1;
    textureOffset = -1;
}

bool TColladaPolyList::IsValid() const
{
    if( stride <= 0 )
        return false;
    if( vertexOffset < 0 )
        return false;
    if( normalOffset < 0 )
        return false;
    if( textureOffset < 0 )
        return false;
    return true;
}

int TColladaPolyList::GetTriangleCount() const
{
    assert( stride != 0 );
    int triangleCount = 0;
    for(size_t v=0;v<vcount.size();++v)
         triangleCount += vcount[v] - 2;
    
    return triangleCount;
}


int TColladaPolyList::Triangulate(std::vector<TTriangleInfo>& triangles) const
{
    assert( IsValid() );
    // skip the #
    //size_t length = vertexSource.length();
    //std::string s = vertexSource.substr(1, length-1);
    
    //length = texCoordSource.length();
    //s = texCoordSource.substr(1, length-1);
    int startIndex = 0;
    int triIndex = 0;
    for(int v=0;v<vcount.size();++v)
    {
        //startIndex = triIndex * stride;
        int vertexCount = vcount[v];
        assert( vertexCount>=3 );
        // divide the polygon to triangles
        for(int k=2;k<vertexCount;++k)
        {
            TTriangleInfo triangle;

            int vv[3] = { 0, k-1, k };
            for(int vi=0;vi<3;++vi)
            {
                int i = vertexOffset + (vv[vi]) * stride + startIndex;
                int vertexID = p[i];
                int ii = vertexID * 3; // x, y, z
                               triangle.vertex[vi] = ii;
                int t0 = textureOffset + (vv[vi]) * stride + startIndex;
                int uu0 = p[t0] * 2; // 2 = u + v
                triangle.texCoord[vi] = uu0;
                
            }
            triangles.push_back(triangle);
            ++ triIndex;
        }
        startIndex += vertexCount * stride;
    }
    return (int)triangles.size();
}


TColladaBase* TColladaVertices::Parse(TiXmlElement* xml)
{
    const char* id = xml->Attribute("id");
    assert( id );
    
    this->id = std::string(id);
    TiXmlElement* inputElement = xml->FirstChildElement("input");
    for(;inputElement;inputElement=inputElement->NextSiblingElement("input"))
    {
        TColladaInput* input = new TColladaInput();
        input->Parse(inputElement);
        inputs.push_back(input);
        if( input->semantic == "POSITION" )
            source = input->source;
    }
    return this;
}


TColladaMesh::TColladaMesh(TColladaGeometry* _geometry)
{
    geometry = _geometry;
}


TColladaMesh::~TColladaMesh()
{
}


TColladaSource* TColladaMesh::FindSourceById(const std::string& id)
{
    
    for(size_t j=0;j<sources.size();++j)
    {
        if( sources[j]->id == id )
            return sources[j];
    }
    printf("%s:Source NOT FOUND!\n", id.c_str());
    return NULL;
}

TColladaSource* TColladaMesh::FindVertexSourceById(const std::string& id)
{
    for(size_t j=0;j<vertices.size();++j)
    {
        if( id == vertices[j]->id )
        {
            // skip the #
            size_t length = vertices[j]->source.length();
            std::string s = vertices[j]->source.substr(1, length-1);
            return FindSourceById(s);
        }
    }
    return NULL;
}

TColladaBase* TColladaMesh::Parse(TiXmlElement* meshElement)
{

    TiXmlElement* sourceElement = meshElement->FirstChildElement("source");
    
    TiXmlElement* polylist = meshElement->FirstChildElement("polylist");
    
    
    TiXmlElement* triangle = meshElement->FirstChildElement("triangles");
    for(;triangle;triangle=triangle->NextSiblingElement("triangles"))
    {
        TColladaTriangle* tri = new TColladaTriangle();
        tri->Parse(triangle);
        
        for(int i=0;i<(int)tri->inputs.size();++i)
        {
            if( tri->inputs[i]->semantic == "TEXCOORD" )
            {
                texCoordSource = tri->inputs[i]->source;
            }
        }
        
        triangles.push_back(tri);
    }
    
    TiXmlElement* verticesElement = meshElement->FirstChildElement("vertices");
    for(;verticesElement;verticesElement=verticesElement->NextSiblingElement("vertices"))
    {
        TColladaVertices* v = new TColladaVertices();
        v->Parse(verticesElement);
        vertices.push_back(v);
        
        for(int i=0;i<(int)v->inputs.size();++i)
        {
            if( v->inputs[i]->semantic == "TEXCOORD" )
            {
                texCoordSource = v->inputs[i]->source;
            }
        }        
    }
    for(;polylist;polylist=polylist->NextSiblingElement("polylist"))
    {
        const char* material = polylist->Attribute("material");
        TiXmlElement* vcount = polylist->FirstChildElement("vcount");
        TiXmlElement* p = polylist->FirstChildElement("p");
        assert( vcount && p );
        TColladaPolyList* list = new TColladaPolyList();
        
        list->material = material ? std::string(material) : "";
        
        TiXmlElement* input = polylist->FirstChildElement("input");
        int j = 0;
        list->stride = 0;
        for(;input;input=input->NextSiblingElement("input"))
        {
            const char* semantic = input->Attribute("semantic");
            if( 0==strcmp(semantic, "VERTEX"))
            {
                list->vertexSource = std::string(input->Attribute("source"));
                list->vertexOffset = j;
                list->stride ++;
            }
            else if( 0==strcmp(semantic, "TEXCOORD"))
            {
                list->texCoordSource = std::string(input->Attribute("source"));
                list->textureOffset = j;
                list->stride ++;
            }
            else if( 0==strcmp(semantic, "NORMAL"))
            {
                list->normalSource = std::string(input->Attribute("source"));
                list->normalOffset = j;
                list->stride ++;
            }
            ++ j;
        }
        
        
        TColladaParserUtils::ParseIntArray(vcount->GetText(), list->vcount);
        TColladaParserUtils::ParseIntArray(p->GetText(), list->p);
        
        
        
        polylists.push_back(list);
        
        
    }
    
    for(;sourceElement;sourceElement=sourceElement->NextSiblingElement("source"))
    {
        TColladaSource* source = new TColladaSource();
        source->Parse(sourceElement);
        if( source )
            sources.push_back(source);
        
    }

    return this;
}
