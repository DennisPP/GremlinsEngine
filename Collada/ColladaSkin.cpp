#include "ColladaSkin.h"

#include "ColladaInput.h"
#include "ColladaSource.h"
#include "ColladaNode.h"

#include "tinyxml.h"


TColladaBase* TColladaJoints::Parse(TiXmlElement *xml)
{
    TiXmlElement* inputElement = xml->FirstChildElement("input");
    for(;inputElement;inputElement=inputElement->NextSiblingElement("input"))
    {
        TColladaInput* input = new TColladaInput();
        input->Parse(inputElement);
        inputs.push_back(input);
    }
    return this;
}

void TColladaVertexWeights::Prepare()
{
    startJointIndices.clear();
    
    int offset = 0;
    for(int j=0;j<vcount.size();++j)
    {
        startJointIndices.push_back(offset);
        offset += inputs.size() * vcount[j];
    }
}




int TColladaVertexWeights::GetJointIndex(int vertexIndex) const
{
    assert( vertexIndex < startJointIndices.size());
    int jointOffset = this->startJointIndices[vertexIndex];
    return this->v[jointOffset];
}


TColladaSkin::TColladaSkin()
{
    joints = NULL;
}

TColladaSkin::~TColladaSkin()
{
    if( joints )
    {
        delete joints;
        joints = NULL;
    }
}


TColladaBase* TColladaSkin::Parse(TiXmlElement *skin)
{
    const char* geometrySource = skin->Attribute("source");
   
    this->source = geometrySource ? std::string(geometrySource) : "";
    
    TiXmlElement* vertex_weights = skin->FirstChildElement("vertex_weights");
    
    TiXmlElement* bind_shape_matrix = skin->FirstChildElement("bind_shape_matrix");
    TiXmlElement* sourceElement = skin->FirstChildElement("source");
    
    if( bind_shape_matrix )
    {
        std::vector<float> values;
        TColladaParserUtils::ParseFloatArray(bind_shape_matrix->GetText(), values);
        assert( values.size() == 16 );
        int i = 0;
        bindShapeMatrix = Matrix4(values[i++], values[i++], values[i++], values[i++],
                                  values[i++], values[i++], values[i++], values[i++],
                                  values[i++], values[i++], values[i++], values[i++],
                                  values[i++], values[i++], values[i++], values[i++]);
        
        //assert( bindShapeMatrix == Matrix4::IDENTITY );
        
        
        
    }
    
    for(;sourceElement;sourceElement=sourceElement->NextSiblingElement("source"))
    {
        TColladaSource* source = new TColladaSource();
        source->Parse(sourceElement);
        sources.push_back(source);
    }
    
    TiXmlElement* jointsElement = skin->FirstChildElement("joints");
    if( jointsElement )
    {
        joints = new TColladaJoints();
        joints->Parse(jointsElement);
        
        
        
        TColladaSource* bindPoseSource = GetBindPoseSource();
        if( bindPoseSource )
        {
            std::vector<float>& bindPoses = bindPoseSource->values;
            assert( bindPoses.size() % 16 == 0 );
            int bindPoseCount = (int)bindPoses.size() / 16;
            for(int j=0;j<bindPoseCount;++j)
            {
                int i = j * 16;
                Matrix4 mat(bindPoses[i++], bindPoses[i++], bindPoses[i++], bindPoses[i++],
                            bindPoses[i++], bindPoses[i++], bindPoses[i++], bindPoses[i++],
                            bindPoses[i++], bindPoses[i++], bindPoses[i++], bindPoses[i++],
                            bindPoses[i++], bindPoses[i++], bindPoses[i++], bindPoses[i++]
                            );
                invBindPoses.push_back(mat);
            }
            printf("%d inverse bind pose loaded\n", bindPoseCount);
        
        }
    }
    
    assert( joints );

    
    
    
    TColladaVertexWeights* vw = new TColladaVertexWeights();
    if( vertex_weights )
    {
        int count = 0;
        vertex_weights->Attribute("count", &count);
        vw->count = count;
        TiXmlElement* inputElement = vertex_weights->FirstChildElement("input");
        for(;inputElement;inputElement=inputElement->NextSiblingElement("input"))
        {
            TColladaInput* input = new TColladaInput();
            input->Parse(inputElement);
            
            vw->inputs.push_back(input);
        }
        
        TiXmlElement* vcount = vertex_weights->FirstChildElement("vcount");
        if( vcount )
        {
            TColladaParserUtils::ParseIntArray(vcount->GetText(), vw->vcount);
        }
        TiXmlElement* v = vertex_weights->FirstChildElement("v");
        if( v )
        {
            TColladaParserUtils::ParseIntArray(v->GetText(), vw->v);
        }
    }
    vertexWeights = vw;
    vertexWeights->Prepare();
    
    
    
    return this;
}

TColladaSource* TColladaSkin::GetBindPoseSource() const
{
    for(size_t j=0;j<joints->inputs.size();++j)
    {
        TColladaInput* input = joints->inputs[j];
        if( input->semantic == std::string("INV_BIND_MATRIX"))
        {
            std::string sid = TColladaParserUtils::SkipStartChar(input->source);
            TColladaSource* s = TColladaParserUtils::FindSourceById(sources, sid);
            if( s )
                return s;
        }
    }
    assert(!"ERROR!!!");
    return NULL;
}


TColladaSource* TColladaSkin::GetWeightSource() const
{
    if( vertexWeights )
    {
        for(int v=0;v<(int)vertexWeights->inputs.size();++v)
        {
            if( vertexWeights->inputs[v]->semantic == std::string("WEIGHT") )
            {
                for(int s=0;s<sources.size();++s)
                {
                    std::string id = TColladaParserUtils::SkipStartChar(vertexWeights->inputs[v]->source);
                    if( id == sources[s]->id )
                    {
                        return sources[s];
                    }
                }
            }
        }
    }
    return NULL;
}

TColladaSource* TColladaSkin::GetJointNameSource() const
{
    for(int s=0;s<(int)sources.size();++s)
    {
        TColladaAccessor* accessor = sources[s]->accessor;
        for(int p=0;p<(int)accessor->params.size();++p)
        {
            if( accessor->params[p]->name == std::string("JOINT") && accessor->params[p]->type == "Name" )
                return sources[s];
            else if( accessor->params[p]->type == "name" )
                return sources[s];
        }
    }
    return NULL;
}


int TColladaSkin::GetJointIndex(TColladaNode* jointNode) const
{
    TColladaSource* jointNameSource = GetJointNameSource();
    assert( jointNameSource );
    for(int j=0;j<jointNameSource->nameArray->names.size();++j)
    {
        if( jointNode->name == jointNameSource->nameArray->names[j] )
            return j;
    }
    return -1;
}
