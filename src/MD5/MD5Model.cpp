#include "MD5Model.h"

#include "MD5Joint.h"
#include "MD5Animation.h"

#include "Math3D/Quaternion.h"
#include "Math3D/Vector2.h"
#include "Math3D/Vector3.h"
#include "Math3D/Vector4.h"

#include "FlatRenderer.h"

#include <stdio.h>
#include <cassert>
#include <cstring>


Quaternion TMD5Model::BuildQuaternion(float qx, float qy, float qz)
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


TMD5Triangle::TMD5Triangle(int _v0, int _v1, int _v2)
{
	v0 = _v0;
	v1 = _v1;
	v2 = _v2;
}

TMD5Mesh::TMD5Mesh()
{
    isVisible = true;
}

bool TMD5Mesh::IsVisible() const
{
    return isVisible;
}


void TMD5Mesh::SetVisible(bool v)
{
    isVisible = v;
}

TMD5Model::TMD5Model()
{	
	mAnimation = NULL; 

	mPlayedTime = 0.0f;
}

TMD5Model::~TMD5Model()
{
	mAnimation = NULL;

	for(size_t j=0;j<mJoints.size();++j)
		delete mJoints[j];
	mJoints.clear();
    
    for(size_t j=0;j<meshes.size();++j)
        delete meshes[j];
    meshes.clear();
}

void TMD5Model::SetAnimation(TMD5Animation* animation)
{
	mAnimation = animation;
}

void TMD5Model::PrepareBindPoseVertices()
{
    for(int mesh=0;mesh<meshes.size();++mesh)
    {
        for(int v=0;v<(int)meshes[mesh]->mVertices.size();++v)
        {
            Vector3 vertexPosition = Vector3::ZERO;
            
            TMD5Vertex* vertex;
            vertex = & meshes[mesh]->mVertices[v];
            int wCount = vertex->weightCount;
            for(int w=0;w<wCount;++w)
            {
                const TMD5Weight& weight = meshes[mesh]->mWegihts[w + vertex->startWeight];
                int jointIndex = weight.mJointIndex;
                
                assert( jointIndex<mJoints.size());
                TMD5Joint* joint = mJoints[jointIndex];
                
                Vector3 wp(weight.mPosition.x, weight.mPosition.y, weight.mPosition.z);
                wp = joint->mOrientation * wp + joint->GetPosition();
                
                vertexPosition += weight.mBias * wp;
            }
            
            meshes[mesh]->mVertices[v].bindPoseVertexPosition = vertexPosition;
        }
    }
}

bool TMD5Model::LoadMesh(const char* path)
{
	FILE* fp = fopen(path, "rt");
	assert( fp );
	if( ! fp )
	{
	//	TDebugDrawer::Print("WARNING!!!! %s Loading failed! %s %s", path,  __FILE__ , __LINE__); 
		return false;
	}
	char line[512];	
	while( fgets(line, 512, fp) )
	{
		char token[512] = { 0 };
		int s = sscanf(line, "%s", token);
		if( s != EOF )
		{
			if( 0 == strcmp(token, "joints") )
			{
				ParseJoints(fp);							
			}
			else if( 0==strcmp(token, "mesh") )
			{
				LoadSubMesh(path, fp);
			}
		}
	}

	for(size_t j=0;j<mJoints.size();++j)
	{
		TMD5Joint* joint = mJoints[j];
		if( joint->mParentId >= 0 )
		{
			TMD5Joint* parent = mJoints[joint->mParentId];
			parent->AddChild(joint);
		}
	}
    
    
    // The MD5 file format do NOT store the vertex bind pose position
    // we calculate the vertex position and store it    
    PrepareBindPoseVertices();
    
    
    
    
    // build the smoothed normal of each vertex
    for(int j=0;j<(int)meshes.size();++j)
    {
        TMD5Mesh* mesh = meshes[j];
        for(int v=0;v<(int)mesh->mVertices.size();++v)
        {
            mesh->mVertices[v].normal = Vector3::ZERO;
            mesh->mVertices[v].tangent = Vector3::ZERO;
        }
        for(int tri=0;tri<(int)mesh->mTriangles.size();++tri)
        {
            TMD5Vertex& v0 = mesh->mVertices[mesh->mTriangles[tri].v0];
            TMD5Vertex& v1 = mesh->mVertices[mesh->mTriangles[tri].v1];
            TMD5Vertex& v2 = mesh->mVertices[mesh->mTriangles[tri].v2];
            Vector3 edge0 = v1.bindPoseVertexPosition - v0.bindPoseVertexPosition;
            Vector3 edge1 = v2.bindPoseVertexPosition - v0.bindPoseVertexPosition;
            Vector3 normal = edge0.crossProduct(edge1);
            v0.normal += normal;
            v1.normal += normal;
            v2.normal += normal;

            
            
            Vector2 uv0 = Vector2(v0.u, v0.v);
            Vector2 uv1 = Vector2(v1.u, v1.v);
            Vector2 uv2 = Vector2(v2.u, v2.v);
            
            Vector3 deltaPos1 = v1.bindPoseVertexPosition - v0.bindPoseVertexPosition;
            Vector3 deltaPos2 = v2.bindPoseVertexPosition - v0.bindPoseVertexPosition;
            
            // UV delta
            Vector2 deltaUV1 = uv1-uv0;
            Vector2 deltaUV2 = uv2-uv0;
            
            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            Vector3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
            //Vector3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;

            v0.tangent += tangent;
            v1.tangent += tangent;
            v2.tangent += tangent;
        }
        for(int v=0;v<(int)mesh->mVertices.size();++v)
        {
            mesh->mVertices[v].normal.normalise();

            Vector3 normal = mesh->mVertices[v].normal;
            Vector3 tangent = mesh->mVertices[v].tangent;
            
            
         
            
            mesh->mVertices[v].tangent = (tangent - normal * normal.dotProduct(tangent));
                       mesh->mVertices[v].tangent.normalise();
        }
    }
    
    
   return true;
}

