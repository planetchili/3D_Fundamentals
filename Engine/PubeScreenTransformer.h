#pragma once
#include "Vec3.h"
#include "Graphics.h"

class PubeScreenTransformer
{
public:
	PubeScreenTransformer()
		:
		xFactor( float( Graphics::ScreenWidth ) / 2.0f ),
		yFactor( float( Graphics::ScreenHeight ) / 2.0f )
	{}
	template<class Vertex>
	Vertex& Transform( Vertex& v ) const
	{
		const float zInv = 1.0f / v.pos.z;
		// divide all position components and attributes by z
		// (we want to be interpolating our attributes in the
		//  same space where the x,y interpolation is taking
		//  place to prevent distortion)
		v *= zInv;
		// adjust position x,y from perspective normalized space
		// to screen dimension space after perspective divide
		v.pos.x = (v.pos.x + 1.0f) * xFactor;
		v.pos.y = (-v.pos.y + 1.0f) * yFactor;
		// store 1/z in z (we will need the interpolated 1/z
		// so that we can recover the attributes after interp.)
		v.pos.z = zInv;

		return v;
	}
	template<class Vertex>
	Vertex GetTransformed( const Vertex& v ) const
	{
		return Transform( Vertex( v ) );
	}
private:
	float xFactor;
	float yFactor;
};