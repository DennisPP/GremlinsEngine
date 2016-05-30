#include "ColladaAnimation.h"

#include "ColladaSource.h"
#include "ColladaInput.h"

#include "tinyxml.h"


TColladaAnimation::TColladaAnimation() : TColladaBase()
{
    playedTime = 0.0f;
    targetNode = NULL;
}

TColladaAnimation::~TColladaAnimation()
{
    for(size_t j=0;j<sources.size();++j)
        if( sources[j])
            delete sources[j];
    sources.clear();
   
    for(size_t j=0;j<samplers.size();++j)
        if( samplers[j] )
            delete samplers[j];
    samplers.clear();
    
    for(size_t j=0;j<channels.size();++j)
        if( channels[j])
            delete channels[j];
    channels.clear();
}

int TColladaAnimation::GetFrameCount() const
{
    TColladaSource* timeSource = GetTimeSource();
    if( timeSource )
    {
        return (int)timeSource->values.size();
    }
    
    assert(!"");
    return 0;
}

void TColladaAnimation::SetTargetNode(TColladaNode* node)
{
    targetNode = node;
}


TColladaBase* TColladaAnimation::Parse(TiXmlElement* animationElement)
{
    TColladaBase::Parse(animationElement);
    
    TiXmlElement* childAnimationElement = animationElement->FirstChildElement("animation");
    for(;childAnimationElement;childAnimationElement=childAnimationElement->NextSiblingElement("animation"))
    {
        TColladaAnimation* child = new TColladaAnimation();
        child->Parse(childAnimationElement);
        childAnimations.push_back(child);
    }
    
    TiXmlElement* sourceElement = animationElement->FirstChildElement("source");
    // sources
    for(;sourceElement;sourceElement=sourceElement->NextSiblingElement("source"))
    {
        TColladaSource* source = new TColladaSource();
        source->Parse(sourceElement);
        sources.push_back(source);
    }
    // samplers
    TiXmlElement* samplerElement = animationElement->FirstChildElement("sampler");
    for(;samplerElement;samplerElement=samplerElement->NextSiblingElement("sampler") )
    {
        TColladaSampler* s = ParseSampler(samplerElement);
        const char* id = samplerElement->Attribute("id");
        if( s )
        {
            s->id = std::string(id);
            samplers.push_back(s);
        }
    }
    // channels
    TiXmlElement* channelElement = animationElement->FirstChildElement("channel");
    for(;channelElement;channelElement=channelElement->NextSiblingElement("channel") )
    {
        const char* source = channelElement->Attribute("source");
        const char* target = channelElement->Attribute("target");
        
        assert( source && target );
        
        TColladaChannel* channel = new TColladaChannel();
        channel->source = std::string(source);
        channel->target = std::string(target);
        
        channels.push_back(channel);
        
        
        this->target = std::string(target);
        
    }
    
    assert(IsValid());
    
    return this;
}

TColladaSampler* TColladaAnimation::ParseSampler(TiXmlElement* samplerElement)
{
    TColladaSampler* sampler = new TColladaSampler();
    TiXmlElement* inputElement = samplerElement->FirstChildElement("input");
    for(;inputElement;inputElement=inputElement->NextSiblingElement("input"))
    {
        TColladaInput* input = new TColladaInput();
        
        input->Parse(inputElement);
        
        sampler->inputs.push_back(input);
    }
    return sampler;
}

TColladaSource* TColladaAnimation::FindSourceById(const std::string& id) const
{
    for(int j=0;j<(int)sources.size();++j)
    {
        if( sources[j]->id == id )
            return sources[j];
        
    }
    return NULL;
}


TColladaSampler* TColladaAnimation::FindSamplerById(const std::string& id) const
{
    for(int j=0;j<(int)samplers.size();++j)
        if( samplers[j]->id == id )
            return samplers[j];
        
    return NULL;
}

TColladaSource* TColladaAnimation::GetTimeSource() const
{
    for(size_t j=0;j<sources.size();++j)
    {
        for(size_t p=0;p<sources[j]->accessor->params.size();++p)
        {
            if( sources[j]->accessor->params[p]->name == std::string("TIME") )
                return sources[j];
        }
    }
    return NULL;
}

bool TColladaAnimation::IsValid() const
{
    // all accessor has the same count
    for(size_t j=1;j<sources.size();++j)
    {
        if( sources[j-1]->accessor->count != sources[j]->accessor->count )
            return false;
    }
    
    // all input source exists
    for(int s=0;s<(int)samplers.size();++s)
    {
        for(int j=0;j<samplers[s]->inputs.size();++j)
        {
            TColladaInput* input = samplers[s]->inputs[j];
            size_t length = input->source.length();
            std::string s = input->source.substr(1, length-1);
            
            if( ! FindSourceById(s) )
                return false;
        }
    }
    
        
    return true;
}

bool TColladaAnimation::IsEqualTimeInterval() const
{
    TColladaSource* timeSource = GetTimeSource();
    if( !timeSource )
        return true; //HACK t
    
    assert( timeSource );
    if( timeSource->values.size()<2 )
        return true;
    for(int t=2;t<(int)timeSource->values.size();++t)
    {
        float dt0 = timeSource->values[t] - timeSource->values[t-1];
        float dt1 = timeSource->values[t-1] - timeSource->values[t-2];
        float diff = fabs(dt1 - dt0);
        if( diff > 0.01f )
            return false;
    }
    return true;
}

float TColladaAnimation::GetFPS() const
{
    if( ! IsEqualTimeInterval() )
        return -1.0f;
    TColladaSource* timeSource = GetTimeSource();
    
    assert( timeSource );
    assert( timeSource->values.size()>=2 );
    
    return 60.0f*(timeSource->values[1] - timeSource->values[0]);
}


