#include "OBB2D.h"


//------------------------------------------------------------------------------
bool OBB2D::Overlaps1Way(const OBB2D & other) const
{
    for (int a = 0; a < 2; ++a) 
	{
		double t = other.corner[0].dotProduct( axis[a] );

        // Find the extent of box 2 on axis a
        double tMin = t;
        double tMax = t;

        for (int c = 1; c < 4; ++c) 
		{
			t = other.corner[c].dotProduct( axis[a] );

            if (t < tMin) {
                tMin = t;
            } else if (t > tMax) {
                tMax = t;
            }
        }

        // We have to subtract off the origin

        // See if [tMin, tMax] intersects [0, 1]
        if ((tMin > 1 + origin[a]) || (tMax < origin[a])) 
		{
            return false;
        }
    }

    return true;
}


//------------------------------------------------------------------------------
void OBB2D::ComputeAxes()
{
    axis[0] = corner[1] - corner[0]; 
    axis[1] = corner[3] - corner[0]; 

    // Make the length of each axis 1/edge length so we know any
    // dot product must be less than 1 to fall within the edge.

    for (int a = 0; a < 2; ++a) 
	{
		float sl = axis[a].length() * axis[a].length();
		axis[ a ].x *= 1.0f / sl;
		axis[ a ].y *= 1.0f / sl;
        origin[a] = corner[0].dotProduct( axis[a] );
    }
}


//------------------------------------------------------------------------------
OBB2D::OBB2D(const Vector2 & center, float w, float h, float angle)
{
	Vector2 X( cos(angle), sin(angle));
	Vector2 Y(-sin(angle), cos(angle));

	X = X * (w / 2);
	Y = Y * (h / 2);

	corner[0] = center - X - Y;
	corner[1] = center + X - Y;
	corner[2] = center + X + Y;
	corner[3] = center - X + Y;

	ComputeAxes();
}


//------------------------------------------------------------------------------
OBB2D::OBB2D(const Vector2 & center, const Vector2 & u , const Vector2 & v, float halfwidth, float halfheight)
{
	origin[0] = center.x;
	origin[1] = center.y;

	float dx = halfwidth;
	float dy = halfheight;

	corner[0] = center - u * dx - v * dy;
	corner[1] = center + u * dx - v * dy;
	corner[2] = center + u * dx + v * dy;
	corner[3] = center - u * dx + v * dy;

	ComputeAxes();
}
   
//------------------------------------------------------------------------------
void OBB2D::MoveTo(const Vector2& center)
{
    Vector2 centroid = (corner[0] + corner[1] + corner[2] + corner[3]) * 0.25f;

    Vector2 translation = center - centroid;

    for (int c = 0; c < 4; ++c) 
	{
        corner[c] += translation;
    }

    ComputeAxes();
}

//------------------------------------------------------------------------------    
bool OBB2D::Overlaps(const OBB2D& other) const
{
	return Overlaps1Way(other) && other.Overlaps1Way(*this);
}


//------------------------------------------------------------------------------
#if 0
void OBB2D::render() const 
{
	glBegin(GL_LINE_LOOP);
	for (int c = 0; c < 4; ++c) 
	{
		glVertex2f( corner[c].x, corner[c].y );
	}
	glEnd();


	glBegin( GL_LINES );

	Vector2 pts[4];
	pts[0] = Vector2( corner[0].x, corner[0].y );
	pts[1] = Vector2( corner[1].x, corner[1].y );
	pts[2] = Vector2( corner[2].x, corner[2].y );
	pts[3] = Vector2( corner[3].x, corner[3].y );
	for(int c=0;c<4;++c)
	{
		Vector2 dir = (pts[ (1 + c)%4 ] - pts[c]).normalisedCopy();
		Vector2 normal( -dir.y, dir.x);
		Vector2 pt0 = 0.5f * ( pts[ (1 + c)%4 ] + pts[c] );
		Vector2 pt1 = pt0 + 2.0f * normal;

		glVertex3f( pt0.x, pt0.y, 0.0f );
		glVertex3f( pt1.x, pt1.y, 0.0f );
	}

	glEnd();

}

#endif

//------------------------------------------------------------------------------
bool TestOBBOBB2D(const OBB2D & obb0, const OBB2D & obb1)
{
	return obb0.Overlaps( obb1 );
}

//------------------------------------------------------------------------------