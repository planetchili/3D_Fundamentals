#pragma once

#include <vector>
#include "Vec3.h"
#include "tiny_obj_loader.h"
#include <fstream>
#include <cctype>

template<class T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList( std::vector<T> verts_in,std::vector<size_t> indices_in )
		:
		vertices( std::move( verts_in ) ),
		indices( std::move( indices_in ) )
	{
		assert( vertices.size() > 2 );
		assert( indices.size() % 3 == 0 );
	}
	static IndexedTriangleList<T> Load( const std::string& filename )
	{
		IndexedTriangleList<T> tl;

		// check first line of file to see if CCW winding comment exists
		bool isCCW = false;
		{
			std::ifstream file( filename );
			std::string firstline;
			std::getline( file,firstline );
			std::transform( firstline.begin(),firstline.end(),firstline.begin(),std::tolower );
			if( firstline.find( "ccw" ) != std::string::npos )
			{
				isCCW = true;
			}
		}

		// these will be filled by obj loading function
		using namespace tinyobj;
		attrib_t attrib;
		std::vector<shape_t> shapes;
		std::string err;

		// load/parse the obj file
		const bool ret = LoadObj( &attrib,&shapes,nullptr,&err,filename.c_str() );

		// check for errors
		if( !err.empty() && err.substr( 0,4 ) != "WARN" )
		{
			throw std::runtime_error( ("LoadObj returned error:" + err + " File:" + filename).c_str() );
		}
		if( !ret )
		{
			throw std::runtime_error( ("LoadObj returned false  File:" + filename).c_str() );
		}
		if( shapes.size() == 0u )
		{
			throw std::runtime_error( ("LoadObj object file had no shapes  File:" + filename).c_str() );
		}

		// extract vertex data
		tl.vertices.reserve( attrib.vertices.size() / 3u );
		for( int i = 0; i < attrib.vertices.size(); i += 3 )
		{
			tl.vertices.emplace_back( Vec3{
				attrib.vertices[i + 0],
				attrib.vertices[i + 1],
				attrib.vertices[i + 2]
			} );
		}

		// extract index data
		const auto& mesh = shapes[0].mesh;
		tl.indices.reserve( mesh.indices.size() );
		for( size_t f = 0; f < mesh.num_face_vertices.size(); f++ )
		{
			if( mesh.num_face_vertices[f] != 3u )
			{
				std::stringstream ss;
				ss << "LoadObj error face #" << f << " has "
					<< mesh.num_face_vertices[f] << " vertices";
				throw std::runtime_error( ss.str().c_str() );
			}

			for( size_t vn = 0; vn < 3u; vn++ )
			{
				const auto idx = mesh.indices[f * 3u + vn];
				tl.indices.push_back( size_t( idx.vertex_index ) );
			}

			// reverse winding if file marked as CCW
			if( isCCW )
			{
				std::swap( tl.indices.back(),*std::prev( tl.indices.end(),2 ) );
			}
		}

		return tl;
	}
	std::vector<T> vertices;
	std::vector<size_t> indices;
};