#pragma once

#pragma once

#include "Vec3.h"
#include <vector>
#include "IndexedLineList.h"
#include "IndexedTriangleList.h"

class ConcaveHexahedron
{
public:
	ConcaveHexahedron( float size )
	{
		const float side = size / 2.0f;
		vertices.emplace_back( 0.0f,side,0.0f );
		vertices.emplace_back( -side,-side,0.0f );
		vertices.emplace_back( side,-side,0.0f );
		vertices.emplace_back( 0.0f,0.0f,side );
		vertices.emplace_back( 0.0f,0.0f,-side );
	}
	IndexedLineList GetLines() const
	{
		return{
			vertices,{
				0,1, 1,3, 3,2, 2,0,
				3,0, 3,4, 1,4, 2,4, 4,0
			}
		};
	}
	IndexedTriangleList GetTriangles() const
	{
		return{
			vertices,{
				1,0,3, 3,0,2,
				1,4,0, 4,2,0,
				3,4,1, 4,3,2
			}
		};
	}
private:
	std::vector<Vec3> vertices;
};