#pragma once

#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"

// flat shading with vertex normals
class PhongPointEffect
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
				n( src.n ),
				worldPos( src.worldPos ),
				pos( pos )
			{}
			Output( const Vec3& pos,const Vec3& n,const Vec3& worldPos )
				:
				n( n ),
				pos( pos ),
				worldPos( worldPos )
			{}
			Output& operator+=( const Output& rhs )
			{
				pos += rhs.pos;
				n += rhs.n;
				worldPos += rhs.worldPos;
				return *this;
			}
			Output operator+( const Output& rhs ) const
			{
				return Output( *this ) += rhs;
			}
			Output& operator-=( const Output& rhs )
			{
				pos -= rhs.pos;
				n -= rhs.n;
				worldPos -= rhs.worldPos;
				return *this;
			}
			Output operator-( const Output& rhs ) const
			{
				return Output( *this ) -= rhs;
			}
			Output& operator*=( float rhs )
			{
				pos *= rhs;
				n *= rhs;
				worldPos *= rhs;
				return *this;
			}
			Output operator*( float rhs ) const
			{
				return Output( *this ) *= rhs;
			}
			Output& operator/=( float rhs )
			{
				pos /= rhs;
				n /= rhs;
				worldPos /= rhs;
				return *this;
			}
			Output operator/( float rhs ) const
			{
				return Output( *this ) /= rhs;
			}
		public:
			Vec3 pos;
			Vec3 n;
			Vec3 worldPos;
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
			const auto pos = v.pos * rotation + translation;
			return{ pos,v.n * rotation,pos };
		}
	private:
		Mat3 rotation;
		Vec3 translation;
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
			// vertex to light data
			const auto v_to_l = light_pos - in.worldPos;
			const auto dist = v_to_l.Len();
			const auto dir = v_to_l / dist;
			// calculate attenuation
			const auto attenuation = 1.0f /
				(constant_attenuation + linear_attenuation * dist + quadradic_attenuation * sq( dist ));
			// calculate intensity based on angle of incidence and attenuation
			const auto d = light_diffuse * attenuation * std::max( 0.0f,in.n.GetNormalized() * dir );
			// add diffuse+ambient, filter by material color, saturate and scale
			return Color( material_color.GetHadamard( d + light_ambient ).Saturate() * 255.0f );
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
		Vec3 light_pos = { 0.0f,0.0f,0.5f };
		Vec3 light_diffuse = { 1.0f,1.0f,1.0f };
		Vec3 light_ambient = { 0.1f,0.1f,0.1f };
		Vec3 material_color = { 0.8f,0.85f,1.0f };
		float linear_attenuation = 1.0f;
		float quadradic_attenuation = 2.619f;
		float constant_attenuation = 0.382f;
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};