#include "FlatRenderer.h"

#include "GraphicsHeader.h"

#include <cassert>
#include <cstring>
#include <math.h>

 //#include <EGL/egl.h>
 //#include <GLES2/gl2.h>

#include "GraphicsUtils.h"

#include "Core/GremlinsFramework.h"


const char* vertexShaderSource =
"attribute highp vec4 vertex;\n"
"attribute mediump vec4	uv;\n"
"attribute mediump vec4 color;\n"
"uniform highp mat4	transform;\n"
"varying mediump vec2	texCoord;\n"
"varying mediump vec4 _color;\n"
"void main(void)\n"
"{\n"
"   texCoord = uv.st;\n"
"   _color = color;\n"
"   gl_Position = transform * vertex;\n"
"}";

const char* fragmentShaderSource =
"uniform sampler2D       diffuse;\n"
"varying mediump vec2	texCoord;\n"
"varying mediump vec4 _color;\n"
"void main (void)\n"
"{\n"
"    gl_FragColor = _color*texture2D(diffuse, texCoord);\n"
"}";


TFlatRenderer* TFlatRenderer::instance = NULL;

TFlatRenderer* TFlatRenderer::GetInstance()
{
    if( !instance )
    {
        instance = new TFlatRenderer();
        instance->Create();
    }
    return instance;
}

TFlatRenderer::TVertexData::TVertexData(float _x, float _y, float _z, float _u, float _v)
{
    x = _x;
    y = _y;
    z = _z;
    u = _u;
    v = _v;
    
    r = 1.0f;
    g = 1.0f;
    b = 1.0f;
    a = 1.0f;
}

TFlatRenderer::TFlatRenderer(int maxVertex)
{
    printf("TFlatRenderer::%s\n", __FUNCTION__);
    maxVeretxCount = maxVertex;
    
    vertices = new float[maxVeretxCount*3];
    memset(vertices, 0, sizeof(float)*maxVeretxCount*3);
    
    uvs = new float[maxVeretxCount*2];
    memset(uvs, 0, sizeof(float)*maxVeretxCount*2);
    
    
    colors = new float[maxVeretxCount*4];
    memset(colors, 0, sizeof(float)*maxVeretxCount*4);
    
    
    nearPlane = 0.1f;
    farPlane = 500.0f;
    
    vertexCount = 0;
}

TFlatRenderer::~TFlatRenderer()
{
    printf("TFlatRenderer::%s\n", __FUNCTION__);
    if( vertices )
    {
        delete [] vertices;
        vertices = NULL;
    }
    if( uvs )
    {
        delete [] uvs;
        uvs = NULL;
    }
    if( colors )
    {
        delete [] colors;
        colors = NULL;
    }
    maxVeretxCount = 0;
    vertexCount = 0;
    
    
    if( glIsShader(vertexShader))
    {
        glDeleteShader(vertexShader);
    }
    if( glIsShader(fragmentShader))
    {
        glDeleteShader(fragmentShader);
    }
    if( glIsProgram(shaderProgram))
    {
        glDeleteProgram(shaderProgram);
    }

    /*
#if GL_ES_VERSION_2_0
#else
    if( glIsBuffer(vertexVBO) )
    {
        glDeleteBuffers(1, &vertexVBO);
    }
    if( glIsBuffer(uvVBO))
    {
        glDeleteBuffers(1, &uvVBO);
    }
#endif
*/
}

