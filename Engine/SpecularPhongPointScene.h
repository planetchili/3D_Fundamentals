#pragma once

#include "Scene.h"
#include "Cube.h"
#include "Mat.h"
#include "Pipeline.h"
#include "SpecularPhongPointEffect.h"
#include "SolidEffect.h"
#include "Sphere.h"
#include "MouseTracker.h"
#include "VertexLightTexturedEffect.h"
#include "RippleVertexSpecularPhongEffect.h"
#include "Plane.h"

struct PointDiffuseParams
{
	static constexpr float linear_attenuation = 0.9f;
	static constexpr float quadradic_attenuation = 0.6f;
	static constexpr float constant_attenuation = 0.682f;
};

struct SpecularParams
{
	static constexpr float specular_power = 30.0f;
	static constexpr float specular_intensity = 0.6f;
};

class SpecularPhongPointScene : public Scene
{
	using SpecularPhongPointEffect = SpecularPhongPointEffect<PointDiffuseParams,SpecularParams>;
	using VertexLightTexturedEffect = VertexLightTexturedEffect<PointDiffuseParams>;
	using RippleVertexSpecularPhongEffect = RippleVertexSpecularPhongEffect<PointDiffuseParams,SpecularParams>;
public:
	struct Wall
	{
		const Surface* pTex;
		IndexedTriangleList<VertexLightTexturedEffect::Vertex> model;
		Mat4 world;
	};
public:
	typedef ::Pipeline<SpecularPhongPointEffect> Pipeline;
	typedef ::Pipeline<SolidEffect> LightIndicatorPipeline;
	typedef ::Pipeline<VertexLightTexturedEffect> WallPipeline;
	typedef ::Pipeline<RippleVertexSpecularPhongEffect> RipplePipeline;
	typedef Pipeline::Vertex Vertex;
public:
	SpecularPhongPointScene( Graphics& gfx )
		:
		pZb( std::make_shared<ZBuffer>( gfx.ScreenWidth,gfx.ScreenHeight ) ),
		pipeline( gfx,pZb ),
		liPipeline( gfx,pZb ),
		wPipeline( gfx,pZb ),
		rPipeline( gfx,pZb ),
		Scene( "phong point shader scene free mesh" )
	{
		// adjust suzanne model
		itlist.AdjustToTrueCenter();
		// set light sphere colors
		for( auto& v : lightIndicator.vertices )
		{
			v.color = Colors::White;
		}
		// load ceiling/walls/floor
		walls.push_back( {
			&tCeiling,
			Plane::GetSkinnedNormals<VertexLightTexturedEffect::Vertex>( 20,20,width,width,tScaleCeiling ),
			Mat4::RotationX( -PI / 2.0f ) * Mat4::Translation( 0.0f,height / 2.0f,0.0f )
		} );
		for( int i = 0; i < 4; i++ )
		{
			walls.push_back( {
				&tWall,
				Plane::GetSkinnedNormals<VertexLightTexturedEffect::Vertex>( 20,20,width,height,tScaleWall ),
				Mat4::Translation( 0.0f,0.0f,width / 2.0f ) * Mat4::RotationY( float( i ) * PI / 2.0f )
			} );
		}
		walls.push_back( {
			&tFloor,
			Plane::GetSkinnedNormals<VertexLightTexturedEffect::Vertex>( 20,20,width,width,tScaleFloor ),
			Mat4::RotationX( PI / 2.0 ) * Mat4::Translation( 0.0f,-height / 2.0f,0.0f )
		} );
	}
	virtual void Update( Keyboard& kbd,Mouse& mouse,float dt ) override
	{
		t += dt;

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
		if( kbd.KeyIsPressed( 'C' ) )
		{
			cam_pos += Vec4{ 0.0f,1.0f,0.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'Z' ) )
		{
			cam_pos += Vec4{ 0.0f,-1.0f,0.0f,0.0f } * !cam_rot_inv * cam_speed * dt;
		}
		if( kbd.KeyIsPressed( 'Q' ) )
		{
			cam_rot_inv = cam_rot_inv * Mat4::RotationZ( cam_roll_speed * dt );
		}
		if( kbd.KeyIsPressed( 'E' ) )
		{
			cam_rot_inv = cam_rot_inv * Mat4::RotationZ( -cam_roll_speed * dt );
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

		theta_y = wrap_angle( t * rotspeed );
		l_pos.y = l_height_amplitude * sin( wrap_angle( (PI / (2.0f * l_height_amplitude)) * t ) );

		rPipeline.effect.vs.SetTime( t );
	}
	virtual void Draw() override
	{
		pipeline.BeginFrame();

		const auto proj = Mat4::ProjectionHFOV( hfov,aspect_ratio,0.2f,6.0f );
		const auto view = Mat4::Translation( -cam_pos ) * cam_rot_inv;

		// render suzanne
		pipeline.effect.vs.BindWorldView(
			Mat4::RotationX( theta_x ) *
			Mat4::RotationY( theta_y ) *
			Mat4::RotationZ( theta_z ) *
			Mat4::Scaling( scale ) *
			Mat4::Translation( mod_pos ) *
			view
		);
		pipeline.effect.vs.BindProjection( proj );
		pipeline.effect.ps.SetLightPosition( l_pos * view );
		pipeline.effect.ps.SetAmbientLight( l_ambient );
		pipeline.effect.ps.SetDiffuseLight( l );
		pipeline.Draw( itlist );

		// draw light indicator with different pipeline
		// don't call beginframe on this pipeline b/c wanna keep zbuffer contents
		// (don't like this assymetry but we'll live with it for now)
		liPipeline.effect.vs.BindWorldView( Mat4::Translation( l_pos ) * view );
		liPipeline.effect.vs.BindProjection( proj );
		liPipeline.Draw( lightIndicator );

		// draw walls (ceiling floor)
		wPipeline.effect.vs.SetLightPosition( l_pos * view );
		wPipeline.effect.vs.BindProjection( proj );
		wPipeline.effect.vs.SetAmbientLight( l_ambient );
		wPipeline.effect.vs.SetDiffuseLight( l );
		for( const auto& w : walls )
		{
			wPipeline.effect.vs.BindWorldView( w.world * view );
			wPipeline.effect.ps.BindTexture( *w.pTex );
			wPipeline.Draw( w.model );
		}

		// draw ripple plane
		rPipeline.effect.ps.BindTexture( tSauron );
		rPipeline.effect.ps.SetLightPosition( l_pos * view );
		rPipeline.effect.vs.BindWorldView( sauronWorld * view );
		rPipeline.effect.vs.BindProjection( proj );
		rPipeline.effect.ps.SetAmbientLight( l_ambient );
		rPipeline.effect.ps.SetDiffuseLight( l );
		rPipeline.Draw( sauron );
	}
private:
	float t = 0.0f;
	// scene params
	static constexpr float width = 4.0f;
	static constexpr float height = 1.75f;
	// pipelines
	std::shared_ptr<ZBuffer> pZb;
	Pipeline pipeline;
	LightIndicatorPipeline liPipeline;
	WallPipeline wPipeline;
	RipplePipeline rPipeline;
	// fov
	static constexpr float aspect_ratio = 1.33333f;
	static constexpr float hfov = 85.0f;
	static constexpr float vfov = hfov / aspect_ratio;
	// camera stuff
	MouseTracker mt;
	static constexpr float htrack = to_rad( hfov ) / (float)Graphics::ScreenWidth;
	static constexpr float vtrack = to_rad( vfov ) / (float)Graphics::ScreenHeight;
	static constexpr float cam_speed = 1.0f;
	static constexpr float cam_roll_speed = PI;
	Vec3 cam_pos = { 0.0f,0.0f,0.0f };
	Mat4 cam_rot_inv = Mat4::Identity();
	// suzanne model stuff
	IndexedTriangleList<Vertex> itlist = IndexedTriangleList<SpecularPhongPointScene::Vertex>::LoadNormals( "models\\suzanne.obj" );
	Vec3 mod_pos = { 1.2f,-0.4f,1.2f };
	float theta_x = 0.0f;
	float theta_y = 0.0f;
	float theta_z = 0.0f;
	float rotspeed = PI / 4.0f;
	float scale = 0.4;
	// light stuff
	IndexedTriangleList<SolidEffect::Vertex> lightIndicator = Sphere::GetPlain<SolidEffect::Vertex>( 0.05f );
	static constexpr float l_height_amplitude = 0.7f;
	static constexpr float l_height_period = 3.713f;
	Vec4 l_pos = { 0.0f,0.0f,0.0f,1.0f };
	Vec3 l = { 1.0f,1.0f,1.0f };
	Vec3 l_ambient = { 0.35f,0.35f,0.35f };
	// wall stuff
	static constexpr float tScaleCeiling = 0.5f;
	static constexpr float tScaleWall = 0.65f;
	static constexpr float tScaleFloor = 0.65f;
	Surface tCeiling = Surface::FromFile( L"Images\\ceiling.png" );
	Surface tWall = Surface::FromFile( L"Images\\stonewall.png" );
	Surface tFloor = Surface::FromFile( L"Images\\floor.png" );
	std::vector<Wall> walls;
	// ripple stuff
	static constexpr float sauronSize = 0.6f;
	Mat4 sauronWorld = Mat4::RotationX( PI / 2.0f ) * Mat4::Translation( 0.3f,-0.8,0.0f );
	Surface tSauron = Surface::FromFile( L"Images\\sauron-bhole-100x100.png" );
	IndexedTriangleList<RippleVertexSpecularPhongEffect::Vertex> sauron = Plane::GetSkinned<RippleVertexSpecularPhongEffect::Vertex>( 50,10,sauronSize,sauronSize,0.6f );
};
