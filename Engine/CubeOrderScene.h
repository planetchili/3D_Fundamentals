#pragma once

#include "Scene.h"
#include "Cube.h"
#include "PubeScreenTransformer.h"
#include "Mat3.h"

class CubeOrderScene : public Scene
{
public:
	CubeOrderScene() = default;
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
		//////////////////////////////////////////////////////
		//      FIXED CUBE
		{
			// generate rotation matrix from euler angles
			const Mat3 rot =
				Mat3::RotationX( theta_x ) *
				Mat3::RotationY( theta_y + PI ) *
				Mat3::RotationZ( theta_z );
			// generate indexed triangle list
			auto triangles = cube.GetTriangles();
			// transform from model space -> world (/view) space
			for( auto& v : triangles.vertices )
			{
				v *= rot;
				v += { 0.0f,0.0f,2.0f };
			}
			// backface culling test (must be done in world (/view) space)
			for( size_t i = 0,
				 end = triangles.indices.size() / 3;
				 i < end; i++ )
			{
				const Vec3& v0 = triangles.vertices[triangles.indices[i * 3]];
				const Vec3& v1 = triangles.vertices[triangles.indices[i * 3 + 1]];
				const Vec3& v2 = triangles.vertices[triangles.indices[i * 3 + 2]];
				triangles.cullFlags[i] = (v1 - v0) % (v2 - v0) * v0 > 0.0f;
			}
			// transform to screen space (includes perspective transform)
			for( auto& v : triangles.vertices )
			{
				pst.Transform( v );
			}
			// draw the mf triangles!
			for( size_t i = 0,
				 end = triangles.indices.size() / 3;
				 i < end; i++ )
			{
				// skip triangles previously determined to be back-facing
				if( !triangles.cullFlags[i] )
				{
					gfx.DrawTriangle(
						triangles.vertices[triangles.indices[i * 3]],
						triangles.vertices[triangles.indices[i * 3 + 1]],
						triangles.vertices[triangles.indices[i * 3 + 2]],
						colors[i] );
				}
			}
		}

		//////////////////////////////////////////////////////
		//      MOBILE CUBE
		{
			// generate rotation matrix from euler angles
			const Mat3 rot =
				Mat3::RotationX( theta_x ) *
				Mat3::RotationY( theta_y ) *
				Mat3::RotationZ( theta_z );
			// generate indexed triangle list
			auto triangles = cube.GetTriangles();
			// transform from model space -> world (/view) space
			for( auto& v : triangles.vertices )
			{
				v *= rot;
				v += { 0.0f,0.0f,offset_z };
			}
			// backface culling test (must be done in world (/view) space)
			for( size_t i = 0,
				 end = triangles.indices.size() / 3;
				 i < end; i++ )
			{
				const Vec3& v0 = triangles.vertices[triangles.indices[i * 3]];
				const Vec3& v1 = triangles.vertices[triangles.indices[i * 3 + 1]];
				const Vec3& v2 = triangles.vertices[triangles.indices[i * 3 + 2]];
				triangles.cullFlags[i] = (v1 - v0) % (v2 - v0) * v0 > 0.0f;
			}
			// transform to screen space (includes perspective transform)
			for( auto& v : triangles.vertices )
			{
				pst.Transform( v );
			}
			// draw the mf triangles!
			for( size_t i = 0,
				 end = triangles.indices.size() / 3;
				 i < end; i++ )
			{
				// skip triangles previously determined to be back-facing
				if( !triangles.cullFlags[i] )
				{
					gfx.DrawTriangle(
						triangles.vertices[triangles.indices[i * 3]],
						triangles.vertices[triangles.indices[i * 3 + 1]],
						triangles.vertices[triangles.indices[i * 3 + 2]],
						colors[i] );
				}
			}
		}
	}
private:
	PubeScreenTransformer pst;
	Cube cube = Cube( 1.0f );
	static constexpr Color colors[12] = {
		Colors::White,
		Colors::Blue,
		Colors::Cyan,
		Colors::Gray,
		Colors::Green,
		Colors::Magenta,
		Colors::LightGray,
		Colors::Red,
		Colors::Yellow,
		Colors::White,
		Colors::Blue,
		Colors::Cyan
	};
	static constexpr float dTheta = PI;
	float offset_z = 2.0f;
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
};