#pragma once

template<class Vertex>
class BaseVertexShader
{
public:
	typedef Vertex Output;
public:
	void BindWorldView( const Mat4& transformation_in )
	{
		worldView = transformation_in;
		worldViewProj = worldView * proj;
	}
	void BindProjection( const Mat4& transformation_in )
	{
		proj = transformation_in;
		worldViewProj = worldView * proj;
	}
	const Mat4& GetProj() const
	{
		return proj;
	}
protected:
	Mat4 proj = Mat4::Identity();
	Mat4 worldView = Mat4::Identity();
	Mat4 worldViewProj = Mat4::Identity();
};