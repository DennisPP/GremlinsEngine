#include "GBOAnimation.h"

#include <cassert>


TGBOJointInfo::TGBOJointInfo()
{
  
}

TGBOJointInfo::~TGBOJointInfo()
{
}

TGBOJointInfo::TGBOJointInfo(const std::string& _name, int parent)
{
    name        = _name;
    parentId    = parent;
}


Quaternion TGBOAnimation::BuildQuaternion(float qx, float qy, float qz)
{
    float t = 1.0f - qx * qx - qy * qy - qz * qz;
    float qw = 0.0f;
    if ( t < 0.0f )
    {
        qw = 0.0f;
    }
    else
    {
        qw =  sqrtf(t);
    }
    Quaternion q(qw, qx, qy, qz);
    return q;
}


TGBOAnimation::TGBOAnimation()
{
    frameRate = 30.0f;
}

TGBOAnimation::~TGBOAnimation()
{
}

void TGBOAnimation::Load(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    
    assert( fp );
    
    TGBAHeader header;
    fread(&header, sizeof(TGBAHeader), 1, fp);
    
    
    int jointCount = 0;
    fread(&jointCount, sizeof(int), 1, fp);
    int frameCount = 0;
    fread(&frameCount, sizeof(int), 1, fp);
    int numAnimtedComponent = 0;
    fread(&numAnimtedComponent, sizeof(int), 1, fp);
    
    for(int j=0;j<jointCount;++j)
    {
        TGBOJointInfo jointInfo;
        
        int nameLength = 0;
        fread(&nameLength, sizeof(int),1 , fp);
        char name[256];
        memset(name, 0, 256);
        fread(name, nameLength, 1, fp);
        printf("%s\n", name);
        
        int parentId = -1;
        fread(&parentId, sizeof(int), 1, fp);
        
        int flags = 0;
        fread(&flags, sizeof(int), 1, fp);
        int startIndex = 0;
        
        
        fread(&startIndex, sizeof(int), 1, fp);
        
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float ox = 0.0f;
        float oy = 0.0f;
        float oz = 0.0f;
        fread(&x, sizeof(float), 1, fp);
        fread(&y, sizeof(float), 1, fp);
        fread(&z, sizeof(float), 1, fp);
        
        fread(&ox, sizeof(float), 1, fp);
        fread(&oy, sizeof(float), 1, fp);
        fread(&oz, sizeof(float), 1, fp);

        
        jointInfo.parentId = parentId;
        jointInfo.name = std::string(name);
        
        jointInfo.position = Vector3(x, y, z);
        jointInfo.orientation = Vector3(ox, oy, oz);
        
        
        jointInfos.push_back(jointInfo);
    }
    
    
    int animationCount = 0;
    fread(&animationCount, sizeof(int), 1, fp);
    for(int j=0;j<animationCount;++j)
    {
        int nameLength = 0;
        fread(&nameLength, sizeof(int),1 , fp);
        char name[256];
        memset(name, 0, 256);
        fread(name, nameLength, 1, fp);
        printf("%s\n", name);
        
        
        TAnimationItem animation;
        animation.target = std::string(name);
        
        int transformCount = 0;
        fread(&transformCount, sizeof(int), 1, fp);
        for(int t=0;t<transformCount;++t)
        {
            TGBAFrameData frameData;
            float matrix[16];
            for(int m=0;m<16;++m)
            {
                fread(&matrix[m], sizeof(float), 1, fp);
            }
            frameData.transform = Matrix4(matrix[0], matrix[1], matrix[2], matrix[3],
                                          matrix[4], matrix[5], matrix[6], matrix[7],
                                          matrix[8], matrix[9], matrix[10], matrix[11],
                                          matrix[12], matrix[13], matrix[14], matrix[15]
                                          );
            animation.data.push_back(frameData);
        }
        animationItems.push_back(animation);
    }
    
    /*
    for(int j=0;j<jointCount;++j)
    {
        TGBOBaseFrame baseFrame;
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float ox = 0.0f;
        float oy = 0.0f;
        float oz = 0.0f;
        fread(&x, sizeof(float), 1, fp);
        fread(&y, sizeof(float), 1, fp);
        fread(&z, sizeof(float), 1, fp);
        
        fread(&ox, sizeof(float), 1, fp);
        fread(&oy, sizeof(float), 1, fp);
        fread(&oz, sizeof(float), 1, fp);
        
        baseFrame.position = Vector3(x, y, z);
        baseFrame.orientation = Vector3(ox, oy, oz);
        
        baseFrames.push_back(baseFrame);
    }
    
    
    for(int j=0;j<frameCount;++j)
    {
        TKeyFrameData data;
        for(int anim=0;anim<numAnimtedComponent;++anim)
        {
            float value = 0.0f;
            fread(&value, sizeof(float), 1, fp);
            data.data.push_back(value);
        }
        frameData.push_back(data);
    }
     */
 
    
    fclose(fp);

}


