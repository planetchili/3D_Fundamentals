#pragma once

#include "Vec3.h"
#include "IndexedTriangleList.h"

class Cube
{
public:
	template<class V>
	static IndexedTriangleList<V> GetPlain( float size = 1.0f )
	{
		const float side = size / 2.0f;

		std::vector<Vec3> vertices;

		vertices.emplace_back( -side,-side,-side ); // 0
		vertices.emplace_back( side,-side,-side ); // 1
		vertices.emplace_back( -side,side,-side ); // 2
		vertices.emplace_back( side,side,-side ); // 3
		vertices.emplace_back( -side,-side,side ); // 4
		vertices.emplace_back( side,-side,side ); // 5
		vertices.emplace_back( -side,side,side ); // 6
		vertices.emplace_back( side,side,side ); // 7

		std::vector<V> verts( vertices.size() );
		for( size_t i = 0; i < vertices.size(); i++ )
		{
			verts[i].pos = vertices[i];
		}
		return{
			std::move( verts ),{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}
	template<class V>
	static IndexedTriangleList<V> GetPlainIndependentFaces( float size = 1.0f )
	{
		const float side = size / 2.0f;

		std::vector<Vec3> vertices;

		vertices.emplace_back( -side,-side,-side ); // 0 near side
		vertices.emplace_back( side,-side,-side ); // 1
		vertices.emplace_back( -side,side,-side ); // 2
		vertices.emplace_back( side,side,-side ); // 3
		vertices.emplace_back( -side,-side,side ); // 4 far side
		vertices.emplace_back( side,-side,side ); // 5
		vertices.emplace_back( -side,side,side ); // 6
		vertices.emplace_back( side,side,side ); // 7
		vertices.emplace_back( -side,-side,-side ); // 8 left side
		vertices.emplace_back( -side,side,-side ); // 9
		vertices.emplace_back( -side,-side,side ); // 10
		vertices.emplace_back( -side,side,side ); // 11
		vertices.emplace_back( side,-side,-side ); // 12 right side
		vertices.emplace_back( side,side,-side ); // 13
		vertices.emplace_back( side,-side,side ); // 14
		vertices.emplace_back( side,side,side ); // 15
		vertices.emplace_back( -side,-side,-side ); // 16 bottom side
		vertices.emplace_back( side,-side,-side ); // 17
		vertices.emplace_back( -side,-side,side ); // 18
		vertices.emplace_back( side,-side,side ); // 19
		vertices.emplace_back( -side,side,-side ); // 20 top side
		vertices.emplace_back( side,side,-side ); // 21
		vertices.emplace_back( -side,side,side ); // 22
		vertices.emplace_back( side,side,side ); // 23

		std::vector<V> verts( vertices.size() );
		for( size_t i = 0; i < vertices.size(); i++ )
		{
			verts[i].pos = vertices[i];
		}
		return{
			std::move( verts ),{
				0,2, 1,    2,3,1,
				4,5, 7,    4,7,6,
				8,10, 9,  10,11,9,
				12,13,15, 12,15,14,
				16,17,18, 18,17,19,
				20,23,21, 20,22,23
			}
		};
	}
	template<class V>
	static IndexedTriangleList<V> GetIndependentFacesNormals( float size = 1.0f )
	{
		auto cube = GetPlainIndependentFaces<V>( size );

		std::vector<Vec3> vertices;

		//vertices.emplace_back( -side,-side,-side ); // 0 near side
		//vertices.emplace_back( side,-side,-side ); // 1
		//vertices.emplace_back( -side,side,-side ); // 2
		//vertices.emplace_back( side,side,-side ); // 3
		cube.vertices[0].n = { 0.0f,0.0f,-1.0f };
		cube.vertices[1].n = { 0.0f,0.0f,-1.0f };
		cube.vertices[2].n = { 0.0f,0.0f,-1.0f };
		cube.vertices[3].n = { 0.0f,0.0f,-1.0f };
		//vertices.emplace_back( -side,-side,side ); // 4 far side
		//vertices.emplace_back( side,-side,side ); // 5
		//vertices.emplace_back( -side,side,side ); // 6
		//vertices.emplace_back( side,side,side ); // 7
		cube.vertices[4].n = { 0.0f,0.0f,1.0f };
		cube.vertices[5].n = { 0.0f,0.0f,1.0f };
		cube.vertices[6].n = { 0.0f,0.0f,1.0f };
		cube.vertices[7].n = { 0.0f,0.0f,1.0f };
		//vertices.emplace_back( -side,-side,-side ); // 8 left side
		//vertices.emplace_back( -side,side,-side ); // 9
		//vertices.emplace_back( -side,-side,side ); // 10
		//vertices.emplace_back( -side,side,side ); // 11
		cube.vertices[8].n = { -1.0f,0.0f,0.0f };
		cube.vertices[9].n = { -1.0f,0.0f,0.0f };
		cube.vertices[10].n = { -1.0f,0.0f,0.0f };
		cube.vertices[11].n = { -1.0f,0.0f,0.0f };
		//vertices.emplace_back( side,-side,-side ); // 12 right side
		//vertices.emplace_back( side,side,-side ); // 13
		//vertices.emplace_back( side,-side,side ); // 14
		//vertices.emplace_back( side,side,side ); // 15
		cube.vertices[12].n = { 1.0f,0.0f,0.0f };
		cube.vertices[13].n = { 1.0f,0.0f,0.0f };
		cube.vertices[14].n = { 1.0f,0.0f,0.0f };
		cube.vertices[15].n = { 1.0f,0.0f,0.0f };
		//vertices.emplace_back( -side,-side,-side ); // 16 bottom side
		//vertices.emplace_back( side,-side,-side ); // 17
		//vertices.emplace_back( -side,-side,side ); // 18
		//vertices.emplace_back( side,-side,side ); // 19
		cube.vertices[16].n = { 0.0f,-1.0f,0.0f };
		cube.vertices[17].n = { 0.0f,-1.0f,0.0f };
		cube.vertices[18].n = { 0.0f,-1.0f,0.0f };
		cube.vertices[19].n = { 0.0f,-1.0f,0.0f };
		//vertices.emplace_back( -side,side,-side ); // 20 top side
		//vertices.emplace_back( side,side,-side ); // 21
		//vertices.emplace_back( -side,side,side ); // 22
		//vertices.emplace_back( side,side,side ); // 23
		cube.vertices[20].n = { 0.0f,1.0f,0.0f };
		cube.vertices[21].n = { 0.0f,1.0f,0.0f };
		cube.vertices[22].n = { 0.0f,1.0f,0.0f };
		cube.vertices[23].n = { 0.0f,1.0f,0.0f };

		return cube;
	}
	template<class V>
	static IndexedTriangleList<V> GetSkinned( float size = 1.0f )
	{
		const float side = size / 2.0f;
		const auto ConvertTexCoord = []( float u,float v )
		{
			return Vec2{ (u + 1.0f) / 3.0f,v / 4.0f };
		};

		std::vector<Vec3> vertices;
		std::vector<Vec2> tc;

		vertices.emplace_back( -side,-side,-side ); // 0
		tc.emplace_back( ConvertTexCoord( 1.0f,0.0f ) );
		vertices.emplace_back( side,-side,-side ); // 1
		tc.emplace_back( ConvertTexCoord( 0.0f,0.0f ) );
		vertices.emplace_back( -side,side,-side ); // 2
		tc.emplace_back( ConvertTexCoord( 1.0f,1.0f ) );
		vertices.emplace_back( side,side,-side ); // 3
		tc.emplace_back( ConvertTexCoord( 0.0f,1.0f ) );
		vertices.emplace_back( -side,-side,side ); // 4
		tc.emplace_back( ConvertTexCoord( 1.0f,3.0f ) );
		vertices.emplace_back( side,-side,side ); // 5
		tc.emplace_back( ConvertTexCoord( 0.0f,3.0f ) );
		vertices.emplace_back( -side,side,side ); // 6
		tc.emplace_back( ConvertTexCoord( 1.0f,2.0f ) );
		vertices.emplace_back( side,side,side ); // 7
		tc.emplace_back( ConvertTexCoord( 0.0f,2.0f ) );
		vertices.emplace_back( -side,-side,-side ); // 8
		tc.emplace_back( ConvertTexCoord( 1.0f,4.0f ) );
		vertices.emplace_back( side,-side,-side ); // 9
		tc.emplace_back( ConvertTexCoord( 0.0f,4.0f ) );
		vertices.emplace_back( -side,-side,-side ); // 10
		tc.emplace_back( ConvertTexCoord( 2.0f,1.0f ) );
		vertices.emplace_back( -side,-side,side ); // 11
		tc.emplace_back( ConvertTexCoord( 2.0f,2.0f ) );
		vertices.emplace_back( side,-side,-side ); // 12
		tc.emplace_back( ConvertTexCoord( -1.0f,1.0f ) );
		vertices.emplace_back( side,-side,side ); // 13
		tc.emplace_back( ConvertTexCoord( -1.0f,2.0f ) );

		std::vector<V> verts( vertices.size() );
		for( size_t i = 0; i < vertices.size(); i++ )
		{
			verts[i].pos = vertices[i];
			verts[i].t = tc[i];
		}

		return{
			std::move( verts ),{
				0,2,1,   2,3,1,
				4,8,5,   5,8,9,
				2,6,3,   3,6,7,
				4,5,7,   4,7,6,
				2,10,11, 2,11,6,
				12,3,7,  12,7,13
			}
		};
	}
};