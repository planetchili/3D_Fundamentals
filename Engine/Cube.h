#pragma once

#include "Vec3.h"
#include <vector>
#include "IndexedLineList.h"
#include "IndexedTriangleList.h"
#include "TexVertex.h"

class Cube
{
public:
	Cube( float size )
	{
		const float side = size / 2.0f;
		vertices.emplace_back( -side,-side,-side ); // 0
		tc.emplace_back( 0.0f,1.0f );
		vertices.emplace_back( side,-side,-side ); // 1
		tc.emplace_back( 1.0f,1.0f );
		vertices.emplace_back( -side,side,-side ); // 2
		tc.emplace_back( 0.0f,0.0f );
		vertices.emplace_back( side,side,-side ); // 3
		tc.emplace_back( 1.0f,0.0f );
		vertices.emplace_back( -side,-side,side ); // 4
		tc.emplace_back( 1.0f,1.0f );
		vertices.emplace_back( side,-side,side ); // 5
		tc.emplace_back( 0.0f,1.0f );
		vertices.emplace_back( -side,side,side ); // 6
		tc.emplace_back( 1.0f,0.0f );
		vertices.emplace_back( side,side,side ); // 7
		tc.emplace_back( 0.0f,0.0f );
	}
	IndexedLineList GetLines() const
	{
		return { 
			vertices,{
			0,1,  1,3,  3,2,  2,0,
			0,4,  1,5,	3,7,  2,6,
			4,5,  5,7,	7,6,  6,4 }
		};
	}
	IndexedTriangleList<Vec3> GetTriangles() const
	{
		return {
			vertices,{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4 }
		};
	}
	IndexedTriangleList<TexVertex> GetTrianglesTex() const
	{
		std::vector<TexVertex> tverts;
		tverts.reserve( vertices.size() );
		for( size_t i = 0; i < vertices.size(); i++ )
		{
			tverts.emplace_back( vertices[i],tc[i] );
		}
		return {
			std::move( tverts ),{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4 }
		};
	}
private:
	std::vector<Vec3> vertices;
	std::vector<Vec2> tc;
};