#pragma once

#include <vector>
#include <array>
#include "Vec2.h"
#include "Vec3.h"
#include "IndexedTriangleList.h"

class Plane
{
public:
	template<class V>
	static IndexedTriangleList<V> GetPlain( int divisions = 7,float size = 1.0f )
	{
		const int nVerticesSide = divisions + 1;
		std::vector<V> vertices( sq( nVerticesSide + 1 ) );
		// give V a ctor for pos only %%%improvements
		{
			const float side = size / 2.0f;
			const float divisionSize = size / float( divisions );
			const Vec3 bottomLeft = { -side,-side,0.0f };

			for( int y = 0,i = 0; y < nVerticesSide; y++ )
			{
				const float y_pos = float( y ) * divisionSize;
				for( int x = 0; x < nVerticesSide; x++,i++ )
				{
					vertices[i].pos = bottomLeft + Vec3{ float( x ) * divisionSize,y_pos,0.0f };
				}
			}
		}
		
		std::vector<size_t> indices;
		indices.reserve( sq( divisions ) * 6 );
		{
			const auto vxy2i = [nVerticesSide]( size_t x,size_t y )
			{
				return y * nVerticesSide + x;
			};
			for( size_t y = 0; y < divisions; y++ )
			{
				for( size_t x = 0; x < divisions; x++ )
				{
					const std::array<size_t,4> indexArray =
					{ vxy2i( x,y ),vxy2i( x + 1,y ),vxy2i( x,y + 1 ),vxy2i( x + 1,y + 1 ) };
					indices.push_back( indexArray[0] );
					indices.push_back( indexArray[2] );
					indices.push_back( indexArray[1] );
					indices.push_back( indexArray[1] );
					indices.push_back( indexArray[2] );
					indices.push_back( indexArray[3] );
				}
			}
		}

		return{ std::move( vertices ),std::move( indices ) };
	}
	template<class V>
	static IndexedTriangleList<V> GetSkinned( int divisions = 7,float size = 1.0f )
	{
		auto itlist = GetPlain<V>( divisions,size );
		{
			const int nVerticesSide = divisions + 1;
			const float tDivisionSize = 1.0f / float( divisions );
			const Vec2 tBottomLeft = { 0.0f,1.0f };

			for( int y = 0,i = 0; y < nVerticesSide; y++ )
			{
				const float y_t = -float( y ) * tDivisionSize;
				for( int x = 0; x < nVerticesSide; x++,i++ )
				{
					itlist.vertices[i].t = tBottomLeft + Vec2{ float( x ) * tDivisionSize,y_t };
				}
			}

		}

		return itlist;
	}
};