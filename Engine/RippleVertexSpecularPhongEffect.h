#pragma once

#include "Pipeline.h"
#include "BaseVertexShader.h"
#include "DefaultGeometryShader.h"
#include "BasePhongShader.h"

// flat shading with vertex normals
template<class Diffuse,class Specular>
class RippleVertexSpecularPhongEffect
{
public:
	// the vertex type that will be input into the pipeline
	class Vertex
	{
	public:
		Vertex() = default;
		Vertex( const Vec3& pos )
			:
			pos( pos )
		{}
		Vertex( const Vec3& pos,const Vertex& src )
			:
			t( src.t ),
			pos( pos )
		{}
		Vertex( const Vec3& pos,const Vec2& t )
			:
			t( t ),
			pos( pos )
		{}
	public:
		Vec3 pos;
		Vec2 t;
	};
	// vertex shader
	// output interpolates position, normal, world position
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
			n( src.n ),
			worldPos( src.worldPos ),
			pos( pos ),
			t( src.t ),
			nend( src.nend )
		{}
		VSOutput( const Vec4& pos,const Vec3& n,const Vec3& worldPos,const Vec2& t,const Vec4& nend )
			:
			n( n ),
			pos( pos ),
			worldPos( worldPos ),
			t( t ),
			nend( nend )
		{}
		VSOutput& operator+=( const VSOutput& rhs )
		{
			pos += rhs.pos;
			n += rhs.n;
			worldPos += rhs.worldPos;
			t += rhs.t;
			nend += rhs.nend;
			return *this;
		}
		VSOutput operator+( const VSOutput& rhs ) const
		{
			return VSOutput( *this ) += rhs;
		}
		VSOutput& operator-=( const VSOutput& rhs )
		{
			pos -= rhs.pos;
			n -= rhs.n;
			worldPos -= rhs.worldPos;
			t -= rhs.t;
			nend -= rhs.nend;
			return *this;
		}
		VSOutput operator-( const VSOutput& rhs ) const
		{
			return VSOutput( *this ) -= rhs;
		}
		VSOutput& operator*=( float rhs )
		{
			pos *= rhs;
			n *= rhs;
			worldPos *= rhs;
			t *= rhs;
			nend *= rhs;
			return *this;
		}
		VSOutput operator*( float rhs ) const
		{
			return VSOutput( *this ) *= rhs;
		}
		VSOutput& operator/=( float rhs )
		{
			pos /= rhs;
			n /= rhs;
			worldPos /= rhs;
			t /= rhs;
			nend /= rhs;
			return *this;
		}
		VSOutput operator/( float rhs ) const
		{
			return VSOutput( *this ) /= rhs;
		}
	public:
		Vec4 pos;
		Vec3 n;
		Vec3 worldPos;
		Vec2 t;
		Vec4 nend;
	};
	class VertexShader : public BaseVertexShader<VSOutput>
	{
	public:
		void SetTime( float time )
		{
			t = time;
		}
		typename BaseVertexShader::Output operator()( const Vertex& v ) const
		{
			// calculate some triggy bois
			const auto angle = wrap_angle( v.pos.x * freq + t * wavelength );
			const auto cosx = std::cos( angle );
			const auto sinx = std::sin( angle );
			// sine wave amplitude from position w/ time variant phase animation
			const auto dz = amplitude * cosx;
			const auto pos = Vec4{ v.pos.x,v.pos.y,v.pos.z + dz,1.0f };
			// normal derived base on cross product of partial dx x dy
			auto n = Vec4{
				-freq * amplitude * sinx,
				0.0f,
				-1.0f,
				0.0f
			};
			n.Normalize();
			const auto nend = pos + n * 0.02f;

			return { pos * worldViewProj,n * worldView,pos * worldView,v.t,nend * worldViewProj };
		}
	private:
		static constexpr float wavelength = PI;
		static constexpr float freq = 45.0f;
		static constexpr float amplitude = 0.02f;
		float t = 0.0f;
	};
	// default gs passes vertices through and outputs triangle
	typedef DefaultGeometryShader<typename VertexShader::Output> GeometryShader;
	// invoked for each pixel of a triangle
	// takes an input of attributes that are the
	// result of interpolating vertex attributes
	// and outputs a color
	class PixelShader : public BasePhongShader<Diffuse,Specular>
	{
	public:
		template<class Input>
		Color operator()( const Input& in ) const
		{
			const auto material_color = Vec3( pTex->GetPixel(
				unsigned int( in.t.x * tex_width + 0.5f ) % tex_width,
				unsigned int( in.t.y * tex_height + 0.5f ) % tex_width
			) ) / 255.0f;
			return Shade( in,material_color );
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