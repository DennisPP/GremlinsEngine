#include "MD5Renderer.h"
#include "Graphics/Graphics.h"

#include "Core/GremlinsFramework.h"

#include "MD5/MD5.h"

#include <cassert>

TMD5Renderer::TMD5Renderer(TMD5Model* model)
{
    md5Model = model;
    
    vertexArrayObject = NULL;
    vertexVBO = NULL;
    normalVBO = NULL;
    tangentVBO = NULL;
    uvVBO = NULL;
    indexVBO = NULL;
    weightVBO = NULL;
    boneIndexVBO = NULL;
}

TMD5Renderer::~TMD5Renderer()
{
    for(int m=0;m<(int)md5Model->meshes.size();++m)
    {
        if( glIsBuffer(vertexVBO[m]))
            glDeleteBuffers(1, &vertexVBO[m]);
        if( glIsBuffer(normalVBO[m]))
            glDeleteBuffers(1, &normalVBO[m]);
        if( glIsBuffer(tangentVBO[m]))
            glDeleteBuffers(1, &tangentVBO[m]);
        if( glIsBuffer(uvVBO[m]))
            glDeleteBuffers(1, &uvVBO[m]);
        if( glIsBuffer(weightVBO[m]))
            glDeleteBuffers(1, &weightVBO[m]);
        if( glIsBuffer(boneIndexVBO[m]))
            glDeleteBuffers(1, &boneIndexVBO[m]);
    }
    
    
    SAVE_ARRAY_DELETE(vertexArrayObject);
    SAVE_ARRAY_DELETE(vertexVBO);
    SAVE_ARRAY_DELETE(normalVBO);
    SAVE_ARRAY_DELETE(tangentVBO);
    SAVE_ARRAY_DELETE(uvVBO);
    SAVE_ARRAY_DELETE(indexVBO);
    SAVE_ARRAY_DELETE(weightVBO);
    SAVE_ARRAY_DELETE(boneIndexVBO);
}

