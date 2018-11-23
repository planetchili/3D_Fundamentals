#pragma once

#include "Pipeline.h"
#include "BaseVertexShader.h"
#include "DefaultGeometryShader.h"
#include "BasePhongShader.h"

// flat shading with vertex normals
class VertexLightTexturedEffect
{
public:
	// the vertex type that will be input into the pipeline
	class Vertex
	{
	public:
		Vertex() = default;
		Vertex( const Vec4& pos )
			:
			pos( pos )
		{}
		Vertex( const Vec4& pos,const Vertex& src )
			:
			n( src.n ),
			pos( pos )
		{}
		Vertex( const Vec4& pos,const Vec3& n )
			:
			n( n ),
			pos( pos )
		{}
	public:
		Vec4 pos;
		Vec3 n;
		Vec2 t;
	};
	// vertex shader
	// output interpolates position, light, and tex coord
	class VSOutput
	{
	public:
		VSOutput() = default;
		VSOutput( const Vec4& pos )
			:
			pos( pos )
		{}
		VSOutput( const Vec4& pos,const VSOutput& src )
			:
			t( src.t ),
			l( src.l ),
			pos( pos )
		{}
		VSOutput( const Vec4& pos,const Vec2& t,const Vec3& l )
			:
			t( t ),
			pos( pos ),
			l( l )
		{}
		VSOutput& operator+=( const VSOutput& rhs )
		{
			pos += rhs.pos;
			t += rhs.t;
			l += rhs.l;
			return *this;
		}
		VSOutput operator+( const VSOutput& rhs ) const
		{
			return VSOutput( *this ) += rhs;
		}
		VSOutput& operator-=( const VSOutput& rhs )
		{
			pos -= rhs.pos;
			t -= rhs.t;
			l -= rhs.l;
			return *this;
		}
		VSOutput operator-( const VSOutput& rhs ) const
		{
			return VSOutput( *this ) -= rhs;
		}
		VSOutput& operator*=( float rhs )
		{
			pos *= rhs;
			t *= rhs;
			l *= rhs;
			return *this;
		}
		VSOutput operator*( float rhs ) const
		{
			return VSOutput( *this ) *= rhs;
		}
		VSOutput& operator/=( float rhs )
		{
			pos /= rhs;
			t /= rhs;
			l /= rhs;
			return *this;
		}
		VSOutput operator/( float rhs ) const
		{
			return VSOutput( *this ) /= rhs;
		}
	public:
		Vec4 pos;
		Vec2 t;
		Vec3 l;
	};
	class VertexShader : public BaseVertexShader<VSOutput>
	{
	public:
		using Output = VSOutput;
	public:
		Output operator()( const Vertex& v ) const
		{
			// transform mech vertex position before lighting calc
			const auto worldPos = v.pos * worldView;
			// vertex to light data
			const auto v_to_l = light_pos - worldPos;
			const auto dist = v_to_l.Len();
			const Vec3 dir = v_to_l / dist;
			// calculate attenuation
			const auto attenuation = 1.0f /
				(constant_attenuation + linear_attenuation * dist * quadradic_attenuation * sq( dist ));
			// calculate intensity based on angle of incidence and attenuation
			const auto d = light_diffuse * attenuation * std::max( 0.0f,static_cast<Vec3&>( Vec4( v.n,0.0f ) * worldView ) * dir );
			// add diffuse+ambient, filter by material color, saturate and scale
			const auto l = d + light_ambient;
			return{ v.pos * worldViewProj,v.t,l };
		}
		void SetDiffuseLight( const Vec3& c )
		{
			light_diffuse = c;
		}
		void SetAmbientLight( const Vec3& c )
		{
			light_ambient = c;
		}
		void SetLightPosition( const Vec3& pos_in )
		{
			light_pos = pos_in;
		}
	private:
		Vec4 light_pos = { 0.0f,0.0f,0.5f,1.0f };
		Vec3 light_diffuse = { 1.0f,1.0f,1.0f };
		Vec3 light_ambient = { 0.1f,0.1f,0.1f };
		float linear_attenuation = 2.0f;
		float quadradic_attenuation = 2.619f;
		float constant_attenuation = 0.682f;
	};
	// default gs passes vertices through and outputs triangle
	typedef DefaultGeometryShader<VertexShader::Output> GeometryShader;
	// invoked for each pixel of a triangle
	// takes an input of attributes that are the
	// result of interpolating vertex attributes
	// and outputs a color
	class PixelShader
	{
	public:
		template<class Input>
		Color operator()( const Input& in ) const
		{
			const auto material_color = Vec3( pTex->GetPixel(
				unsigned int( in.t.x * tex_width  + 0.5f ) % tex_width,
				unsigned int( in.t.y * tex_height + 0.5f ) % tex_width
			) ) / 255.0f;
			return Color( material_color.GetHadamard( in.l ).GetSaturated() * 255.0f );
		}
		void BindTexture( const std::wstring& filename )
		{
			pTex = std::make_unique<Surface>( Surface::FromFile( filename ) );
			tex_width = pTex->GetWidth();
			tex_height = pTex->GetHeight();
		}
	private:
		std::unique_ptr<Surface> pTex;
		unsigned int tex_width;
		unsigned int tex_height;
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};
