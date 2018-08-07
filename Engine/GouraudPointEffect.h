#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"

// flat shading with vertex normals
class GouraudPointEffect
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
			n( src.n ),
			pos( pos )
		{}
		Vertex( const Vec3& pos,const Vec3& n )
			:
			n( n ),
			pos( pos )
		{}
		Vertex& operator+=( const Vertex& rhs )
		{
			pos += rhs.pos;
			return *this;
		}
		Vertex operator+( const Vertex& rhs ) const
		{
			return Vertex( *this ) += rhs;
		}
		Vertex& operator-=( const Vertex& rhs )
		{
			pos -= rhs.pos;
			return *this;
		}
		Vertex operator-( const Vertex& rhs ) const
		{
			return Vertex( *this ) -= rhs;
		}
		Vertex& operator*=( float rhs )
		{
			pos *= rhs;
			return *this;
		}
		Vertex operator*( float rhs ) const
		{
			return Vertex( *this ) *= rhs;
		}
		Vertex& operator/=( float rhs )
		{
			pos /= rhs;
			return *this;
		}
		Vertex operator/( float rhs ) const
		{
			return Vertex( *this ) /= rhs;
		}
	public:
		Vec3 pos;
		Vec3 n;
	};
	// calculate color based on normal to light angle
	// no interpolation of color attribute
	class VertexShader
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
				color( src.color ),
				pos( pos )
			{}
			Output( const Vec3& pos,const Vec3& color )
				:
				color( color ),
				pos( pos )
			{}
			Output& operator+=( const Output& rhs )
			{
				pos += rhs.pos;
				color += rhs.color;
				return *this;
			}
			Output operator+( const Output& rhs ) const
			{
				return Output( *this ) += rhs;
			}
			Output& operator-=( const Output& rhs )
			{
				pos -= rhs.pos;
				color -= rhs.color;
				return *this;
			}
			Output operator-( const Output& rhs ) const
			{
				return Output( *this ) -= rhs;
			}
			Output& operator*=( float rhs )
			{
				pos *= rhs;
				color *= rhs;
				return *this;
			}
			Output operator*( float rhs ) const
			{
				return Output( *this ) *= rhs;
			}
			Output& operator/=( float rhs )
			{
				pos /= rhs;
				color /= rhs;
				return *this;
			}
			Output operator/( float rhs ) const
			{
				return Output( *this ) /= rhs;
			}
		public:
			Vec3 pos;
			Vec3 color;
		};
	public:
		void BindRotation( const Mat3& rotation_in )
		{
			rotation = rotation_in;
		}
		void BindTranslation( const Vec3& translation_in )
		{
			translation = translation_in;
		}
		Output operator()( const Vertex& v ) const
		{
			// transform mech vertex position before lighting calc
			const auto pos = v.pos * rotation + translation;
			// vertex to light data
			const auto v_to_l = light_pos - pos;
			const auto dist = v_to_l.Len();
			const auto dir = v_to_l / dist;
			// calculate attenuation
			const auto attenuation = 1.0f / 
				(constant_attenuation + linear_attenuation * dist * quadradic_attenuation * sq( dist ));
			// calculate intensity based on angle of incidence and attenuation
			const auto d = light_diffuse * attenuation * std::max( 0.0f,(v.n * rotation) * dir );
			// add diffuse+ambient, filter by material color, saturate and scale
			const auto c = material_color.GetHadamard( d + light_ambient ).Saturate() * 255.0f;
			return{ pos,c };
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
		Mat3 rotation;
		Vec3 translation;
		Vec3 light_pos = { 0.0f,0.0f,0.5f };
		Vec3 light_diffuse = { 1.0f,1.0f,1.0f };
		Vec3 light_ambient = { 0.1f,0.1f,0.1f };
		Vec3 material_color = { 0.8f,0.85f,1.0f };
		float linear_attenuation = 1.0f;
		float quadradic_attenuation = 2.619f;
		float constant_attenuation = 0.382f;
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
			return Color( in.color );
		}
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};