void TFlatRenderer::Create()
{
    printf("TFlatRenderer::%s\n", __FUNCTION__);
    
    shaderProgram = glCreateProgram();
    DumpProgramInfoLog(shaderProgram);
    assert( shaderProgram>0 );
    printf("Shader Program created successfully!\n");
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    DumpShaderInfoLog(vertexShader);
    assert( vertexShader>0 );
    printf("Vertex Shader created successfully\n");
    
    
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    DumpShaderInfoLog(fragmentShader);
    assert( fragmentShader>0 );
    printf("Fragment Shader created successfully\n");
    
    
    const std::string& root = TGremlinsFramework::GetInstance()->GetAssetRoot();
    
    char vertexShaderPath[1024];
    char fragmentShaderPath[1024];
    
    #ifdef GL_ES_VERSION_2_0
        sprintf(vertexShaderPath, "%s%s",root.c_str(), "media/shaders/FlatShading.vs");
        sprintf(fragmentShaderPath, "%s%s",root.c_str(), "media/shaders/FlatShading.fs");
    #else
        sprintf(vertexShaderPath, "%s%s",root.c_str(), "media/shaders/FlatShadingGL4.vs");
        sprintf(fragmentShaderPath, "%s%s",root.c_str(), "media/shaders/FlatShadingGL4.fs");
    #endif
    

     glShaderSource(vertexShader, 1, (const char**)&vertexShaderSource, 0);


   // bool loadVSOK = LoadShaderSource(vertexShader, vertexShaderPath);
    //DumpShaderInfoLog(vertexShader);
    //assert(loadVSOK);

    
   // bool loadFSOK = LoadShaderSource(fragmentShader, fragmentShaderPath);
   // DumpShaderInfoLog(fragmentShader);
    //assert(loadFSOK);

    glShaderSource(fragmentShader, 1, (const char**)&fragmentShaderSource, 0);


    glCompileShader(vertexShader);
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    CheckShaderStatus(vertexShader, GL_COMPILE_STATUS);
    
    DumpShaderInfoLog(vertexShader);
    
    
    glCompileShader(fragmentShader);
    DumpShaderInfoLog(fragmentShader);
    
    
    
    
    glAttachShader(shaderProgram, vertexShader);
    
    glAttachShader(shaderProgram, fragmentShader);
    assert( glGetError() == GL_NO_ERROR);

    
    glBindAttribLocation(shaderProgram, 0, "vertex");
    glBindAttribLocation(shaderProgram, 1, "uv");
    glBindAttribLocation(shaderProgram, 2, "color");
    
    
    assert( glGetError() == GL_NO_ERROR);

    glLinkProgram(shaderProgram);
    if( ! CheckProgramStatus(shaderProgram, GL_LINK_STATUS))
    {
        DumpProgramInfoLog(shaderProgram);
    }
    
    
   
    
    assert( glGetError() == GL_NO_ERROR);
    
    
    assert( glGetError() == GL_NO_ERROR);
    
    //#ifdef GL_ES_VERSION_2_0
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        
    /*
    #else
        // use VAO and VBO in GLSL 4.1
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    
        assert( glGetError() == GL_NO_ERROR);

    
        glGenBuffers (1, &vertexVBO);
        assert( vertexVBO!=0 );
        glBindBuffer (GL_ARRAY_BUFFER, vertexVBO);
        glBufferData (GL_ARRAY_BUFFER, 3 * maxVeretxCount * sizeof(float), vertices, GL_DYNAMIC_DRAW);
        assert( glGetError() == GL_NO_ERROR);
    
        glGenBuffers(1, &uvVBO);
        assert( uvVBO != 0 );
        assert( glGetError() == GL_NO_ERROR);
        glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
        assert( glGetError() == GL_NO_ERROR);
        glBufferData(GL_ARRAY_BUFFER, 2 * maxVeretxCount*sizeof(float), uvs, GL_DYNAMIC_DRAW);
        assert( glGetError() == GL_NO_ERROR);
    
    
    
    
    
        // Bind a vertex Array
        glBindBuffer (GL_ARRAY_BUFFER, vertexVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        assert( glGetError() == GL_NO_ERROR);
    
        glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
        assert( glGetError() == GL_NO_ERROR);
    
    
    
    #endif
    */
    
    
    diffuseLocation = glGetUniformLocation(shaderProgram, "diffuse");
    assert( diffuseLocation != -1 );

   
    
    
    glValidateProgram(shaderProgram);
    if( ! CheckProgramStatus(shaderProgram, GL_VALIDATE_STATUS) )
    {
        DumpProgramInfoLog(shaderProgram);
    }
    else
    {
        PrintInBlock("Shader validation OK!");
    }
    
    assert( glGetError() == GL_NO_ERROR);
    texture = 0;
    
    worldMatrix = Matrix4::IDENTITY;
    viewMatrix = Matrix4::IDENTITY;
    projectionMatrix = Matrix4::IDENTITY;
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    
    glUseProgram(0);
    
}

