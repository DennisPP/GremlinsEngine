//
//  Renderer.h
//  OSXOpenGL
//
//  Created by dennis on 11/2/2015.
//  Copyright (c) 2015年 dennis. All rights reserved.
//

#ifndef __FlatRenderer__
#define __FlatRenderer__

#include "GraphicsHeader.h"

#include "Graphics/Color.h"

#include "Math3D/Matrix4.h"
#include "Math3D/Vector2.h"
#include "Math3D/Vector3.h"

#include "Math3D/Ray.h"

//#include <EGL/egl.h>
//#include <GLES2/gl2.h>


class TFlatRenderer
{
public:
    struct TVertexData
    {
    public:
        TVertexData(){};
        TVertexData(float x, float y, float z, float u=0.0f, float v=0.0f);
        float x, y, z, u, v;
        float r, g, b, a;
    };
    static TFlatRenderer* GetInstance();
    static TFlatRenderer* instance;
    
    TFlatRenderer(int maxVertex = 8192);
    ~TFlatRenderer();
    
    
    
    void Create();
    void RenderTriangle(const TVertexData& v0, const TVertexData& v1, const TVertexData& v2, const TColor& color = TColor::LightGray);
    
    void Reset();
    void Flush();
    void BindTexture(unsigned int texture);
    void SetOrtho2D(float left, float right, float bottom, float top);
    void SetWorldTransform(const Matrix4& transform);
    Vector3 UnProject(const Vector3 & pt, int width, int height, float depth) const;
    Ray CalPickRay(float screenWidth, float screenHeight, int mx, int my);

    static void RenderLine(const Vector2& pt0, const Vector2& pt1, float thickness, const TColor& color);
    static void RenderCircle(const Vector2& center, float r, int count, const TColor& color);
    static void RenderConcavePolygon(const std::vector<Vector2>& polygon, const TColor& color);
    static void RenderConvexPolygon(const std::vector<Vector2>& polygon, const TColor& color);
    
public:
    Matrix4 worldMatrix;
    Matrix4 projectionMatrix;
    Matrix4 viewMatrix;

    
    static void DumpShaderInfoLog(unsigned int shader);
    static void DumpProgramInfoLog(unsigned int program);
    static bool LoadShaderSource(unsigned int shader, const char *filename);
    static bool CheckShaderStatus(unsigned int shader, GLenum statusFlag);
    static bool CheckProgramStatus(unsigned int program, GLenum statusFlag);
    void PrintInBlock(const char* s);
    void SetProjection(float fovy, float aspect, float near, float far);
    void LookAt(const Vector3& eye, const Vector3& at, const Vector3& up);
protected:
    
    void UpdateMatrix();
protected:
    
    
    
    float nearPlane;
    float farPlane;
    unsigned int shaderProgram;
    unsigned int vertexShader;
    unsigned int fragmentShader;
        
#if GL_ES_VERSION_2_0
#else
    unsigned int vertexArrayObject;
    unsigned int vertexVBO;
    unsigned int uvVBO;
    
#endif
    
    unsigned int texture;
    
    
    
    int diffuseLocation;
    
    
    float* vertices;
    float* uvs;
    float* colors;
    int maxVeretxCount;
    int vertexCount;
};

#endif /* defined(__OSXOpenGL__Renderer__) */
