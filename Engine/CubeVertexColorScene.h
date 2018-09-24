#pragma once

#include "Scene.h"
#include "Cube.h"
#include "Mat.h"
#include "Pipeline.h"
#include "VertexColorEffect.h"

class CubeVertexColorScene : public Scene
{
public:
	typedef Pipeline<VertexColorEffect> Pipeline;
	typedef Pipeline::Vertex Vertex;
public:
	CubeVertexColorScene( Graphics& gfx )
		:
		itlist( Cube::GetPlain<Vertex>() ),
		pipeline( gfx ),
		Scene( "Colored cube vertex gradient scene" )
	{
		itlist.vertices[0].color = Vec3( Colors::Red );
		itlist.vertices[1].color = Vec3( Colors::Green );
		itlist.vertices[2].color = Vec3( Colors::Blue );
		itlist.vertices[3].color = Vec3( Colors::Yellow );
		itlist.vertices[4].color = Vec3( Colors::Cyan );
		itlist.vertices[5].color = Vec3( Colors::Magenta );
		itlist.vertices[6].color = Vec3( Colors::White );
		itlist.vertices[7].color = Vec3( Colors::Black );
	}
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
	virtual void Draw() override
	{
		pipeline.BeginFrame();
		// generate rotation matrix from euler angles
		// translation from offset
		const Mat3 rot =
			Mat3::RotationX( theta_x ) *
			Mat3::RotationY( theta_y ) *
			Mat3::RotationZ( theta_z );
		const Vec3 trans = { 0.0f,0.0f,offset_z };
		// set pipeline transform
		pipeline.effect.vs.BindRotation( rot );
		pipeline.effect.vs.BindTranslation( trans );
		// render triangles
		pipeline.Draw( itlist );
	}
private:
	IndexedTriangleList<Vertex> itlist;
	Pipeline pipeline;
	static constexpr float dTheta = PI;
	float offset_z = 2.0f;
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
};