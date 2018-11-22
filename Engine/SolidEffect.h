#pragma once

#include "Pipeline.h"
#include "BaseVertexShader.h"
#include "DefaultGeometryShader.h"

// solid color attribute not interpolated
class SolidEffect
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
			color( src.color ),
			pos( pos )
		{}
		Vertex( const Vec3& pos,const Color& color )
			:
			color( color ),
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
		Color color;
	};
	// vertex type solid effect
	// only pos is interpolated
	class VSOutput
	{
	public:
		VSOutput() = default;
		VSOutput( const Vec4& pos )
			:
			pos( pos )
		{}
		VSOutput( const Vec4& pos,const Vertex& src )
			:
			color( src.color ),
			pos( pos )
		{}
		VSOutput( const Vec4& pos,const Color& color )
			:
			color( color ),
			pos( pos )
		{}
		VSOutput& operator+=( const VSOutput& rhs )
		{
			pos += rhs.pos;
			return *this;
		}
		VSOutput operator+( const VSOutput& rhs ) const
		{
			return VSOutput( *this ) += rhs;
		}
		VSOutput& operator-=( const VSOutput& rhs )
		{
			pos -= rhs.pos;
			return *this;
		}
		VSOutput operator-( const VSOutput& rhs ) const
		{
			return VSOutput( *this ) -= rhs;
		}
		VSOutput& operator*=( float rhs )
		{
			pos *= rhs;
			return *this;
		}
		VSOutput operator*( float rhs ) const
		{
			return VSOutput( *this ) *= rhs;
		}
		VSOutput& operator/=( float rhs )
		{
			pos /= rhs;
			return *this;
		}
		VSOutput operator/( float rhs ) const
		{
			return VSOutput( *this ) /= rhs;
		}
	public:
		Vec4 pos;
		Color color;
	};
	class VertexShader : public BaseVertexShader<VSOutput>
	{
	public:
		Output operator()( const Vertex& v ) const
		{
			return{ Vec4( v.pos ) * worldViewProj,v.color };
		}
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
		template<class I>
		Color operator()( const I& in ) const
		{
			return in.color;
		}
	};
public:
	VertexShader vs;
	GeometryShader gs;
	PixelShader ps;
};