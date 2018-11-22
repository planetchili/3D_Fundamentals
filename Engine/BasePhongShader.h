#pragma once
#include "Colors.h"
#include "Vec4.h"

struct DefaultPointDiffuseParams
{
	static constexpr float linear_attenuation = 1.0f;
	static constexpr float quadradic_attenuation = 2.619f;
	static constexpr float constant_attenuation = 0.382f;
};

struct DefaultSpecularParams
{
	static constexpr float specular_power = 30.0f;
	static constexpr float specular_intensity = 0.6f;
};

template<class PointDiffuse = DefaultPointDiffuseParams,class Specular = DefaultSpecularParams>
class BasePhongShader
{
public:
	template<class Input>
	Color Shade( const Input& in,const Vec4& material_color ) const
	{
		// re-normalize interpolated surface normal
		const auto surf_norm = in.n.GetNormalized();
		// vertex to light data
		const auto v_to_l = light_pos - in.worldPos;
		const auto dist = v_to_l.Len();
		const auto dir = v_to_l / dist;
		// calculate attenuation
		const auto attenuation = 1.0f /
			(PointDiffuse::constant_attenuation + PointDiffuse::linear_attenuation * dist + PointDiffuse::quadradic_attenuation * sq( dist ));
		// calculate intensity based on angle of incidence and attenuation
		const auto d = light_diffuse * attenuation * std::max( 0.0f,surf_norm * dir );
		// reflected light vector
		const auto w = surf_norm * (v_to_l * surf_norm);
		const auto r = w * 2.0f - v_to_l;
		// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
		const auto s = light_diffuse * Specular::specular_intensity * std::pow( std::max( 0.0f,-r.GetNormalized() * in.worldPos.GetNormalized() ),Specular::specular_power );
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
};