void TFlatRenderer::BindTexture(unsigned int _texture)
{
   
    if( texture != _texture )
    {
        Flush();
    
        texture = _texture;
    }
}

void TFlatRenderer::Reset()
{
    vertexCount = 0;
    texture = 0;
}

void TFlatRenderer::Flush()
{
    if( vertexCount <= 0 )
        return;
    
    
   
   
    
    
    assert( glGetError() ==GL_NO_ERROR );
    glUseProgram(shaderProgram);
    
    
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    assert( glGetError() ==GL_NO_ERROR );
    
    //glEnableVertexAttribArray(0);
    //glEnableVertexAttribArray(1);
    
    //assert( glGetError() ==GL_NO_ERROR );
    
    glBindAttribLocation(shaderProgram, 0, "vertex");
    glBindAttribLocation(shaderProgram, 1, "uv");
    glBindAttribLocation(shaderProgram, 2, "color");
    
    
    assert( glGetError() ==GL_NO_ERROR );
    
    diffuseLocation = glGetUniformLocation(shaderProgram, "diffuse");
    assert( glGetError() ==GL_NO_ERROR );
    glUniform1i(diffuseLocation, 0);

   


//#ifdef GL_ES_VERSION_2_0
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    
    assert( glGetError() == GL_NO_ERROR);
    
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, uvs);
    assert( glGetError() == GL_NO_ERROR);
    
    
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, colors);
    assert( glGetError() == GL_NO_ERROR);
    

    glDrawArrays (GL_TRIANGLES, 0, vertexCount);
    
    assert( glGetError() == GL_NO_ERROR);
    
//#else
/*
     glBindVertexArray(vertexArrayObject);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    

    
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*3*vertexCount, vertices);
    assert( glGetError() == GL_NO_ERROR);
   
    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*2*vertexCount, uvs);
    assert( glGetError() ==GL_NO_ERROR );
    
    
    
   
    
    glDrawArrays (GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
*/
//#endif
    
   
    assert( glGetError() ==GL_NO_ERROR );
 
    
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glUseProgram(0);
    vertexCount = 0;    
}

void TFlatRenderer::DumpProgramInfoLog(unsigned int program)
{
    int logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar*)malloc(logLength);
        glGetProgramInfoLog(program, logLength, &logLength, log);
        printf("Shader program log:\n%s\n", log);
        free(log);
    }
}

void TFlatRenderer::DumpShaderInfoLog(unsigned int shader)
{
    int logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar*)malloc(logLength);
        glGetShaderInfoLog(shader, logLength, &logLength, log);
        printf("Shader log:\n%s\n", log);
        free(log);
    }
}


bool TFlatRenderer::LoadShaderSource(unsigned int shader, const char *filename)
{
    FILE *fp ;
    char* buffer;
    const char* pFilename = filename;
    
    
    fp = fopen(pFilename, "rt");
    if( ! fp )
    {
        printf("Failed loading:%s\n", filename);
    }
    assert( fp && filename );
    if( ! fp )
        return false;
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    buffer = (char*) malloc(size+1);
    
    memset( buffer, 0, size+1);
    
    fseek(fp, 0, SEEK_SET);
    fread(buffer, 1, size, fp);
    printf("%s\n", buffer);
    
    glShaderSource(shader, 1, (const char**) &buffer, 0);
    free(buffer);
   
    assert( glGetError() == GL_NO_ERROR);
    return true;
}

