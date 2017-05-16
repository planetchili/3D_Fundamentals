#pragma once

#pragma once

#include "Scene.h"
#include "Cube.h"
#include "PubeScreenTransformer.h"
#include "Mat3.h"

class TexWrapCubeScene : public Scene
{
public:
	TexWrapCubeScene( float texdim )
		:
		cube( 1.0f,texdim ),
		Scene( std::string( "Textured Cube Butthole Sauron Wrapping Dim: " ) + std::to_string( texdim ) )
	{}
	virtual void Update( Keyboard& kbd,Mouse& mouse,float dt ) override
	{
		if( kbd.KeyIsPressed( 'Q' ) )
		{
			theta_x = wrap_angle( theta_x + dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'W' ) )
		{
			theta_y = wrap_angle( theta_y + dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'E' ) )
		{
			theta_z = wrap_angle( theta_z + dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'A' ) )
		{
			theta_x = wrap_angle( theta_x - dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'S' ) )
		{
			theta_y = wrap_angle( theta_y - dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'D' ) )
		{
			theta_z = wrap_angle( theta_z - dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'R' ) )
		{
			offset_z += 2.0f * dt;
		}
		if( kbd.KeyIsPressed( 'F' ) )
		{
			offset_z -= 2.0f * dt;
		}
	}
	virtual void Draw( Graphics& gfx ) const override
	{
		// NOTE: Changes had to be made because of Vec3 -> TexVertex
		// generate indexed triangle list
		auto triangles = cube.GetTrianglesTex();
		// generate rotation matrix from euler angles
		const Mat3 rot =
			Mat3::RotationX( theta_x ) *
			Mat3::RotationY( theta_y ) *
			Mat3::RotationZ( theta_z );
		// transform from model space -> world (/view) space
		for( auto& v : triangles.vertices )
		{
			v.pos *= rot;
			v.pos += { 0.0f,0.0f,offset_z };
		}
		// backface culling test (must be done in world (/view) space)
		for( size_t i = 0,
			 end = triangles.indices.size() / 3;
			 i < end; i++ )
		{
			const Vec3& v0 = triangles.vertices[triangles.indices[i * 3]].pos;
			const Vec3& v1 = triangles.vertices[triangles.indices[i * 3 + 1]].pos;
			const Vec3& v2 = triangles.vertices[triangles.indices[i * 3 + 2]].pos;
			triangles.cullFlags[i] = (v1 - v0) % (v2 - v0) * v0 > 0.0f;
		}
		// transform to screen space (includes perspective transform)
		for( auto& v : triangles.vertices )
		{
			pst.Transform( v.pos );
		}
		// draw the mf triangles!
		for( size_t i = 0,
			 end = triangles.indices.size() / 3;
			 i < end; i++ )
		{
			// skip triangles previously determined to be back-facing
			if( !triangles.cullFlags[i] )
			{
				gfx.DrawTriangleTexWrap(
					triangles.vertices[triangles.indices[i * 3]],
					triangles.vertices[triangles.indices[i * 3 + 1]],
					triangles.vertices[triangles.indices[i * 3 + 2]],
					sbTex );
			}
		}
	}
private:
	PubeScreenTransformer pst;
	Cube cube;
	Surface sbTex = Surface::FromFile( L"Images\\sauron-bhole-100x100.png" );
	static constexpr float dTheta = PI;
	float offset_z = 2.0f;
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
};