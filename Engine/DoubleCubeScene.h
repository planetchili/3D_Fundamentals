#pragma once

#include "Scene.h"
#include "Cube.h"
#include "Mat.h"
#include "Pipeline.h"
#include "SolidEffect.h"

class DoubleCubeScene : public Scene
{
public:
	typedef Pipeline<SolidEffect> Pipeline;
	typedef Pipeline::Vertex Vertex;
public:
	DoubleCubeScene( Graphics& gfx )
		:
		itlist( Cube::GetPlainIndependentFaces<Vertex>() ),
		pipeline( gfx ),
		Scene( "Double cube interpenetration test scene" )
	{
		const Color colors[] = {
			Colors::Red,Colors::Green,Colors::Blue,Colors::Magenta,Colors::Yellow,Colors::Cyan
		};

		for( int i = 0; i < itlist.vertices.size(); i++ )
		{
			itlist.vertices[i].color = colors[i / 4];
		}
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
		// draw fixed cube
		{
			// generate rotation matrix from euler angles
			// rotate in opposition to mobile cube
			const Mat3 rot =
				Mat3::RotationX( -theta_x ) *
				Mat3::RotationY( -theta_y ) *
				Mat3::RotationZ( -theta_z );
			// set pipeline transform
			pipeline.effect.vs.BindRotation( rot );
			pipeline.effect.vs.BindTranslation( { 0.0f,0.0f,2.0f } );
			// render triangles
			pipeline.Draw( itlist );
		}
		// draw mobile cube
		{
			// generate rotation matrix from euler angles
			const Mat3 rot =
				Mat3::RotationX( theta_x ) *
				Mat3::RotationY( theta_y ) *
				Mat3::RotationZ( theta_z );
			// set pipeline transform
			pipeline.effect.vs.BindRotation( rot );
			pipeline.effect.vs.BindTranslation( { 0.0f,0.0f,offset_z } );
			// render triangles
			pipeline.Draw( itlist );
		}
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