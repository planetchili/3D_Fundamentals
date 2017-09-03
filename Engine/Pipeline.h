#pragma once

#include "ChiliWin.h"
#include "Graphics.h"
#include "Triangle.h"
#include "IndexedTriangleList.h"
#include "PubeScreenTransformer.h"
#include "Mat3.h"
#include <algorithm>

// triangle drawing pipeline with programable
// pixel shading stage
template<class Effect>
class Pipeline
{
public:
	// vertex type used for geometry and throughout pipeline
	typedef typename Effect::Vertex Vertex;
public:
	Pipeline( Graphics& gfx )
		:
		gfx( gfx )
	{}
	void Draw( IndexedTriangleList<Vertex>& triList )
	{
		ProcessVertices( triList.vertices,triList.indices );
	}
	void BindRotation( const Mat3& rotation_in )
	{
		rotation = rotation_in;
	}
	void BindTranslation( const Vec3& translation_in )
	{
		translation = translation_in;
	}
private:
	// vertex processing function
	// transforms vertices and then passes vtx & idx lists to triangle assembler
	void ProcessVertices( const std::vector<Vertex>& vertices,const std::vector<size_t>& indices )
	{
		// create vertex vector for vs output
		std::vector<Vertex> verticesOut;

		// transform vertices using matrix + vector
		for( const auto& v : vertices )
		{
			verticesOut.emplace_back( v.pos * rotation + translation,v );
		}

		// assemble triangles from stream of indices and vertices
		AssembleTriangles( verticesOut,indices );
	}
	// triangle assembly function
	// assembles indexed vertex stream into triangles and passes them to post process
	// culls (does not send) back facing triangles
	void AssembleTriangles( const std::vector<Vertex>& vertices,const std::vector<size_t>& indices )
	{
		// assemble triangles in the stream and process
		for( size_t i = 0,end = indices.size() / 3;
			 i < end; i++ )
		{
			// determine triangle vertices via indexing
			const auto& v0 = vertices[indices[i * 3]];
			const auto& v1 = vertices[indices[i * 3 + 1]];
			const auto& v2 = vertices[indices[i * 3 + 2]];
			// cull backfacing triangles with cross product (%) shenanigans
			if( (v1.pos - v0.pos) % (v2.pos - v0.pos) * v0.pos <= 0.0f )
			{
				// process 3 vertices into a triangle
				ProcessTriangle( v0,v1,v2 );
			}
		}
	}
	// triangle processing function
	// takes 3 vertices to generate triangle
	// sends generated triangle to post-processing
	void ProcessTriangle( const Vertex& v0,const Vertex& v1,const Vertex& v2 )
	{
		// generate triangle from 3 vertices using gs
		// and send to post-processing
		PostProcessTriangleVertices( Triangle<Vertex>{ v0,v1,v2 } );
	}
	// vertex post-processing function
	// perform perspective and viewport transformations
	void PostProcessTriangleVertices( Triangle<Vertex>& triangle )
	{
		// perspective divide and screen transform for all 3 vertices
		pst.Transform( triangle.v0.pos );
		pst.Transform( triangle.v1.pos );
		pst.Transform( triangle.v2.pos );

		// draw the triangle
		DrawTriangle( triangle );
	}
	// === triangle rasterization functions ===
	//   it0, it1, etc. stand for interpolants
	//   (values which are interpolated across a triangle in screen space)
	//
	// entry point for tri rasterization
	// sorts vertices, determines case, splits to flat tris, dispatches to flat tri funcs
	void DrawTriangle( const Triangle<Vertex>& triangle )
	{
		// using pointers so we can swap (for sorting purposes)
		const Vertex* pv0 = &triangle.v0;
		const Vertex* pv1 = &triangle.v1;
		const Vertex* pv2 = &triangle.v2;

		// sorting vertices by y
		if( pv1->pos.y < pv0->pos.y ) std::swap( pv0,pv1 );
		if( pv2->pos.y < pv1->pos.y ) std::swap( pv1,pv2 );
		if( pv1->pos.y < pv0->pos.y ) std::swap( pv0,pv1 );

		if( pv0->pos.y == pv1->pos.y ) // natural flat top
		{
			// sorting top vertices by x
			if( pv1->pos.x < pv0->pos.x ) std::swap( pv0,pv1 );

			DrawFlatTopTriangle( *pv0,*pv1,*pv2 );
		}
		else if( pv1->pos.y == pv2->pos.y ) // natural flat bottom
		{
			// sorting bottom vertices by x
			if( pv2->pos.x < pv1->pos.x ) std::swap( pv1,pv2 );

			DrawFlatBottomTriangle( *pv0,*pv1,*pv2 );
		}
		else // general triangle
		{
			// find splitting vertex interpolant
			const float alphaSplit =
				(pv1->pos.y - pv0->pos.y) /
				(pv2->pos.y - pv0->pos.y);
			const auto vi = interpolate( *pv0,*pv2,alphaSplit );

			if( pv1->pos.x < vi.pos.x ) // major right
			{
				DrawFlatBottomTriangle( *pv0,*pv1,vi );
				DrawFlatTopTriangle( *pv1,vi,*pv2 );
			}
			else // major left
			{
				DrawFlatBottomTriangle( *pv0,vi,*pv1 );
				DrawFlatTopTriangle( vi,*pv1,*pv2 );
			}
		}
	}
	// does flat *TOP* tri-specific calculations and calls DrawFlatTriangle
	void DrawFlatTopTriangle( const Vertex& it0,
							  const Vertex& it1,
							  const Vertex& it2 )
	{
		// calulcate dVertex / dy
		// change in interpolant for every 1 change in y
		const float delta_y = it2.pos.y - it0.pos.y;
		const auto dit0 = (it2 - it0) / delta_y;
		const auto dit1 = (it2 - it1) / delta_y;

		// create right edge interpolant
		auto itEdge1 = it1;

		// call the flat triangle render routine
		DrawFlatTriangle( it0,it1,it2,dit0,dit1,itEdge1 );
	}
	// does flat *BOTTOM* tri-specific calculations and calls DrawFlatTriangle
	void DrawFlatBottomTriangle( const Vertex& it0,
								 const Vertex& it1,
								 const Vertex& it2 )
	{
		// calulcate dVertex / dy
		// change in interpolant for every 1 change in y
		const float delta_y = it2.pos.y - it0.pos.y;
		const auto dit0 = (it1 - it0) / delta_y;
		const auto dit1 = (it2 - it0) / delta_y;

		// create right edge interpolant
		auto itEdge1 = it0;

		// call the flat triangle render routine
		DrawFlatTriangle( it0,it1,it2,dit0,dit1,itEdge1 );
	}
	// does processing common to both flat top and flat bottom tris
	// scan over triangle in screen space, interpolate attributes,
	// invoke ps and write pixel to screen
	void DrawFlatTriangle( const Vertex& it0,
						   const Vertex& it1,
						   const Vertex& it2,
						   const Vertex& dv0,
						   const Vertex& dv1,
						   Vertex itEdge1 )
	{
		// create edge interpolant for left edge (always v0)
		auto itEdge0 = it0;

		// calculate start and end scanlines
		const int yStart = (int)ceil( it0.pos.y - 0.5f );
		const int yEnd = (int)ceil( it2.pos.y - 0.5f ); // the scanline AFTER the last line drawn

		// do interpolant prestep
		itEdge0 += dv0 * (float( yStart ) + 0.5f - it0.pos.y);
		itEdge1 += dv1 * (float( yStart ) + 0.5f - it0.pos.y);

		for( int y = yStart; y < yEnd; y++,itEdge0 += dv0,itEdge1 += dv1 )
		{
			// calculate start and end pixels
			const int xStart = (int)ceil( itEdge0.pos.x - 0.5f );
			const int xEnd = (int)ceil( itEdge1.pos.x - 0.5f ); // the pixel AFTER the last pixel drawn

			// create scanline interpolant startpoint
			// (some waste for interpolating x,y,z, but makes life easier not having
			//  to split them off, and z will be needed in the future anyways...)
			auto iLine = itEdge0;

			// calculate delta scanline interpolant / dx
			const float dx = itEdge1.pos.x - itEdge0.pos.x;
			const auto diLine = (itEdge1 - iLine) / dx;

			// prestep scanline interpolant
			iLine += diLine * (float( xStart ) + 0.5f - itEdge0.pos.x);

			for( int x = xStart; x < xEnd; x++,iLine += diLine )
			{
				// invoke pixel shader and write resulting color value
				gfx.PutPixel( x,y,effect.ps( iLine ) );
			}
		}
	}
public:
	Effect effect;
private:
	Graphics& gfx;
	PubeScreenTransformer pst;
	Mat3 rotation;
	Vec3 translation;
};