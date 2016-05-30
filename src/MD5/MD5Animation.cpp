#include "MD5Animation.h"

TMD5JointInfo::TMD5JointInfo()
{	
}

TMD5JointInfo::~TMD5JointInfo()
{
}

TMD5JointInfo::TMD5JointInfo(const char* name, int parent, int flags, int startIndex)
{
	mName = std::string(name);
	


	mParentId = parent;
	mFlags = flags;
	mStartIndex = startIndex;
}



TMD5Animation::TMD5Animation()
{
	mFrameRate = 30.0f;
	mNumJoints = 0;
	mNumAnimatedComponents = 0;
}

TMD5Animation::~TMD5Animation()
{
}

Quaternion TMD5Animation::BuildQuaternion(float qx, float qy, float qz)
{
    float t = 1.0f - qx * qx - qy * qy - qz * qz;
    float qw = 0.0f;
    if ( t < 0.0f )
    {
        qw = 0.0f;
    }
    else
    {
        qw = -sqrtf(t);
    }
    Quaternion q(qw, qx, qy, qz);
    return q;
}


bool TMD5Animation::Load(const char* path)
{
	FILE* fp = fopen(path, "rt");
	assert( fp );
	if( ! fp )
	{
		return false;
	}
	char line[512];	
	while( fgets(line, 512, fp) )
	{
		char token[512] = { 0 };
		int s = sscanf(line, "%s", token);
		if( s != EOF )
		{
            if( 0==strcmp(token, "numJoints") )
			{
				int s = sscanf(line, "%s %d", token, &mNumJoints);
                assert( s != EOF );
			}
			else if( 0==strcmp(token, "frameRate") )
			{
				int s = sscanf(line, "%s %f", token, &mFrameRate);
                 assert( s != EOF );
			}
			else if( 0==strcmp(token, "numAnimatedComponents") )
			{
				int s = sscanf(line, "%s %d", token, &mNumAnimatedComponents);
                 assert( s != EOF );
			}
			else if( 0==strcmp(token, "hierarchy") )
			{
				ParseHierarchy(fp);
			}
			else if( 0==strcmp(token, "bounds") )
			{
				ParseBounds(fp);
			}
			else if( 0==strcmp(token, "baseframe") )
			{
				ParseBaseframe(fp);
			}
			else if( 0==strcmp(token, "frame") )
			{
				ParseFrame(fp);
			}
		}
	}

	//assert( mNumJoints == (int)mBaseFrames.size() );
	return true;
}


void TMD5Animation::ParseHierarchy(FILE* fp)
{
	char line[512];	
	while( fgets(line, 512, fp) )
	{
		if( line[0] == '}' )
			return;
		else
		{
			// "origin"	-1 0 0	/
			int parentIndex;
			int flags;
			int startIndex;
			char jointName[512] = { 0 };
			int k = sscanf(line, "%s %d %d %d", jointName, &parentIndex, &flags, &startIndex);

            assert( k != EOF );
            TMD5JointInfo info(jointName, parentIndex, flags, startIndex);
			
			
			
			mJointInfos.push_back(info);

			if( flags & 1 )
			{
				// x
			}
			if( flags & 2 )
			{
				// y
			}
			if( flags & 4 )
			{
				// z
			}
			if( flags & 8 )
			{
				// orientation x
			}
			if( flags & 16 )
			{
				// orientation y
			}
			if( flags & 32 )
			{
				// orientation z
			}




		}
	}
}

void TMD5Animation::ParseBounds(FILE* fp)
{
	char line[512];	
	while( fgets(line, 512, fp) )
	{
		if( line[0] == '}' )
			return;
		else
		{			
			//int parentIndex;
			//int flags;
			//int startIndex;
			char dummy[512] = { 0 };
			Vector3 minPt;
			Vector3 maxPt;
			int k = sscanf(line, "%s %f %f %f %s %s %f %f %f %s", dummy, &minPt.x, &minPt.y, &minPt.z, dummy, dummy, &maxPt.x, &maxPt.y, &maxPt.z, dummy);
            assert( k != EOF );
		}
	}
}

void TMD5Animation::ParseBaseframe(FILE* fp)
{
	char line[512];	
	while( fgets(line, 512, fp) )
	{
		if( line[0] == '}' )
			return;
		else
		{			
			//int parentIndex;
			//int flags;
			//int startIndex;
			char dummy[512] = { 0 };
			Vector3 position;
			Vector3 orientation;
			int k = sscanf(line, "%s %f %f %f %s %s %f %f %f %s", dummy, &position.x, &position.y, &position.z, dummy, dummy, &orientation.x, &orientation.y, &orientation.z, dummy);
            
            assert( k != EOF );
			TBaseFrame frame;
			frame.mPosition = position;
			frame.mOrientation = orientation;
			mBaseFrames.push_back(frame);
		}
	}	
}

void TMD5Animation::ParseFrame(FILE* fp)
{
	char line[512];	
	TFrameData data;
	while( fgets(line, 512, fp) )
	{
		if( line[0] == '}' )
		{
			break;
		}			
		else
		{	
			 const char* space = " ";
			 char * pch = strtok(line, space);
			 while( pch )
			 {
				 pch = strtok(NULL, space);
				 if( pch )
				 {
					float value = atof(pch);
					data.mData.push_back(value);
				 }
			 }
		}
	}	
	mFrameData.push_back(data);
}