void TMD5Model::ParseJoints(FILE* fp)
{
	char line[512];	
	while( fgets(line, 512, fp) )
	{
		if( line[0] == '}' )
			return;
		else
		{
			char jointName[512] = { 0 };
			char dummy[512] = { 0 };
			int parentId = 0;
			float pos[3];
			float orientation[3];
			int s = sscanf(line, "%s %d %s %f %f %f %s %s %f %f %f %s", jointName, &parentId, dummy, &pos[0], &pos[1], &pos[2], dummy, dummy, &orientation[0], &orientation[1], &orientation[2], dummy);
            
            assert( s == 12 );
			if( s != EOF )
			{
				TMD5Joint* joint = new TMD5Joint();
				joint->SetName(jointName);
				joint->SetParent(parentId);
				joint->SetPosition(Vector3(pos[0], pos[1], pos[2]));
				joint->SetOrientation(BuildQuaternion(orientation[0], orientation[1], orientation[2]));
                
                joint->mPositionLoaded = Vector3(pos[0], pos[1], pos[2]);
                
                
                Matrix3 rot;
                joint->mOrientation.ToRotationMatrix(rot);
                Matrix4 bindPose(rot);
                bindPose.setTrans(joint->mPosition);
                joint->bindPose = bindPose;
                joint->inverseBindPose = bindPose.inverse();
				mJoints.push_back(joint);
			}
		}

		//"origin"	-1 ( 0 0 0 ) ( -0.7071067095 0 0 )	

	}
}

