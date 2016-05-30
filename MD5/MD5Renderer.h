
#ifndef __MD5Renderer__
#define __MD5Renderer__


#include "Math3D/Matrix4.h"
#include "Math3D/Vector3.h"
#include "MD5Model.h"

class TMD5Model;


// Hardware Skinning MD5 Renderer
// Only use support maximum 4 bones in this implementation
// NOTE: MD5 format support arbitrary number of bones

class TMD5Mesh;

class TMD5Renderer
{
public:
    TMD5Renderer(TMD5Model* model);
    ~TMD5Renderer();
    
    void Render();


    
    
    void SetProjection(float fovy, float aspect, float near, float far);
    
    void LookAt(const Vector3& eye, const Vector3& at, const Vector3& up);
    
    void SetWorldTransform(const Matrix4 &transform);
    
    void BindTexture(unsigned int texture);
    void BindNormalTexture(unsigned int normalTexture);
    bool Create();
    void SetLightPosition(const Vector3& pos);
    void SetEyePosition(const Vector3& pos);
   
protected:
    void UpdateMatrix();
    
    void PrepareBuffers(TMD5Model* model);
    
    TMD5Model* md5Model;
    
    unsigned int* vertexArrayObject;
    unsigned int* vertexVBO;
    unsigned int* normalVBO;
    unsigned int* tangentVBO;
    unsigned int* uvVBO;
    unsigned int* indexVBO;
    unsigned int* weightVBO;
    unsigned int* boneIndexVBO;
    
    unsigned int texture;
    unsigned int normalTexture;
    
    
    
    unsigned int shaderProgram;
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int bonesLocation;
    
    
    Vector3 eyePos;
    Vector3 lightPos;
    Matrix4 viewMatrix;
    Matrix4 projectionMatrix;
    Matrix4 worldMatrix;
    
    
    float nearPlane;
    float farPlane;
    
    
    int diffuseLocation;
    
    
};


#endif