#pragma once
#include "Vertex.h"
#include <vector>
#include <DirectXMath.h>

class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList( Vertex::DataBuffer verts_in,std::vector<unsigned int> indices_in )
		:
		vertices( std::move( verts_in ) ),
		indices( std::move( indices_in ) )
	{
		assert( vertices.Size() > 2 );
		assert( indices.size() % 3 == 0 );
	}
	void Transform( DirectX::FXMMATRIX matrix )
	{
		for( int i = 0; i < vertices.Size(); i++ )
		{
			auto& pos = vertices[i].Attri<Vertex::Position3D>();
			DirectX::XMStoreFloat3(
				&pos,
				DirectX::XMVector3Transform( DirectX::XMLoadFloat3( &pos ),matrix )
			);
		}
	}
	void SetNormalsIndependentFlat() noexcept(!IS_DEBUG)
	{
		for( size_t i = 0; i < indices.size(); i += 3 )
		{
			using namespace DirectX;
			auto v0 = vertices[indices[i]];
			auto v1 = vertices[indices[i + 1]];
			auto v2 = vertices[indices[i + 2]];
			const auto p0 = DirectX::XMLoadFloat3( &v0.Attri<Vertex::Position3D>() );
			const auto p1 = DirectX::XMLoadFloat3( &v1.Attri<Vertex::Position3D>() );
			const auto p2 = DirectX::XMLoadFloat3( &v2.Attri<Vertex::Position3D>() );

			const auto n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross( (p1 - p0),(p2 - p0) ) );
			
			DirectX::XMStoreFloat3( &v0.Attri<Vertex::Normal>(),n );
			DirectX::XMStoreFloat3( &v1.Attri<Vertex::Normal>(),n );
			DirectX::XMStoreFloat3( &v2.Attri<Vertex::Normal>(),n );
		}
	}

public:
	Vertex::DataBuffer vertices;
	std::vector<unsigned int> indices;
};