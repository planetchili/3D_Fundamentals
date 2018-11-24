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
	static IndexedTriangleList<V> GetPlain( int divisions_x = 7,int divisions_y = 7,float width = 1.0f,float height = 1.0f )
	{
		const int nVertices_x = divisions_x + 1;
		const int nVertices_y = divisions_y + 1;
		std::vector<V> vertices( nVertices_x * nVertices_y );
		// give V a ctor for pos only %%%improvements
		{
			const float side_x = width / 2.0f;
			const float side_y = height / 2.0f;
			const float divisionSize_x = width / float( divisions_x );
			const float divisionSize_y = height / float( divisions_y );
			const Vec3 bottomLeft = { -side_x,-side_y,0.0f };

			for( int y = 0,i = 0; y < nVertices_y; y++ )
			{
				const float y_pos = float( y ) * divisionSize_y;
				for( int x = 0; x < nVertices_x; x++,i++ )
				{
					vertices[i].pos = bottomLeft + Vec3{ float( x ) * divisionSize_x,y_pos,0.0f };
				}
			}
		}
		
		std::vector<size_t> indices;
		indices.reserve( sq( divisions_x * divisions_y ) * 6 );
		{
			const auto vxy2i = [nVertices_x]( size_t x,size_t y )
			{
				return y * nVertices_x + x;
			};
			for( size_t y = 0; y < divisions_y; y++ )
			{
				for( size_t x = 0; x < divisions_x; x++ )
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
	static IndexedTriangleList<V> GetSkinned( int divisions_x = 7,int divisions_y = 7,float width = 1.0f,float height = 1.0f,float tScale = 1.0f )
	{
		auto itlist = GetPlain<V>( divisions_x,divisions_y,width,height );
		{
			const int nVertices_x = divisions_x + 1;
			const int nVertices_y = divisions_y + 1;
			const float tDivisionSize_x = width / float( divisions_x );
			const float tDivisionSize_y = height / float( divisions_y );
			const Vec2 tBottomLeft = { 0.0f,1.0f };

			for( int y = 0,i = 0; y < nVertices_y; y++ )
			{
				const float y_t = (-float( y ) * tDivisionSize_y) / tScale;
				for( int x = 0; x < nVertices_x; x++,i++ )
				{
					itlist.vertices[i].t = tBottomLeft + Vec2{ (float( x ) * tDivisionSize_x) / tScale,y_t };
				}
			}
		}

		return itlist;
	}
	template<class V>
	static IndexedTriangleList<V> GetNormals( int divisions_x = 7,int divisions_y = 7,float width = 1.0f,float height = 1.0f )
	{
		auto itlist = GetPlain<V>( divisions_x,divisions_y,width,height );
		for( auto& v : itlist.vertices )
		{
			v.n = { 0.0f,0.0f,-1.0f };
		}

		return itlist;
	}
	template<class V>
	static IndexedTriangleList<V> GetSkinnedNormals( int divisions_x = 7,int divisions_y = 7,float width = 1.0f,float height = 1.0f,float tScale = 1.0f )
	{
		auto itlist = GetSkinned<V>( divisions_x,divisions_y,width,height,tScale );
		for( auto& v : itlist.vertices )
		{
			v.n = { 0.0f,0.0f,-1.0f };
		}

		return itlist;
	}
};