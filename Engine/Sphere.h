#pragma once

#include "Vec3.h"
#include "Mat3.h"
#include "IndexedTriangleList.h"

class Sphere
{
public:
	template<class V>
	static IndexedTriangleList<V> GetPlain( float radius = 1.0f,int latDiv = 12,int longDiv = 24 )
	{
		const Vec3 base = { 0.0f,0.0f,radius };
		const float lattitudeAngle = PI / latDiv;
		const float longitudeAngle = 2.0f * PI / longDiv;

		std::vector<V> vertices;
		for( int iLat = 1; iLat < latDiv; iLat++ )
		{
			const auto latBase = base * Mat3::RotationX( lattitudeAngle * iLat );
			for( int iLong = 0; iLong < longDiv; iLong++ )
			{
				vertices.emplace_back();
				vertices.back().pos = latBase * Mat3::RotationZ( longitudeAngle * iLong );
			}
		}
		
		const auto calcIdx = [latDiv,longDiv]( int iLat,int iLong )
			{ return iLat * longDiv + iLong; };
		std::vector<size_t> indices;
		for( int iLat = 0; iLat < latDiv - 2; iLat++ )
		{
			for( int iLong = 0; iLong < longDiv - 1; iLong++ )
			{
				indices.push_back( calcIdx( iLat,iLong ) );
				indices.push_back( calcIdx( iLat + 1,iLong ) );
				indices.push_back( calcIdx( iLat,iLong + 1 ) );
				indices.push_back( calcIdx( iLat,iLong + 1 ) );
				indices.push_back( calcIdx( iLat + 1,iLong ) );
				indices.push_back( calcIdx( iLat + 1,iLong + 1 ) );
			}
			// wrapping quads
		}

		// cap fans

		return{ std::move( vertices ),std::move( indices ) };
	}
};