#pragma once

#include "Scene.h"
#include "Cube.h"
#include "Mat.h"
#include "Pipeline.h"
#include "SpecularPhongPointEffect.h"
#include "SolidEffect.h"
#include "Sphere.h"
#include "MouseTracker.h"

class SpecularPhongPointScene : public Scene
{
public:
	typedef ::Pipeline<SpecularPhongPointEffect> Pipeline;
	typedef ::Pipeline<SolidEffect> LightIndicatorPipeline;
	typedef Pipeline::Vertex Vertex;
public:
	SpecularPhongPointScene( Graphics& gfx,IndexedTriangleList<Vertex> tl )
		:
		itlist( std::move( tl ) ),
		pZb( std::make_shared<ZBuffer>( gfx.ScreenWidth,gfx.ScreenHeight ) ),
		pipeline( gfx,pZb ),
		liPipeline( gfx,pZb ),
		Scene( "phong point shader scene free mesh" )
	{
		itlist.AdjustToTrueCenter();
		mod_pos.z = itlist.GetRadius() * 1.6f;
		for( auto& v : lightIndicator.vertices )
		{
			v.color = Colors::White;
		}
	}
	virtual void Update( Keyboard& kbd,Mouse& mouse,float dt ) override
	{
		if( kbd.KeyIsPressed( 'W' ) )
		{
			cam_pos += Vec4{ 0.0f,0.0f,1.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'A' ) )
		{
			cam_pos += Vec4{ -1.0f,0.0f,0.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'S' ) )
		{
			cam_pos += Vec4{ 0.0f,0.0f,-1.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'D' ) )
		{
			cam_pos += Vec4{ 1.0f,0.0f,0.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}

		while( !mouse.IsEmpty() )
		{
			const auto e = mouse.Read();
			switch( e.GetType() )
			{
			case Mouse::Event::Type::LPress:
				mt.Engage( e.GetPos() );
				break;
			case Mouse::Event::Type::LRelease:
				mt.Release();
				break;
			case Mouse::Event::Type::Move:
				if( mt.Engaged() )
				{
					const auto delta = mt.Move( e.GetPos() );
					cam_rot_inv = cam_rot_inv
						* Mat4::RotationY( (float)-delta.x * htrack )
						* Mat4::RotationX( (float)-delta.y * vtrack );
				}
				break;
			}
		}
	}
	virtual void Draw() override
	{
		pipeline.BeginFrame();

		const auto proj = Mat4::ProjectionHFOV( hfov,aspect_ratio,0.5f,4.0f );
		const auto view = Mat4::Translation( -cam_pos ) * cam_rot_inv;
		// set pipeline transform
		pipeline.effect.vs.BindWorld(
			Mat4::RotationX( theta_x ) *
			Mat4::RotationY( theta_y ) *
			Mat4::RotationZ( theta_z ) *
			Mat4::Translation( mod_pos )
		);
		pipeline.effect.vs.BindView( view );
		pipeline.effect.vs.BindProjection( proj );
		pipeline.effect.ps.SetLightPosition( l_pos * view );
		// render triangles
		pipeline.Draw( itlist );

		// draw light indicator with different pipeline
		// don't call beginframe on this pipeline b/c wanna keep zbuffer contents
		// (don't like this assymetry but we'll live with it for now)
		liPipeline.effect.vs.BindWorldView( Mat4::Translation( l_pos ) * view );
		liPipeline.effect.vs.BindProjection( proj );
		liPipeline.Draw( lightIndicator );
	}
private:
	IndexedTriangleList<Vertex> itlist;
	IndexedTriangleList<SolidEffect::Vertex> lightIndicator = Sphere::GetPlain<SolidEffect::Vertex>( 0.05f );
	std::shared_ptr<ZBuffer> pZb;
	Pipeline pipeline;
	LightIndicatorPipeline liPipeline;
	MouseTracker mt;
	// fov
	static constexpr float aspect_ratio = 1.33333f;
	static constexpr float hfov = 95.0f;
	static constexpr float vfov = hfov / aspect_ratio;
	// camera stuff
	static constexpr float htrack = to_rad( hfov ) / (float)Graphics::ScreenWidth;
	static constexpr float vtrack = to_rad( vfov ) / (float)Graphics::ScreenHeight;
	static constexpr float cam_speed = 1.0f;
	Vec3 cam_pos = { 0.0f,0.0f,0.0f };
	Mat4 cam_rot_inv = Mat4::Identity();
	// model stuff
	Vec3 mod_pos = { 0.0f,0.0f,2.0f };
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	// light stuff
	Vec4 l_pos = { 0.0f,0.0f,0.6f,1.0f };
};
