#pragma once

struct Texture;

struct Mesh
{
	char* name;

	// A mesh is made of one or more subsets. Each subset has an index buffer that references the mesh's single vertex buffer.
	struct Subset
    {
		Texture*      diffuse_texture;
		ID3D11Buffer* index_buffer;
		uint          num_triangles;
	};
	Array<Subset*>    subsets;

	ID3D11Buffer*     vertex_buffer;
};

struct MeshManager
{
	static void Startup(ID3D11Device*);
	static void Shutdown();

	static Mesh* LoadOBJ(ID3D11Device* device, const char* filename);

    static ID3D11InputLayout* input_layout;
	static Array<Mesh*> meshes;
};