void TMD5Model::LoadSubMesh(const char* path, FILE* fp)
{
	char line[512];
    int maxWeightCount = 0;
    
    TMD5Mesh* mesh = new TMD5Mesh();
	while( fgets(line, 512, fp) )
	{
		if( line[0] == '}' )
			break;
		else
		{
			char token[512] = { 0 };
			int s = sscanf(line, "%s", token);
			if( s != EOF )
			{
				if( 0 == strcmp("numverts", token) )
				{

				}
				else if( 0 == strcmp("vert", token) )
				{
                    char name[256];
					char dummy = { 0 };
					float u = -1.0f;
					float v = -1.0f;
					int index = 0;
					int startWeight;
					int weightCount;
					int k = sscanf(line, "%s %d %c %f %f %c %d %d", name, &index, &dummy, &u, &v, &dummy, &startWeight, &weightCount);
                    
                    assert( k != EOF);
                    assert( k == 8 );
					TMD5Vertex vertex;
					vertex.u = u;
					vertex.v = v;
					vertex.startWeight = startWeight;
					vertex.weightCount = weightCount;

                    
                    maxWeightCount = vertex.weightCount>maxWeightCount ? vertex.weightCount : maxWeightCount;
                    
					mesh->mVertices.push_back(vertex);
					
				}
				else if( 0 == strcmp("tri", token) )
				{
					char dummy[512] = { 0 };
					int index = 0;
					int t0, t1, t2;
					int k = sscanf(line, "%s %d %d %d %d", dummy, &index, &t0, &t1, &t2);
                    
                    assert( k != EOF );
                    assert( k == 5 );
                    mesh->mTriangles.push_back(TMD5Triangle(t0, t1, t2));
				}
				else if( 0 == strcmp("weight", token) )
				{
					char dummy[512] = { 0 };
					int weightIndex;
					int jointIndex;
					float weightBias;
					float weightPosition[3];
					int k = sscanf(line, "%s %d %d %f %s %f %f %f", dummy, &weightIndex, &jointIndex, &weightBias, dummy, &weightPosition[0], &weightPosition[1], &weightPosition[2]);
                    
                    assert( k == 8 );
                    
                    assert( k != EOF );

					TMD5Weight weight;
					weight.mJointIndex = jointIndex;
					weight.mBias = weightBias;
					weight.mPosition = Vector3(weightPosition[0], weightPosition[1], weightPosition[2]);
					mesh->mWegihts.push_back(weight);
				}
			}
		}
	}
    printf("**************************************************\n");
    printf("Sub Mesh loaded:%s\n", path);
    printf("Joint count:%ld\n", mJoints.size());
    printf("Vertex Count:%ld\n", mesh->mVertices.size());
    printf("Triangle Count:%ld\n", mesh->mTriangles.size());
    printf("Weight Count:%ld\n", mesh->mWegihts.size());
    printf("Max Weight Count:%d\n", maxWeightCount);
    printf("**************************************************\n");
    
    meshes.push_back(mesh);
    
}

TMD5Joint* TMD5Model::GetRootJoint() const
{
	return mJoints.size() > 0 ? mJoints[0] : NULL;
}