void TGBOAnimation::ParseHierarchy(FILE* fp)
{
    
    char line[1024];
    while( fgets(line, 1024, fp) )
    {
        if( line[0] == '}' )
            break;
        else
        {
            char dummy = { 0 };
            int parentIndex;
            char jointName[1024] = { 0 };
            float x, y, z, ox, oy, oz;
            int k = sscanf(line, "%s %d %c %f %f %f %c %c %f %f %f %c", jointName, &parentIndex, &dummy, &x, &y, &z, &dummy, &dummy, &ox, &oy, &oz, &dummy);
            
            assert( k != EOF );
            TGBOJointInfo info(jointName, parentIndex);
            
            
            info.position = Vector3(x, y, z);
            info.orientation = Vector3(ox, oy, oz);
            
            jointInfos.push_back(info);
            
        }
    }
    
    int a = 0;
}


bool TGBOAnimation::LoadAscii(const char* filename)
{
    FILE*fp = fopen(filename, "rt");
    if( ! fp )
        return false;
    
    char line[512];
    while( fgets(line, 512, fp) )
    {
        char token[512] = { 0 };
        int s = sscanf(line, "%s", token);
        if( s != EOF )
        {
            if( 0==strcmp(token, "numJoints") )
            {
                int s = sscanf(line, "%s %d", token, &numJoints);
                assert( s != EOF );
            }
            else if( 0==strcmp(token, "frameRate") )
            {
                int s = sscanf(line, "%s %f", token, &frameRate);
                assert( s != EOF );
            }
            else if( 0==strcmp(token, "numAnimatedComponents") )
            {
                int s = sscanf(line, "%s %d", token, &numAnimatedComponents);
                assert( s != EOF );
            }
            else if( 0==strcmp(token, "hierarchy") )
            {
                ParseHierarchy(fp);
            }
            else if( 0 ==strcmp(token, "numAnimations"))
            {
                int animationCount = 0;
                int ss = sscanf(line, "%s %d", token, &animationCount);
                assert( ss == 2 );
                for(int j=0;j<animationCount;++j)
                {
                    fgets(line, 512, fp);
                    char anim[128];
                    char name[128];
                    int count = 0;
                    sscanf(line, "%s %s %d", anim, name, &count);
                    
                    TAnimationItem animation;
                    
                    animation.target = std::string(name);
                    for(int j=0;j<count;++j)
                    {
                        fgets(line, 512, fp);
                        //float x, y, z, ox, oy, oz;
                        float value[16];
                        sscanf(line, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                               &value[0], &value[1], &value[2], &value[3],
                               &value[4], &value[5], &value[6], &value[7],
                               &value[8], &value[9], &value[10], &value[11],
                               &value[12], &value[13], &value[14], &value[15]);
                               
                        
                        //Quaternion q = BuildQuaternion(ox, oy, oz);
                        //Matrix3 rotation;
                        //q.ToRotationMatrix(rotation);
                        //transform = Matrix4(rotation);
                        //transform.setTrans(Vector3(x,y,z));
                        
                        Matrix4 transform(value[0], value[1], value[2], value[3],
                                          value[4], value[5], value[6], value[7],
                                          value[8], value[9], value[10], value[11],
                                          value[12], value[13], value[14], value[15]
                                          
                                          );
                        
                        TGBAFrameData data;
                        data.transform = transform;
                        animation.data.push_back(data);
                    }
                    animationItems.push_back(animation);
                }
            }
        }
    
    }
    
    return true;
}

