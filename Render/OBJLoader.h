#pragma once

#include <Array.h>

struct Vertex
{
    float position[3];
    float normal[3];
    float texcoord[2];
};

struct Triangle
{
    uint index[3];
};

struct Material
{
    Material() { memset(this, 0, sizeof(Material)); }
    static const int MaxStringSize = 128;
    char name[MaxStringSize];
    float Ns;
    float Ni;
    float d;
    float Tr;
    float Tf[3];
    float illum;
    float Ka[3];
    float Kd[3];
    float Ks[3];
    float Ke[3];
    char map_Ka[MaxStringSize];
    char map_Kd[MaxStringSize];
    char map_d[MaxStringSize];
    char map_bump[MaxStringSize];
    char bump[MaxStringSize];
};

class OBJLoader
{
public:
	OBJLoader() { num_vertices = 0; }
    ~OBJLoader();

    void Load(const char* filename);
    
    uint num_vertices;
    Vertex* vertices;

	struct Subset
	{
		Subset() { num_triangles = 0; }

        Material* material;
		uint      num_triangles;
		Triangle* triangles;
	};

    Array<Subset*> subsets;
    Array<Material*> materials;

protected:
	void LoadMaterialLibrary(const char*, const char*);
};
