#include "Mesh.h"
#include "Globals.h"

#include "tiny_gltf.h"


Mesh::Mesh() 
{
}

Mesh::~Mesh()
{
}

bool Mesh::LoadMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive)
{
	return true;
}

void Mesh::Draw() const
{
    //TODO: Pending
}

void Mesh::Destroy()
{
    //TODO: Pending
}



