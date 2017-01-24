#pragma once
#pragma once

#include "Scene.h"
#include "ConcaveHexahedron.h"
#include "PubeScreenTransformer.h"
#include "Mat3.h"

class ConHexWireScene : public Scene
{
public:
	ConHexWireScene() = default;
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
		// generate indexed triangle list
		auto triangles = hex.GetTriangles();
		// generate indexed line list
		auto lines = hex.GetLines();
		// generate rotation matrix from euler angles
		const Mat3 rot =
			Mat3::RotationX( theta_x ) *
			Mat3::RotationY( theta_y ) *
			Mat3::RotationZ( theta_z );
		// transform from model space -> world (/view) space
		for( auto& v : triangles.vertices )
		{
			v *= rot;
			v += { 0.0f,0.0f,offset_z };
		}
		for( auto& v : lines.vertices )
		{
			v *= rot;
			v += { 0.0f,0.0f,offset_z };
		}
		// transform to screen space (includes perspective transform)
		for( auto& v : triangles.vertices )
		{
			pst.Transform( v );
		}
		for( auto& v : lines.vertices )
		{
			pst.Transform( v );
		}
		// draw the mf triangles!
		for( size_t i = 0,
			 end = triangles.indices.size() / 3;
			 i < end; i++ )
		{
			gfx.DrawTriangle(
				triangles.vertices[triangles.indices[i * 3]],
				triangles.vertices[triangles.indices[i * 3 + 1]],
				triangles.vertices[triangles.indices[i * 3 + 2]],
				colorSolid );
		}
		// draw the mf lines!
		for( size_t i = 0,
			 end = lines.indices.size() / 2;
			 i < end; i++ )
		{
			gfx.DrawLine(
				lines.vertices[lines.indices[i * 2]],
				lines.vertices[lines.indices[i * 2 + 1]],
				colorWire );
		}
	}
private:
	PubeScreenTransformer pst;
	ConcaveHexahedron hex = ConcaveHexahedron( 1.0f );
	static constexpr Color colorSolid = Colors::Gray;
	static constexpr Color colorWire = Colors::Magenta;
	static constexpr float dTheta = PI;
	float offset_z = 2.0f;
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
};