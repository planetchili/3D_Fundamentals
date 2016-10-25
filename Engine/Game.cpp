/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Game.cpp																			  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"

Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
}

void Game::Go()
{
	gfx.BeginFrame();
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
}
#include "PubeScreenTransformer.h"
void Game::ComposeFrame()
{
	PubeScreenTransformer pst;
	Vec3 v0 = { 0.0f,0.5f,0.0f };
	Vec3 v1 = { 0.5f,-0.5f,0.0f };
	Vec3 v2 = { -0.5f,-0.5f,0.0f };
	gfx.DrawLine( pst.GetTransformed( v0 ),pst.GetTransformed( v1 ),Colors::White );
	gfx.DrawLine( pst.GetTransformed( v1 ),pst.GetTransformed( v2 ),Colors::White );
	gfx.DrawLine( pst.GetTransformed( v2 ),pst.GetTransformed( v0 ),Colors::White );
}