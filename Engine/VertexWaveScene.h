#pragma once

#pragma once

#pragma once

#include "Scene.h"
#include "Plane.h"
#include "Mat.h"
#include "WaveVertexTextureEffect.h"

class VertexWaveScene : public Scene
{
public:
	typedef Pipeline<WaveVertexTextureEffect> Pipeline;
	typedef Pipeline::Vertex Vertex;
public:
	VertexWaveScene( Graphics& gfx )
		:
		itlist( Plane::GetSkinned<Vertex>( 20 ) ),
		pipeline( gfx ),
		Scene( "Test Plane Rippling VS" )
	{
		pipeline.effect.ps.BindTexture( L"images\\sauron-bhole-100x100.png" );
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
		if( kbd.KeyIsPressed( 'U' ) )
		{
			phi_x = wrap_angle( phi_x + dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'I' ) )
		{
			phi_y = wrap_angle( phi_y + dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'O' ) )
		{
			phi_z = wrap_angle( phi_z + dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'J' ) )
		{
			phi_x = wrap_angle( phi_x - dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'K' ) )
		{
			phi_y = wrap_angle( phi_y - dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'L' ) )
		{
			phi_z = wrap_angle( phi_z - dTheta * dt );
		}
		if( kbd.KeyIsPressed( 'R' ) )
		{
			offset_z += 2.0f * dt;
		}
		if( kbd.KeyIsPressed( 'F' ) )
		{
			offset_z -= 2.0f * dt;
		}
		time += dt;
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
		const Mat3 rot_phi =
			Mat3::RotationX( phi_x ) *
			Mat3::RotationY( phi_y ) *
			Mat3::RotationZ( phi_z );
		const Vec3 trans = { 0.0f,0.0f,offset_z };
		// set pipeline transform
		pipeline.effect.vs.BindRotation( rot );
		pipeline.effect.vs.BindTranslation( trans );
		pipeline.effect.vs.SetTime( time );
		pipeline.effect.gs.SetLightDirection( light_dir * rot_phi );
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
	float time = 0.0f;
	float phi_x = 0.0f;
	float phi_y = 0.0f;
	float phi_z = 0.0f;
	Vec3 light_dir = { 0.2f,-0.5f,1.0f };
};