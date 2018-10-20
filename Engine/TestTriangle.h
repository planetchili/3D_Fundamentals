#pragma once

#include <vector>
#include <array>
#include "Vec2.h"
#include "Vec3.h"
#include "IndexedTriangleList.h"

class TestTriangle
{
public:
	template<class V>
	static IndexedTriangleList<V> GetPlain()
	{
		std::vector<V> vertices( 3 );
		vertices[0].pos = { 0.0f,0.8f,0.0f };
		vertices[1].pos = { 0.8f,-0.8f,0.0f };
		vertices[2].pos = { -0.8f,-0.8f,0.0f };

		std::vector<size_t> indices;
		indices.reserve( 3 );
		indices.push_back( 0 );
		indices.push_back( 1 );
		indices.push_back( 2 );

		return{ std::move( vertices ),std::move( indices ) };
	}
	template<class V>
	static IndexedTriangleList<V> GetNormals()
	{
		auto itlist = GetPlain<V>();
		for( auto& v : itlist.vertices )
		{
			v.n = { 0.0f,0.0f,-1.0f };
		}

		return itlist;
	}
};
