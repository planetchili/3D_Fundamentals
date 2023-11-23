#pragma once

#include "Pipeline.h"
#include "BaseVertexShader.h"
#include "DefaultGeometryShader.h"
#include "BasePhongShader.h"


// flat shading with vertex normals
template<class Diffuse>
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
			const auto worldPos = v.pos * this->worldView;
			// vertex to light data
			const auto v_to_l = light_pos - worldPos;
			const auto dist = v_to_l.Len();
			const Vec3 dir = v_to_l / dist;
			// calculate attenuation
			const auto attenuation = 1.0f /
				(Diffuse::constant_attenuation + Diffuse::linear_attenuation * dist * Diffuse::quadradic_attenuation * sq( dist ));
			// calculate intensity based on angle of incidence and attenuation
			const auto d = light_diffuse * attenuation * std::max( 0.0f,static_cast<Vec3>( Vec4( v.n,0.0f ) * this->worldView ) * dir );
			// add diffuse+ambient, filter by material color, saturate and scale
			const auto l = d + light_ambient;
			return{ v.pos * this->worldViewProj,v.t,l };
		}
		void SetDiffuseLight( const Vec3& c )
		{
			light_diffuse = c;
		}
		void SetAmbientLight( const Vec3& c )
		{
			light_ambient = c;
		}
		void SetLightPosition( const Vec4& pos_in )
		{
			light_pos = pos_in;
		}
	private:
		Vec3 light_diffuse;
		Vec3 light_ambient;
		Vec4 light_pos;
	};
	// default gs passes vertices through and outputs triangle
	typedef DefaultGeometryShader<typename VertexShader::Output> GeometryShader;
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
				static_cast<unsigned int>( in.t.x * tex_width  + 0.5f ) % tex_width,
				static_cast<unsigned int>( in.t.y * tex_height + 0.5f ) % tex_width
			) ) / 255.0f;
			return Color( material_color.GetHadamard( in.l ).GetSaturated() * 255.0f );
		}
		void BindTexture( const Surface& tex )
		{
			pTex = &tex;
			tex_width = pTex->GetWidth();
			tex_height = pTex->GetHeight();
		}
	private:
		const Surface* pTex = nullptr;
		unsigned int tex_width;
		unsigned int tex_height;
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};
