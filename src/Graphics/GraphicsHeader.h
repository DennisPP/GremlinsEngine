//
//  GraphicsHeader.h
//  SocketBig2IOSClient
//
//  Created by dennis on 18/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef _GraphicsHeader_h
#define _GraphicsHeader_h

#if defined(__APPLE__)
//#pragma message("__APPLE__ is defined")
#include "TargetConditionals.h"
#endif



#if WIN32
    //#pragma message("Use OpenGL ES:(WIN32)")
    #include <EGL/egl.h>
    #include <GLES2/gl2.h>
#elif TARGET_OS_IPHONE
    //#pragma message("Use OpenGL ES")
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#else
    //#pragma message("Use OpenGL 3")
    #include <OpenGL/gl3.h>
#endif


#endif