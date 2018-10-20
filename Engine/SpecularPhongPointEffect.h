#pragma once

#include "Pipeline.h"
#include "DefaultGeometryShader.h"

// flat shading with vertex normals
class SpecularPhongPointEffect
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
			Output( const Vec4& pos )
				:
				pos( pos )
			{}
			Output( const Vec4& pos,const Output& src )
				:
				n( src.n ),
				worldPos( src.worldPos ),
				pos( pos )
			{}
			Output( const Vec4& pos,const Vec3& n,const Vec3& worldPos )
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
			Vec4 pos;
			Vec3 n;
			Vec3 worldPos;
		};
	public:
		void BindWorld( const Mat4& transformation_in )
		{
			world = transformation_in;
			worldProj = world * proj;
		}
		void BindProjection( const Mat4& transformation_in )
		{
			proj = transformation_in;
			worldProj = world * proj;
		}
		const Mat4& GetProj() const
		{
			return proj;
		}
		Output operator()( const Vertex& v ) const
		{
			const auto p4 = Vec4( v.pos );
			return { p4 * worldProj,Vec4{ v.n,0.0f } * world,p4 * world };
		}
	private:
		Mat4 world = Mat4::Identity();
		Mat4 proj = Mat4::Identity();
		Mat4 worldProj = Mat4::Identity();
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
			// re-normalize interpolated surface normal
			const auto surf_norm = in.n.GetNormalized();
			// vertex to light data
			const auto v_to_l = light_pos - in.worldPos;
			const auto dist = v_to_l.Len();
			const auto dir = v_to_l / dist;
			// calculate attenuation
			const auto attenuation = 1.0f /
				(constant_attenuation + linear_attenuation * dist + quadradic_attenuation * sq( dist ));
			// calculate intensity based on angle of incidence and attenuation
			const auto d = light_diffuse * attenuation * std::max( 0.0f,surf_norm * dir );
			// reflected light vector
			const auto w = surf_norm * (v_to_l * surf_norm);
			const auto r = w * 2.0f - v_to_l;
			// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
			const auto s = light_diffuse * specular_intensity * std::pow( std::max( 0.0f,-r.GetNormalized() * in.worldPos.GetNormalized() ),specular_power );
			// add diffuse+ambient, filter by material color, saturate and scale
			return Color( material_color.GetHadamard( d + light_ambient + s ).Saturate() * 255.0f );
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
		// diffuse
		float linear_attenuation = 1.0f;
		float quadradic_attenuation = 2.619f;
		float constant_attenuation = 0.382f;
		// specular
		float specular_power = 30.0f;
		float specular_intensity = 0.6f;
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};