void TMD5Renderer::PrepareBuffers(TMD5Model* model)
{
    int meshCount = (int)model->meshes.size();
    
    vertexArrayObject = new unsigned int[meshCount];
    vertexVBO = new unsigned int[meshCount];
    tangentVBO = new unsigned int[meshCount];
    normalVBO = new unsigned int[meshCount];
    uvVBO = new unsigned int[meshCount];
    indexVBO = new unsigned int [meshCount];
    weightVBO = new unsigned int [meshCount];
    boneIndexVBO = new unsigned int [meshCount];
    
    for(int m=0;m<(int)model->meshes.size();++m)
    {
        TMD5Mesh* mesh = model->meshes[m];
    
    
        glGenVertexArrays(1, &vertexArrayObject[m]);
        glBindVertexArray(vertexArrayObject[m]);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        
        assert( glGetError() == GL_NO_ERROR);

        int vertexCount = (int)mesh->mVertices.size();
        int bufferSize = vertexCount * 3;
        float* vertices = new float[bufferSize];
        float* normals = new float[bufferSize];
        float* tangents = new float[bufferSize];
        
        float* ptr = vertices;
        
        float* weights = new float[vertexCount*4];
        memset(weights, 0.0f, sizeof(float)*vertexCount*4);
        float* weightPointer = weights;
        
        float* boneIndices = new float[vertexCount*4];
        memset(boneIndices, 0.0f, sizeof(float)*vertexCount*4);
        float* bonePointer = boneIndices;
        
        float* nptr = normals;
        float* tptr = tangents;
        
        for(int j=0;j<vertexCount;++j)
        {
            const Vector3& bpp = mesh->mVertices[j].bindPoseVertexPosition;
            const Vector3& normal = mesh->mVertices[j].normal;
            const Vector3& tangent = mesh->mVertices[j].tangent;
            
            *(ptr++) = bpp.x;
            *(ptr++) = bpp.y;
            *(ptr++) = bpp.z;
            
            *(nptr++) = normal.x;
            *(nptr++) = normal.y;
            *(nptr++) = normal.z;
            
            *(tptr++) = tangent.x;
            *(tptr++) = tangent.y;
            *(tptr++) = tangent.z;
            
            float ws[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            float bi[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
            for(int v=0;v<mesh->mVertices[j].weightCount&&v<4;++v)
            {
                int index = v + mesh->mVertices[j].startWeight;
                ws[v] = mesh->mWegihts[index].mBias;
                bi[v] = mesh->mWegihts[index].mJointIndex;
            }
            for(int v=0;v<4;++v)
            {
                *weightPointer ++ = ws[v];
                *bonePointer ++ = bi[v];
            }
        }
        
        glGenBuffers (1, &vertexVBO[m]);
        assert( vertexVBO[m]!=0 );
        glBindBuffer (GL_ARRAY_BUFFER, vertexVBO[m]);
        glBufferData (GL_ARRAY_BUFFER, 3*vertexCount*sizeof(float), vertices, GL_STATIC_DRAW);
        assert( glGetError() == GL_NO_ERROR);
        delete [] vertices;
        
        
        glGenBuffers (1, &normalVBO[m]);
        assert( normalVBO[m]!=0 );
        glBindBuffer (GL_ARRAY_BUFFER, normalVBO[m]);
        glBufferData (GL_ARRAY_BUFFER, 3*vertexCount*sizeof(float), normals, GL_STATIC_DRAW);
        assert( glGetError() == GL_NO_ERROR);
        delete[] normals;

        
        glGenBuffers (1, &tangentVBO[m]);
        assert( tangentVBO[m]!=0 );
        glBindBuffer (GL_ARRAY_BUFFER, tangentVBO[m]);
        glBufferData (GL_ARRAY_BUFFER, 3*vertexCount*sizeof(float), tangents, GL_STATIC_DRAW);
        assert( glGetError() == GL_NO_ERROR);
        delete[] tangents;

        
        glGenBuffers(1, &weightVBO[m]);
        assert( weightVBO );
        glBindBuffer(GL_ARRAY_BUFFER, weightVBO[m]);
        glBufferData (GL_ARRAY_BUFFER, 4*vertexCount*sizeof(float), weights, GL_STATIC_DRAW);
        assert( glGetError() == GL_NO_ERROR );
        
        glGenBuffers(1, &boneIndexVBO[m]);
        assert( boneIndexVBO[m] );
        glBindBuffer(GL_ARRAY_BUFFER, boneIndexVBO[m]);
        glBufferData (GL_ARRAY_BUFFER, 4*vertexCount*sizeof(float), boneIndices, GL_STATIC_DRAW);
        
        
        float* uvs = new float[vertexCount*2];
        ptr = uvs;
        for(int j=0;j<vertexCount;++j)
        {
            *(ptr++) = mesh->mVertices[j].u;
            *(ptr++) = 1.0f - mesh->mVertices[j].v; //REMARK: invert the v
        }
        
        
        glGenBuffers(1, &uvVBO[m]);
        assert( uvVBO[m] != 0 );
        assert( glGetError() == GL_NO_ERROR);
        glBindBuffer(GL_ARRAY_BUFFER, uvVBO[m]);
        assert( glGetError() == GL_NO_ERROR);
        glBufferData(GL_ARRAY_BUFFER, 2 * vertexCount * sizeof(float), uvs, GL_STATIC_DRAW);
        assert( glGetError() == GL_NO_ERROR);
        
        delete [] uvs;
        
        
        int indexCount = 3 * (int)mesh->mTriangles.size();
        unsigned int* indices = new unsigned int[indexCount];
        
        unsigned int* p = indices;
        for(int j=0;j<(int)mesh->mTriangles.size();++j)
        {
            *(p++) = (unsigned int)mesh->mTriangles[j].v0;
            *(p++) = (unsigned int)mesh->mTriangles[j].v1;
            *(p++) = (unsigned int)mesh->mTriangles[j].v2;
        }
        
        
        // Generate a buffer for the indices
        glGenBuffers(1, &indexVBO[m]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[m]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

        delete [] indices;
        
        
        // Bind a vertex Array
        glBindBuffer (GL_ARRAY_BUFFER, vertexVBO[m]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        assert( glGetError() == GL_NO_ERROR);
        
        glBindBuffer (GL_ARRAY_BUFFER, normalVBO[m]);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        assert( glGetError() == GL_NO_ERROR);
        
        glBindBuffer (GL_ARRAY_BUFFER, tangentVBO[m]);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
        assert( glGetError() == GL_NO_ERROR);
        
        glBindBuffer(GL_ARRAY_BUFFER, uvVBO[m]);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
        assert( glGetError() == GL_NO_ERROR);

        glBindBuffer(GL_ARRAY_BUFFER, boneIndexVBO[m]);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
        assert( glGetError() == GL_NO_ERROR);

        glBindBuffer(GL_ARRAY_BUFFER, weightVBO[m]);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);
        assert( glGetError() == GL_NO_ERROR);
    }
}


bool TMD5Renderer::Create()
{
    
    shaderProgram = glCreateProgram();
    assert( 0!=shaderProgram );
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    assert( 0!=vertexShader );
    
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    assert( 0!=fragmentShader );
    
    const std::string& root = TGremlinsFramework::GetInstance()->GetAssetRoot();
    
    
    
    char vertexShaderPath[1024];
    char fragmentShaderPath[1024];
    
#ifdef GL_ES_VERSION_2_0
    sprintf(vertexShaderPath, "%s%s",root.c_str(), "media/shaders/FlatShading.vs");
    sprintf(fragmentShaderPath, "%s%s",root.c_str(), "media/shaders/FlatShading.fs");
#else
    sprintf(vertexShaderPath, "%s%s",root.c_str(), "media/shaders/SkinnedMeshGL4.vs");
    sprintf(fragmentShaderPath, "%s%s",root.c_str(), "media/shaders/SkinnedMeshGL4.fs");
    
  //  sprintf(vertexShaderPath, "%s%s",root.c_str(), "EngineMedia/Shaders/SkinnedMeshGL4.vs");
   // sprintf(fragmentShaderPath, "%s%s",root.c_str(), "EngineMedia/Shaders/SkinnedMeshGL4.fs");
    
#endif
    
    
    bool loadVSOK = TGraphicsUtils::LoadShaderSource(vertexShader, vertexShaderPath);
    assert(loadVSOK);
    
    
    
    bool loadFSOK = TGraphicsUtils::LoadShaderSource(fragmentShader, fragmentShaderPath);
    assert(loadFSOK);
    
    glCompileShader(vertexShader);
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    
    
    glCompileShader(fragmentShader);
    
    
    
    
    glAttachShader(shaderProgram, vertexShader);
    
    glAttachShader(shaderProgram, fragmentShader);
    assert( glGetError() == GL_NO_ERROR);
    
    
    glBindAttribLocation(shaderProgram, 0, "vertex");
    glBindAttribLocation(shaderProgram, 1, "normal");
    glBindAttribLocation(shaderProgram, 2, "tangent");
    glBindAttribLocation(shaderProgram, 3, "uv");
    glBindAttribLocation(shaderProgram, 4, "boneIndices");
    glBindAttribLocation(shaderProgram, 5, "weights");
    
    assert( glGetError() == GL_NO_ERROR);
    
    glLinkProgram(shaderProgram);
    
    if( ! TGraphicsUtils::CheckProgramStatus(shaderProgram, GL_LINK_STATUS))
    {
        TGraphicsUtils::DumpProgramInfoLog(shaderProgram);
    }

    assert( glGetError() == GL_NO_ERROR);
    
    
    assert( glGetError() == GL_NO_ERROR);
    
    
#ifdef GL_ES_VERSION_2_0
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
  
#else
    // use VAO and VBO in GLSL 4.1
    
       
   
    PrepareBuffers(md5Model);
    
    
    
    
    
    
#endif
    
    
    
    diffuseLocation = glGetUniformLocation(shaderProgram, "diffuse");
    assert( diffuseLocation != -1 );
    
    
    
    
    glValidateProgram(shaderProgram);
    
    if( ! TGraphicsUtils::CheckProgramStatus(shaderProgram, GL_VALIDATE_STATUS) )
    {
        TGraphicsUtils::DumpProgramInfoLog(shaderProgram);
    }
    else
    {
        TGraphicsUtils::PrintInBlock("Shader validation OK!");
    }

    assert( glGetError() == GL_NO_ERROR);
    
    
  
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    
    viewMatrix = Matrix4::IDENTITY;
    projectionMatrix = Matrix4::IDENTITY;
    worldMatrix = Matrix4::IDENTITY;
    
    UpdateMatrix();
    SetLightPosition(Vector3(1.0f, 1.0f, 1.0f));
    SetEyePosition(Vector3(0.0f, 0.0f, 100.0f));
    nearPlane = 0.1f;
    farPlane = 1000.0f;
    
    
    return true;
}


void TMD5Renderer::SetProjection(float fovy, float aspect, float near, float far)
{
    nearPlane = near;
    farPlane = far;
    projectionMatrix = Matrix4::MakeProjectionMatrix(fovy, aspect, near, far);
    
    UpdateMatrix();
}

void TMD5Renderer::LookAt(const Vector3& eye, const Vector3& at, const Vector3& up)
{
    viewMatrix = Matrix4::MakeViewMatrix(eye, at, up);
    UpdateMatrix();
}

void TMD5Renderer::SetWorldTransform(const Matrix4 &transform)
{
    worldMatrix = transform;
    UpdateMatrix();
}

void TMD5Renderer::SetLightPosition(const Vector3 &pos)
{
    glUseProgram(shaderProgram);
    lightPos = pos;
    int lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos");
    assert( glGetError() ==GL_NO_ERROR );
    glUniform3f(lightPosLocation, pos.x, pos.y, pos.z);
    glUseProgram(0);

}

void TMD5Renderer::SetEyePosition(const Vector3& pos)
{
    glUseProgram(shaderProgram);
    eyePos = pos;
    int eyePosLocation = glGetUniformLocation(shaderProgram, "eyePos");
    assert( glGetError() ==GL_NO_ERROR );
    glUniform3f(eyePosLocation, pos.x, pos.y, pos.z);
    glUseProgram(0);
}

void TMD5Renderer::UpdateMatrix()
{
    glUseProgram(shaderProgram);
    
    int t = glGetUniformLocation(shaderProgram, "transform");
    
    Matrix4 transform = worldMatrix * viewMatrix * projectionMatrix;
    
    int worldTransformLocation = glGetUniformLocation(shaderProgram, "worldTransform");
    assert( glGetError() ==GL_NO_ERROR );
    glUniformMatrix4fv(worldTransformLocation, 1, GL_FALSE, worldMatrix._m);
    
    glUniformMatrix4fv(t, 1, GL_FALSE, transform._m);
    glUseProgram(0);
}

void TMD5Renderer::BindTexture(unsigned int _texture)
{
    if( texture != _texture )
    {
        texture = _texture;
    }
}

void TMD5Renderer::BindNormalTexture(unsigned int _texture)
{
    if( normalTexture != _texture )
    {
        normalTexture = _texture;
    }
}


void TMD5Renderer::Render()
{
    
    glUseProgram(shaderProgram);
    
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    assert( glGetError() ==GL_NO_ERROR );
    
    diffuseLocation = glGetUniformLocation(shaderProgram, "diffuse");
    assert( glGetError() ==GL_NO_ERROR );
    glUniform1i(diffuseLocation, 0);
    

    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    assert( glGetError() ==GL_NO_ERROR );

    
    
    
    int normalMapLocation = glGetUniformLocation(shaderProgram, "normalMap");
    assert( glGetError() ==GL_NO_ERROR );
    glUniform1i(normalMapLocation, 1);
    
    
    
    
   
    
    
    //glActiveTexture(GL_TEXTURE0);
    glBindAttribLocation(shaderProgram, 0, "vertex");
    glBindAttribLocation(shaderProgram, 1, "uv");
    assert( glGetError() ==GL_NO_ERROR );

    
#ifdef GL_ES_VERSION_2_0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    
    assert( glGetError() == GL_NO_ERROR);
    
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, uvs);
    assert( glGetError() == GL_NO_ERROR);
    
    
    glDrawArrays (GL_TRIANGLES, 0, vertexCount);
    
    assert( glGetError() == GL_NO_ERROR);
    
#else
    
    bonesLocation = glGetUniformLocation(shaderProgram, "bones");
    assert( bonesLocation!=-1);
    assert( glGetError() ==GL_NO_ERROR );
    
    
    int count = md5Model->GetJointCount();
    float matrices[count * 16];
    memset(matrices, 0, sizeof(float)*count*16);
    float* ptr = matrices;
    for(int j=0;j<count;++j)
    {
        TMD5Joint* joint = md5Model->GetJoint(j);
        
        
        Matrix4 transform = joint->transform * joint->inverseBindPose;
        
        *ptr ++ = transform.m[0][0];
        *ptr ++ = transform.m[1][0];
        *ptr ++ = transform.m[2][0];
        *ptr ++ = transform.m[3][0];
        
        *ptr ++ = transform.m[0][1];
        *ptr ++ = transform.m[1][1];
        *ptr ++ = transform.m[2][1];
        *ptr ++ = transform.m[3][1];
        
        *ptr ++ = transform.m[0][2];
        *ptr ++ = transform.m[1][2];
        *ptr ++ = transform.m[2][2];
        *ptr ++ = transform.m[3][2];
        
        *ptr ++ = transform.m[0][3];
        *ptr ++ = transform.m[1][3];
        *ptr ++ = transform.m[2][3];
        *ptr ++ = transform.m[3][3];
        
        
    }
    
    //glUniform1i(boneCountLocation, count);
    assert( glGetError() == GL_NO_ERROR );
    
    glUniformMatrix4fv(bonesLocation, count, GL_FALSE, matrices);
    
    
    assert( glGetError() == GL_NO_ERROR );
    
    
    for(int m=0;m<md5Model->meshes.size();++m)
    {
        if( md5Model->meshes[m]->IsVisible() )
        {
            TMD5Mesh* mesh = md5Model->meshes[m];
            glBindVertexArray(vertexArrayObject[m]);
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
            glEnableVertexAttribArray(4);
            glEnableVertexAttribArray(5);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO[m]);
        
            int indexCount = 3 * (int)mesh->mTriangles.size();
            // Draw the triangles !
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)0);
    
    
            assert( glGetError() == GL_NO_ERROR);
        }
    }
    
#endif
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glUseProgram(0);
    glBindVertexArray(0);
}

