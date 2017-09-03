#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <string>

class Scene
{
public:
	Scene( const std::string& name )
		:
		name( name )
	{}
	virtual void Update( Keyboard& kbd,Mouse& mouse,float dt ) = 0;
	virtual void Draw() = 0;
	virtual ~Scene() = default;
	const std::string& GetName() const
	{
		return name;
	}
private:
	std::string name;
};