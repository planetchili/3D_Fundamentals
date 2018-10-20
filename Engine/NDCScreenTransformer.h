#pragma once
#include "Vec3.h"
#include "Graphics.h"

class NDCScreenTransformer
{
public:
	NDCScreenTransformer()
		:
		xFactor( float( Graphics::ScreenWidth ) / 2.0f ),
		yFactor( float( Graphics::ScreenHeight ) / 2.0f )
	{}
	template<class Vertex>
	Vertex& Transform( Vertex& v ) const
	{
		// perform homo -> ndc on xyz / perspective-correct interpolative divide on all other attributes
		const float wInv = 1.0f / v.pos.w;
		v *= wInv;
		// additional divide for mapped z because it must be interpolated
		// adjust position x,y from perspective normalized space
		// to screen dimension space after perspective divide
		v.pos.x = (v.pos.x + 1.0f) * xFactor;
		v.pos.y = (-v.pos.y + 1.0f) * yFactor;
		// store 1/w in w (we will need the interpolated 1/w
		// so that we can recover the attributes after interp.)
		v.pos.w = wInv;

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