void TMD5Model::RenderBindPoseVertexPositions()
{
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    for(int mesh=0;mesh<meshes.size();++mesh)
    {
    for(int j=0;j<(int)meshes[mesh]->mVertices.size();++j)
    {
        TMD5Vertex* vertex = & meshes[mesh]->mVertices[j];
        //if( joint->mParentId<0)
        //  continue;
        //TMD5Joint* parentJoint = mJoints[joint->mParentId];
        
        Vector3 U = Vector3::UNIT_X;
        Vector3 V = Vector3::UNIT_Y;
        
        Vector3 P = vertex->bindPoseVertexPosition;
        
        Vector3 p0 = P - U - V;
        Vector3 p1 = P + U - V;
        Vector3 p2 = P + U + V;
        Vector3 p3 = P - U + V;
        
        
        TFlatRenderer::TVertexData pt0(p0.x, p0.z, -p0.y, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt1(p1.x, p1.z, -p1.y, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt2(p2.x, p2.z, -p2.y, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt3(p3.x, p3.z, -p3.y, 0.0f, 0.0f);
        
        renderer->RenderTriangle(pt0, pt1, pt2);
        renderer->RenderTriangle(pt0, pt2, pt3);
    }
    renderer->Flush();
    }
}

void TMD5Model::RenderBindPoseSkeleton()
{
    TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    for(int j=0;j<(int)mJoints.size();++j)
    {
        TMD5Joint* joint = mJoints[j];
        //if( joint->mParentId<0)
          //  continue;
        //TMD5Joint* parentJoint = mJoints[joint->mParentId];
        
        Vector3 U = Vector3::UNIT_X;
        Vector3 V = Vector3::UNIT_Y;
        
        Vector3 P = joint->mPositionLoaded;
        
        Vector3 p0 = P - U - V;
        Vector3 p1 = P + U - V;
        Vector3 p2 = P + U + V;
        Vector3 p3 = P - U + V;
        
        
        TFlatRenderer::TVertexData pt0(p0.x, p0.z, -p0.y, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt1(p1.x, p1.z, -p1.y, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt2(p2.x, p2.z, -p2.y, 0.0f, 0.0f);
        TFlatRenderer::TVertexData pt3(p3.x, p3.z, -p3.y, 0.0f, 0.0f);
        
        renderer->RenderTriangle(pt0, pt1, pt2);
        renderer->RenderTriangle(pt0, pt2, pt3);
    }
    renderer->Flush();
}

/*
int TMD5Model::GetVertexCount() const
{
    return (int)mVertices.size();
}
 */


void TMD5Model::Render()
{
	TFlatRenderer* renderer = TFlatRenderer::GetInstance();
    
    
    int maxWeightCount = 0;
    for(int mesh=0;mesh<(int)meshes.size();++mesh)
    {
	for(size_t tri=0;tri<meshes[mesh]->mTriangles.size();++tri)
	{
		TMD5Vertex* vs[3];
		vs[0] = & meshes[mesh]->mVertices[meshes[mesh]->mTriangles[tri].v0];
		vs[1] = & meshes[mesh]->mVertices[meshes[mesh]->mTriangles[tri].v1];
		vs[2] = & meshes[mesh]->mVertices[meshes[mesh]->mTriangles[tri].v2];
		
		Vector3 jps[3];
		jps[0] = Vector3::ZERO;
		jps[1] = Vector3::ZERO;
		jps[2] = Vector3::ZERO;
        
        
        #if 0
        // softtware skinning, quaternion way
		for(int v=0;v<3;++v)
		{				
			int wCount = vs[v]->weightCount;
            maxWeightCount = wCount > maxWeightCount ? wCount:maxWeightCount;
            //wCount = wCount>=4 ? 4:wCount;
			for(int w=0;w<wCount;++w)
			{
				const TMD5Weight& weight = mWegihts[w + vs[v]->startWeight];
				int jointIndex = weight.mJointIndex;

				TMD5Joint* joint = mJoints[jointIndex];
									
				Vector3 wp(weight.mPosition.x, weight.mPosition.y, weight.mPosition.z);
				wp = joint->mOrientation * wp + joint->GetPosition();

				jps[v] += weight.mBias * wp;
			}
		}
        #else
        // software skinning, matrix way
        /*
        for(int v=0;v<3;++v)
        {
            int wCount = vs[v]->weightCount;
            
            
            for(int w=0;w<wCount;++w)
            {
                const TMD5Weight& weight = mWegihts[w + vs[v]->startWeight];
                
                
                TMD5Joint* joint = mJoints[weight.mJointIndex];
                
                Vector4 wp(weight.mPosition.x, weight.mPosition.y, weight.mPosition.z, 0.0f);
                
                wp = joint->transform * wp;
                
                Vector3 jp = joint->transform.getTrans();
                jps[v] += weight.mBias * (jp+ Vector3(wp.x, wp.y, wp.z));
            }
        }
        #endif
        */
        
        // suitable to easily move to GPU skinning
        
#if 1
        for(int v=0;v<3;++v)
        {
            int wCount = vs[v]->weightCount;
            Vector4 bvp = Vector4(vs[v]->bindPoseVertexPosition.x, vs[v]->bindPoseVertexPosition.y, vs[v]->bindPoseVertexPosition.z, 1.0f);
            
            for(int w=0;w<wCount;++w)
            {
                const TMD5Weight& weight = meshes[mesh]->mWegihts[w + vs[v]->startWeight];
                
                TMD5Joint* joint = mJoints[weight.mJointIndex];
               
                Vector4 ppp = joint->transform * joint->inverseBindPose * bvp * weight.mBias;
                
                jps[v] += Vector3(ppp.x, ppp.y, ppp.z);
               
            }
        }
#endif
        #endif
        float u1 = vs[0]->u;
		float v1 = 1.0f - vs[0]->v;

		float u2 = vs[1]->u;
		float v2 = 1.0f - vs[1]->v;

		float u3 = vs[2]->u;
		float v3 = 1.0f - vs[2]->v;

        
        
        TFlatRenderer::TVertexData pt0(jps[0].x, jps[0].z, -jps[0].y, u1, v1);
        TFlatRenderer::TVertexData pt1(jps[1].x, jps[1].z, -jps[1].y, u2, v2);
        TFlatRenderer::TVertexData pt2(jps[2].x, jps[2].z, -jps[2].y, u3, v3);
    	
		renderer->RenderTriangle(pt0, pt1, pt2);


	
		
	}

	renderer->Flush();
    }
}


void TMD5Model::Update(float dt)
{
    if( ! mAnimation )
        return;

	mPlayedTime += dt; // HACK
    
    //mPlayedTime += dt;
    UpdateJoints();
}

void TMD5Model::Eval(const TMD5JointInfo& info, int frame, Vector3& pos, Vector3& orientation)
{
    int index = info.mStartIndex;
    if(	info.mFlags & 1 )
    {
        pos.x = mAnimation->mFrameData[frame].mData[index];
        ++ index;
    }
    if(	info.mFlags & 2 )
    {
        pos.y = mAnimation->mFrameData[frame].mData[index];
        ++ index;
    }
    if(	info.mFlags & 4 )
    {
        pos.z = mAnimation->mFrameData[frame].mData[index];
        ++ index;
    }
    if(	info.mFlags & 8 )
    {
        orientation.x = mAnimation->mFrameData[frame].mData[index];
        ++ index;
    }
    if(	info.mFlags & 16 )
    {
        orientation.y = mAnimation->mFrameData[frame].mData[index];
        ++ index;
    }
    if(	info.mFlags & 32 )
    {
        orientation.z = mAnimation->mFrameData[frame].mData[index];
        ++ index;
    }
}

void TMD5Model::UpdateJoints()
{

    if( ! mAnimation )
        return;
    
    //float fps = mAnimation->mFrameRate;
    int frameCount = (int)mAnimation->mFrameData.size();
    
    float totalTime = (1.0f / mAnimation->mFrameRate) * frameCount;
    
    
    if( mPlayedTime > totalTime )
        mPlayedTime = fmod(mPlayedTime, totalTime);
    
   // mPlayedTime = 0.0f;// TODO:HACK testing
    
    float t = fmod(mPlayedTime, totalTime);
    t /= totalTime;
    
    assert( t>=0.0f && t<=1.0f );
            
    int frame0 = (int)(t * frameCount);
    
    if( frame0 >= mAnimation->mFrameData.size() )
        frame0 = 0;
    
    int frame1 = frame0 + 1;
    if( frame1 >= mAnimation->mFrameData.size()-1 )
        frame1 = (int)mAnimation->mFrameData.size()-1;

    size_t baseFrames = mAnimation->mBaseFrames.size();
    assert( mAnimation->mJointInfos.size() == baseFrames );
    for(size_t j=0;j<baseFrames;++j)
    {
        Vector3 position0 = mAnimation->mBaseFrames[j].mPosition;
        float qx = mAnimation->mBaseFrames[j].mOrientation.x;
        float qy = mAnimation->mBaseFrames[j].mOrientation.y;			
        float qz = mAnimation->mBaseFrames[j].mOrientation.z;

        Vector3 orientation0(qx, qy, qz);

        TMD5JointInfo& info = mAnimation->mJointInfos[j];

        
        Vector3 position1 = position0;
        Vector3 orientation1 = orientation0;
        
        Eval(info, frame0, position0, orientation0); //TODO: HACK commented out to test stuff, uncomment after test
        Eval(info, frame1, position1, orientation1);
      
        Quaternion rotation0 = BuildQuaternion(orientation0.x, orientation0.y, orientation0.z);
        Quaternion rotation1 = BuildQuaternion(orientation1.x, orientation1.y, orientation1.z);
        
        Vector3 position = position0*(1.0f-t) + position1*(t);
        Quaternion orientation = Quaternion::Slerp(1.0f-t, rotation0, rotation1, true);
        
        if( info.mParentId >= 0 )
        {
            TMD5Joint* parentJoint = mJoints[info.mParentId];
            Quaternion qq = parentJoint->mOrientation;
            Vector3 pp = qq * position;

            position = pp + parentJoint->GetPosition();
            orientation = qq * orientation;				
        }

        mJoints[j]->mPosition = position;
        mJoints[j]->mOrientation = orientation;
        
        Matrix3 rot;
        orientation.ToRotationMatrix(rot);
        
        mJoints[j]->transform = Matrix4(rot);
        mJoints[j]->transform.setTrans(position);
    }
}


TMD5Joint* TMD5Model::GetJoint(int idx) const
{
    assert( idx>=0 && idx<GetJointCount());
    return mJoints[idx];
}

int TMD5Model::GetJointCount() const
{
    return (int)mJoints.size();
}

int TMD5Model::GetMeshCount() const
{
    return (int)meshes.size();
}

TMD5Mesh* TMD5Model::GetMesh(int idx)
{
    return meshes[idx];
}


