#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"

class WaveVertexTextureEffect
{
public:
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
		Vertex& operator+=( const Vertex& rhs )
		{
			pos += rhs.pos;
			t += rhs.t;
			return *this;
		}
		Vertex operator+( const Vertex& rhs ) const
		{
			return Vertex( *this ) += rhs;
		}
		Vertex& operator-=( const Vertex& rhs )
		{
			pos -= rhs.pos;
			t -= rhs.t;
			return *this;
		}
		Vertex operator-( const Vertex& rhs ) const
		{
			return Vertex( *this ) -= rhs;
		}
		Vertex& operator*=( float rhs )
		{
			pos *= rhs;
			t *= rhs;
			return *this;
		}
		Vertex operator*( float rhs ) const
		{
			return Vertex( *this ) *= rhs;
		}
		Vertex& operator/=( float rhs )
		{
			pos /= rhs;
			t /= rhs;
			return *this;
		}
		Vertex operator/( float rhs ) const
		{
			return Vertex( *this ) /= rhs;
		}
	public:
		Vec3 pos;
		Vec2 t;
	};
	// perturbes vertices in y axis in sin wave based on
	// x position and time
	class VertexShader
	{
	public:
		typedef Vertex Output;
	public:
		void BindRotation( const Mat3& rotation_in )
		{
			rotation = rotation_in;
		}
		void BindTranslation( const Vec3& translation_in )
		{
			translation = translation_in;
		}
		Output operator()( const Vertex& in ) const
		{
			Vec3 pos = in.pos * rotation + translation;
			pos.y += amplitude * std::sin( time * freqScroll + pos.x * freqWave );
			return{ pos,in.t };
		}
		void SetTime( float t )
		{
			time = t;
		}
	private:
		Mat3 rotation;
		Vec3 translation;
		float time = 0.0f;
		float freqWave = 10.0f;
		float freqScroll = 5.0f;
		float amplitude = 0.05f;
	};
	// calculate lighting intensity based on light direction
	// and a face normal computed from geometry with cross product
	class GeometryShader
	{
	public:
		class Output
		{
		public:
			Output() = default;
			Output( const Vec3& pos )
				:
				pos( pos )
			{}
			Output( const Vec3& pos,const Output& src )
				:
				t( src.t ),
				l( src.l ),
				pos( pos )
			{}
			Output( const Vec3& pos,const Vec2& t,float l )
				:
				t( t ),
				l( l ),
				pos( pos )
			{}
			Output& operator+=( const Output& rhs )
			{
				pos += rhs.pos;
				t += rhs.t;
				return *this;
			}
			Output operator+( const Output& rhs ) const
			{
				return Output( *this ) += rhs;
			}
			Output& operator-=( const Output& rhs )
			{
				pos -= rhs.pos;
				t -= rhs.t;
				return *this;
			}
			Output operator-( const Output& rhs ) const
			{
				return Output( *this ) -= rhs;
			}
			Output& operator*=( float rhs )
			{
				pos *= rhs;
				t *= rhs;
				return *this;
			}
			Output operator*( float rhs ) const
			{
				return Output( *this ) *= rhs;
			}
			Output& operator/=( float rhs )
			{
				pos /= rhs;
				t /= rhs;
				return *this;
			}
			Output operator/( float rhs ) const
			{
				return Output( *this ) /= rhs;
			}
		public:
			Vec3 pos;
			Vec2 t;
			float l;
		};
	public:
		Triangle<Output> operator()( const VertexShader::Output& in0,const VertexShader::Output& in1,const VertexShader::Output& in2,size_t triangle_index ) const
		{
			// calculate face normal
			const auto n = ((in1.pos - in0.pos) % (in2.pos - in0.pos)).GetNormalized();
			// calculate intensity based on angle of incidence plus ambient and saturate
			const auto l = std::min( 1.0f,diffuse * std::max( 0.0f,-n * dir ) + ambient );
			return{ { in0.pos,in0.t,l },{ in1.pos,in1.t,l },{ in2.pos,in2.t,l } };
		}
		void SetDiffuseLight( float d )
		{
			diffuse = d;
		}
		void SetAmbientLight( float a )
		{
			ambient = a;
		}
		void SetLightDirection( const Vec3& dl )
		{
			assert( dl.LenSq() >= 0.001f );
			dir = dl.GetNormalized();
		}
	private:
		Mat3 rotation;
		Vec3 translation;
		// direction of travel of light rays
		Vec3 dir = { 0.0f,0.0f,1.0f };
		// this is the intensity if direct light from source
		// white light so only need 1 channel to represent it
		float diffuse = 1.0f;
		// this is intensity of indirect light that bounces off other obj in scene
		// white light so only need 1 channel to represent it
		float ambient = 0.15f;
	};
	// texture clamped ps with light intensity input
	class PixelShader
	{
	public:
		template<class Input>
		Color operator()( const Input& in ) const
		{
			// lookup color in texture
			const Vec3 color = Vec3( pTex->GetPixel(
				(unsigned int)std::min( in.t.x * tex_width + 0.5f,tex_xclamp ),
				(unsigned int)std::min( in.t.y * tex_height + 0.5f,tex_yclamp )
			) );
			// use texture color as material to determine ratio / magnitude
			// of the different color components diffuse reflected from triangle at this pt.
			return Color( color * in.l );
		}
		void BindTexture( const std::wstring& filename )
		{
			pTex = std::make_unique<Surface>( Surface::FromFile( filename ) );
			tex_width = float( pTex->GetWidth() );
			tex_height = float( pTex->GetHeight() );
			tex_xclamp = tex_width - 1.0f;
			tex_yclamp = tex_height - 1.0f;
		}
	private:
		std::unique_ptr<Surface> pTex;
		float tex_width;
		float tex_height;
		float tex_xclamp;
		float tex_yclamp;
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};