const char* StatusToString(GLenum flag)
{
    switch (flag)
    {
        case GL_COMPILE_STATUS:
            return "GL_COMPILE_STATUS";
        break;
        case GL_LINK_STATUS:
            return "GL_LINK_STATUS";
        break;
        case GL_VALIDATE_STATUS:
            return "GL_VALIDATE_STATUS";
    }
    assert(!"Status Flag NOT KNOW\n");
    return NULL;
}

bool TFlatRenderer::CheckShaderStatus(unsigned int shader, GLenum statusFlag)
{
    GLint status;
    glGetShaderiv(shader, statusFlag, &status);
    if (status == 0)
    {
        printf("%s:failed\n", StatusToString(statusFlag));
    }
    else
    {
        printf("%s:success\n", StatusToString(statusFlag));
    }
    return status==0 ? false : true;
}

bool TFlatRenderer::CheckProgramStatus(unsigned int program, GLenum statusFlag)
{
    GLint status;
    glGetProgramiv(program, statusFlag, &status);
    if (status == 0)
    {
        printf("%s:failed\n", StatusToString(statusFlag));
    }
    else
    {
        printf("%s:success\n", StatusToString(statusFlag));
    }
    return status==0 ? false : true;
}

void TFlatRenderer::PrintInBlock(const char* s)
{
    for(int j=0;j<50;++j)
        printf("*");
    printf("\n");
    
    printf("%s\n", s);
    
    for(int j=0;j<50;++j)
        printf("*");
    printf("\n");
}

void TFlatRenderer::SetOrtho2D(float left, float right, float bottom, float top)
{
    Flush();
    
    worldMatrix = Matrix4::IDENTITY;
    viewMatrix = Matrix4::IDENTITY;
    projectionMatrix = Matrix4::MakeOrtho2DMatrix(left, right, bottom, top);
    
    UpdateMatrix();
}

void TFlatRenderer::SetProjection(float fovy, float aspect, float near, float far)
{
    
    nearPlane = near;
    farPlane = far;
    projectionMatrix = Matrix4::MakeProjectionMatrix(fovy, aspect, near, far);

    UpdateMatrix();
}

void TFlatRenderer::LookAt(const Vector3& eye, const Vector3& at, const Vector3& up)
{
    viewMatrix = Matrix4::MakeViewMatrix(eye, at, up);
    UpdateMatrix();
}

void TFlatRenderer::SetWorldTransform(const Matrix4 &transform)
{
    worldMatrix = transform;
    UpdateMatrix();
}

void TFlatRenderer::UpdateMatrix()
{
    glUseProgram(shaderProgram);
    
    int t = glGetUniformLocation(shaderProgram, "transform");
    
    Matrix4 transform = worldMatrix * viewMatrix * projectionMatrix;
    
    glUniformMatrix4fv(t, 1, GL_FALSE, transform._m);
    glUseProgram(0);
}

void TFlatRenderer::RenderTriangle(const TVertexData& v0, const TVertexData& v1, const TVertexData& v2, const TColor& color)
{
    if( vertexCount > maxVeretxCount - 3 )
    {
     //   printf("%d : %d\n", vertexCount, maxVeretxCount);
        Flush();
    }
    
    int i = 3 * vertexCount;
    
    vertices[i++] = v0.x;
    vertices[i++] = v0.y;
    vertices[i++] = v0.z;
    
    vertices[i++] = v1.x;
    vertices[i++] = v1.y;
    vertices[i++] = v1.z;
    
    vertices[i++] = v2.x;
    vertices[i++] = v2.y;
    vertices[i++] = v2.z;

    i = 2 * vertexCount;
    uvs[i++] = v0.u;
    uvs[i++] = v0.v;
    
    uvs[i++] = v1.u;
    uvs[i++] = v1.v;

    uvs[i++] = v2.u;
    uvs[i++] = v2.v;
    
    
    i = 4 * vertexCount;
    // v0
    colors[i++] = color.r;
    colors[i++] = color.g;
    colors[i++] = color.b;
    colors[i++] = color.a;
    
    // v1
    colors[i++] = color.r;
    colors[i++] = color.g;
    colors[i++] = color.b;
    colors[i++] = color.a;
    
    // v2
    colors[i++] = color.r;
    colors[i++] = color.g;
    colors[i++] = color.b;
    colors[i++] = color.a;
    
    
    
    vertexCount += 3;
}

