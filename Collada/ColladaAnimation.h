//
//  ColladaAnimation.h
//  ModelViewer
//
//  Created by dennis on 12/3/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __ModelViewer__ColladaAnimation__
#define __ModelViewer__ColladaAnimation__

#include "ColladaBase.h"
#include <vector>


class TColladaSource;
class TColladaSampler;
class TColladaChannel;
class TColladaInput;
class TColladaNode;

class TColladaChannel : public TColladaBase
{
public:
    std::string source;
    std::string target;
};


class TColladaSampler : public TColladaBase
{
public:
    std::vector<TColladaInput*> inputs;
};


class TColladaAnimation : public TColladaBase
{
public:
    TColladaAnimation();
    ~TColladaAnimation();
    
    
    TColladaBase* Parse(TiXmlElement* xml);
    
    TColladaSampler* ParseSampler(TiXmlElement* samplerElement);

    void SetTargetNode(TColladaNode* node);
    
    bool IsValid() const;
    
    TColladaSource* FindSourceById(const std::string& id) const;
    TColladaSampler* FindSamplerById(const std::string& id) const;
    
    TColladaSource* GetTimeSource() const;
    bool IsEqualTimeInterval() const;
    float GetFPS() const;
    int GetFrameCount() const;
    
    
    std::vector<TColladaAnimation*> childAnimations;
    std::vector<TColladaSource*> sources;
    std::vector<TColladaSampler*> samplers;
    std::vector<TColladaChannel*> channels;
    std::string target;
    
    
    TColladaNode* targetNode; // only pointer
    
    
    float playedTime;
};


#endif /* defined(__ModelViewer__ColladaAnimation__) */
