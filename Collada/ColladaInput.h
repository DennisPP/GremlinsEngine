//
//  ColladaInput.h
//  ModelViewer
//
//  Created by dennis on 13/3/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __ColladaInput__
#define __ColladaInput__

#include "ColladaBase.h"

class TColladaInput : TColladaBase
{
public:
    virtual TColladaBase* Parse(TiXmlElement* xml);
    std::string semantic;
    std::string source;
    int offset;
};

#endif /* defined(__ModelViewer__ColladaInput__) */