Vector3 TFlatRenderer::UnProject(const Vector3 & pt, int width, int height, float depth) const
{
    Vector4 h;
    h.x = pt.x;
    h.y = pt.y;
    h.z = pt.z;
    h.w = 1.0f;
    
    /* Map x and y from window coordinates */
    h.x = (h.x - 0.0f) / width;
    h.y = (h.y - 0.0f) / height;
    //h.z = (h.z - mNear) / ( mFar - mNear);
    
    /* Map to range -1 to 1 */
    h.x = 2.0f * h.x - 1.0f;
    h.y = 2.0f * h.y - 1.0f;
    h.z = 2.0f * h.z - 1.0f;
    
    
    h.y = - h.y;
    
    Matrix4 final = (viewMatrix*projectionMatrix).inverse();
    
    h = h * final;
    
    h.x /= h.w;
    h.y /= h.w;
    h.z /= h.w;
    
    return Vector3(h.x, h.y, h.z);
}


Ray TFlatRenderer::CalPickRay(float screenWidth, float screenHeight, int mx, int my)
{
    Vector3 pt1 = UnProject(Vector3((float)mx, (float)my,1.0f), screenWidth, screenHeight, farPlane);
    Vector3 pt0 = UnProject(Vector3((float)mx, (float)my,0.0f), screenWidth, screenHeight, nearPlane);
    
    Vector3 dir = (pt1 - pt0).normalisedCopy();
    Ray ray(pt0,dir);	
    return ray;
}


void TFlatRenderer::RenderLine(const Vector2& pt0, const Vector2& pt1, float thickness, const TColor& color)
{
    TFlatRenderer* flatRenderer = TFlatRenderer::GetInstance();
    
    Vector2 dir = (pt1 - pt0).normalisedCopy();
    Vector2 normal = Vector2(-dir.y, dir.x);
    
    float half = 0.5f * thickness;
    Vector2 pp0 = pt0 - normal * half;
    Vector2 pp1 = pt1 - normal * half;
    Vector2 pp2 = pt1 + normal * half;
    Vector2 pp3 = pt0 + normal * half;
    
    TFlatRenderer::TVertexData p0(pp0.x, pp0.y, 0.0f, 0.0f, 0.0f);
    TFlatRenderer::TVertexData p1(pp1.x, pp1.y, 0.0f, 0.0f, 0.0f);
    TFlatRenderer::TVertexData p2(pp2.x, pp2.y, 0.0f, 0.0f, 0.0f);
    TFlatRenderer::TVertexData p3(pp3.x, pp3.y, 0.0f, 0.0f, 0.0f);
    
    flatRenderer->RenderTriangle(p0, p1, p2, color);
    flatRenderer->RenderTriangle(p0, p2, p3, color);
    flatRenderer->Flush();
}

