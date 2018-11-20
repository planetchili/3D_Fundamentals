#pragma once
#include "Vec2.h"

class MouseTracker
{
public:
	void Engage( const Vei2& pos )
	{
		base = pos;
		engaged = true;
	}
	void Release()
	{
		engaged = false;
	}
	Vei2 Move( const Vei2& pos )
	{
		const auto delta = pos - base;
		base = pos;
		return delta;
	}
	bool Engaged() const
	{
		return engaged;
	}
private:
	bool engaged = false;
	Vei2 base;
};