void TFlatRenderer::RenderCircle(const Vector2& center, float r, int count, const TColor& color)
{
    TFlatRenderer* flatRenderer = TFlatRenderer::GetInstance();
    
    if( count < 4 )
        return;
    float angle_inc = Math::TWO_PI / count;
    float angle = 0.0f;
    for(int j=0;j<count;++j)
    {
        float x0 = center.x + r * cos(angle);
        float y0 = center.y + r * sin(angle);
        angle += angle_inc;
        float x1 = center.x + r * cos(angle);
        float y1 = center.y + r * sin(angle);
        
        TFlatRenderer::TVertexData pt0(x0, y0, 0.0f, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt1(x1, y1, 0.0f, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt2(center.x, center.y, 0.0f, 0.0f, 0.0f);
        flatRenderer->RenderTriangle(pt0, pt1, pt2, color);
    }
    flatRenderer->Flush();
}



void TFlatRenderer::RenderConvexPolygon(const std::vector<Vector2>& polygon, const TColor& color)
{
    if( polygon.size() < 3 )
        return;
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    

    for(int j=2;j<(int)polygon.size();++j)
    {
        Vector2 p0 = polygon[0];
        Vector2 p1 = polygon[j-1];
        Vector2 p2 = polygon[j];
        
        TFlatRenderer::TVertexData pt0 = TFlatRenderer::TVertexData(p0.x, p0.y, 0.0f, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt1 = TFlatRenderer::TVertexData(p1.x, p1.y, 0.0f, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt2 = TFlatRenderer::TVertexData(p2.x, p2.y, 0.0f, 0.0f, 0.0f);
        
        renderer->RenderTriangle(pt0, pt1, pt2, color);
        
    }
    renderer->Flush();
}


void TFlatRenderer::RenderConcavePolygon(const std::vector<Vector2>& polygon, const TColor& color)
{
    if( polygon.size() < 3 )
        return;

    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 255, 255);
    glStencilOp(GL_KEEP,GL_KEEP,GL_INVERT);
    glColorMask(0,0,0,0);


    Vector2 minPt, maxPt;
    minPt.x =  std::numeric_limits<float>::infinity();
    minPt.y =  std::numeric_limits<float>::infinity();
    maxPt.x = -std::numeric_limits<float>::infinity();
    maxPt.y = -std::numeric_limits<float>::infinity();

    TFlatRenderer* renderer = TFlatRenderer::GetInstance();



    int count = (int)polygon.size();
    for(int j=2;j<count;++j)
    {
        const Vector2& p0 = polygon[0];
        const Vector2& p1 = polygon[j-1];
        const Vector2& p2 = polygon[j];
    
    if( p0.x < minPt.x )
        minPt.x = p0.x;
        if( p1.x < minPt.x )
            minPt.x = p1.x;
            if( p2.x < minPt.x )
                minPt.x = p2.x;
                
                if( p0.y < minPt.y )
                    minPt.y = p0.y;
                    if( p1.y < minPt.y )
                        minPt.y = p1.y;
                        if( p2.y < minPt.y )
                            minPt.y = p2.y;
                            
                            if( p0.x > maxPt.x )
                                maxPt.x = p0.x;
                                if( p1.x > maxPt.x )
                                    maxPt.x = p1.x;
                                    if( p2.x > maxPt.x )
                                        maxPt.x = p2.x;
                                        
                                        if( p0.y > maxPt.y )
                                            maxPt.y = p0.y;
                                            if( p1.y > maxPt.y )
                                                maxPt.y = p1.y;
                                                if( p2.y > maxPt.y )
                                                    maxPt.y = p2.y;
                                                    
                                                    TFlatRenderer::TVertexData pt0(p0.x, p0.y, 0.0f, 0.0f, 0.0f);
                                                    TFlatRenderer::TVertexData pt1(p1.x, p1.y, 0.0f, 0.0f, 0.0f);
                                                    TFlatRenderer::TVertexData pt2(p2.x, p2.y, 0.0f, 0.0f, 0.0f);
                                                    renderer->RenderTriangle(pt0, pt1, pt2, color);
                                                    }
renderer->Flush();

glColorMask(1,1,1,1);
glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
glStencilFunc(GL_NOTEQUAL,0,255);

TFlatRenderer::TVertexData pt0(minPt.x, minPt.y, 0.0f, 0.0f, 0.0f);
TFlatRenderer::TVertexData pt1(maxPt.x, minPt.y, 0.0f, 0.0f, 0.0f);
TFlatRenderer::TVertexData pt2(maxPt.x, maxPt.y, 0.0f, 0.0f, 0.0f);
TFlatRenderer::TVertexData pt3(minPt.x, maxPt.y, 0.0f, 0.0f, 0.0f);

renderer->RenderTriangle(pt0, pt1, pt2, color);
renderer->RenderTriangle(pt0, pt2, pt3, color);

renderer->Flush();

glDisable(GL_STENCIL_TEST);

}
