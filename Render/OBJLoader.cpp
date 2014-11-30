#include <windows.h>
#include <cassert>
#include "stdio.h"
#include "string.h"
#include "core.h"
#include "OBJLoader.h"
#include "math.h"
#include "float.h"

using namespace std;

OBJLoader::~OBJLoader()
{
    delete[] vertices;
}

char* skip_past_null(char* ptr)
{
    while (*ptr++) ;
    return ptr;
}

struct Position
{
    float x,y,z;
};

struct Normal
{
    float i,j,k;
};

struct TexCoord
{
    float u,v;
};

struct UniqueVertex
{
	Vertex data;
	unsigned int id[3];
    
	bool operator == (const UniqueVertex& rhs)
        {
            if (id[0] == rhs.id[0] &&
                id[1] == rhs.id[1] &&
                id[2] == rhs.id[2])
                return true;
            else
                return false;
            /*
            if ( fabs(data.position[0] - rhs.data.position[0]) < 1e-6 &&
                 fabs(data.position[1] - rhs.data.position[1]) < 1e-6 &&
                 fabs(data.position[2] - rhs.data.position[2]) < 1e-6 &&
                 fabs(data.normal[0]   - rhs.data.normal[0])   < 1e-3 &&
                 fabs(data.normal[1]   - rhs.data.normal[1])   < 1e-3 &&
                 fabs(data.normal[2]   - rhs.data.normal[2])   < 1e-3 &&
                 fabs(data.texcoord[0] - rhs.data.texcoord[0]) < 1e-4 &&
                 fabs(data.texcoord[1] - rhs.data.texcoord[1]) < 1e-4)
                return true;
            else
                return false;
            */
        }
};

static uint AddVertex(Array<UniqueVertex>* vertices, UniqueVertex newv)
{
#if 0
	for (uint i=0; i<vertices->size; i++)
	{
		if (vertices[i] == newv)
			return i;
	}
#endif
    
	vertices->Add(newv);
	return (uint)vertices->size - 1;
}

void OBJLoader::Load(const char* pFilename)
{
    FILE* file = fopen(pFilename, "rt");
    assert(file);
    
	Array<Position, 4096> positions;
	Array<Normal, 4096> normals;
	Array<TexCoord, 4096> texcoords;
    
	Array<UniqueVertex> temp_vertices;
	Array<Triangle> temp_triangles;

	char* mtl_filename;
	char* start;
	char* end;

    char mtl_name[4096];
	Material* material;
    
    uint attribute_indices[12];
    uint num_attribute_indices;
    
    UniqueVertex v[4];
    Triangle t0;
    Triangle t1;
    uint index0, index1, index2, index3;
    
	int i;
	while (!feof(file))
    {
        static char buffer[4096];
        fgets(buffer, 4096, file);
        char* line = buffer;
        
        char* pToken = strtok(line, " \n");
        if (!pToken)
            continue;
        
        switch(*pToken)
        {
        case 'v':
            line = skip_past_null(line);
            if (strcmp(pToken, "v") == 0)
            {
                Position pos;
                sscanf(line, "%f %f %f", &pos.x, &pos.y, &pos.z);
				positions.Add(pos);
            }
            if (strcmp(pToken, "vn") == 0)
            {
                Normal normal;
                sscanf(line, "%f %f %f", &normal.i, &normal.j, &normal.k);
				normals.Add(normal);
            }
            if (strcmp(pToken, "vt") == 0)
            {
				TexCoord tc;
                sscanf(line, "%f %f", &tc.u, &tc.v);
				texcoords.Add(tc);
            }
            break;
        case 'f':
            line = skip_past_null(line);

            num_attribute_indices = 0;

            i = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                   &attribute_indices[ 0], &attribute_indices[ 1], &attribute_indices[ 2],
                   &attribute_indices[ 3], &attribute_indices[ 4], &attribute_indices[ 5],
                   &attribute_indices[ 6], &attribute_indices[ 7], &attribute_indices[ 8],
                   &attribute_indices[ 9], &attribute_indices[10], &attribute_indices[11]);
			if (i == 12)
			{
                num_attribute_indices = 12;
            } else {
                i = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d",
                           &attribute_indices[0], &attribute_indices[1], &attribute_indices[2],
                           &attribute_indices[3], &attribute_indices[4], &attribute_indices[5],
                           &attribute_indices[6], &attribute_indices[7], &attribute_indices[8]);
                if (i == 9) {
                    num_attribute_indices = 9;
                } else {
                    i = sscanf(line, "%d//%d %d//%d %d//%d",
                               &attribute_indices[0], &attribute_indices[2],
                               &attribute_indices[3], &attribute_indices[5],
                               &attribute_indices[6], &attribute_indices[8]);
                    if (i == 6)
                    {
                        attribute_indices[1] = 0;
                        attribute_indices[4] = 0;
                        attribute_indices[7] = 0;
                        num_attribute_indices = 9;
                    } else {
                        i = sscanf(line, "%d %d %d",
                                   &attribute_indices[0], &attribute_indices[3], &attribute_indices[6]);
                        if (i == 3)
                        {
                            attribute_indices[1] = 0;
                            attribute_indices[2] = 0;
                            attribute_indices[4] = 0;
                            attribute_indices[5] = 0;
                            attribute_indices[7] = 0;
                            attribute_indices[8] = 0;
                            num_attribute_indices = 9;
                        }
                    }
                }
            }

            // Otherwise, the above code didn't recognize the f ... layout in the obj file
            assert(num_attribute_indices/3 == 3 || num_attribute_indices/3 == 4);
            
            for (uint i=0; i<num_attribute_indices/3; i++)
            {
                uint iPos = attribute_indices[i*3 + 0] - 1;
                uint iTC = attribute_indices[i*3 + 1] - 1;
                uint iNorm = attribute_indices[i*3 + 2] - 1;
                v[i].data.position[0] = positions[iPos].x;
                v[i].data.position[1] = positions[iPos].y;
                v[i].data.position[2] = positions[iPos].z;
                v[i].data.normal[0] = normals[iNorm].i;
                v[i].data.normal[1] = normals[iNorm].j;
                v[i].data.normal[2] = normals[iNorm].k;
                v[i].data.texcoord[0] = texcoords[iTC].u;
                v[i].data.texcoord[1] = texcoords[iTC].v;
                v[i].id[0] = iPos;
                v[i].id[1] = iNorm;
                v[i].id[2] = iTC;
            }

            index0 = AddVertex(&temp_vertices, v[0]);
            index1 = AddVertex(&temp_vertices, v[1]);
            index2 = AddVertex(&temp_vertices, v[2]);

            t0.index[0] = index0;
            t0.index[1] = index1;
            t0.index[2] = index2;
            temp_triangles.Add(t0);

            if (num_attribute_indices/3 == 4)
            {
                index3 = AddVertex(&temp_vertices, v[3]);
                
                t1.index[0] = index0;
                t1.index[1] = index2;
                t1.index[2] = index3;
                
                temp_triangles.Add(t1);
            }
			break;
            
        case 'g':
            line = skip_past_null(line);
            // flush finished group
            if (temp_triangles.size)
            {
                Subset* subset = new Subset;
				subset->material = material;
                subset->num_triangles = (uint)temp_triangles.size;
                subset->triangles = new Triangle[subset->num_triangles];
                for (uint i=0; i<subset->num_triangles; i++)
                    subset->triangles[i] = temp_triangles[i];
				subsets.Add(subset);
            }
            // start new group
            temp_triangles.Clear();
            line = strchr(line, '\n')+1;
            break;

        case 'm':
            assert(strcmp(line, "mtllib") == 0);
			line += 7;
            start = line;
            end = start;
            while (*end++ != '\n') ;
            mtl_filename = new char[end-start];
            strncpy(mtl_filename, start, end-start-1);
            mtl_filename[end-start-1] = 0;
            LoadMaterialLibrary(pFilename, mtl_filename);
            line = strchr(line, '\n')+1;
            break;

        case 'u':
            assert(strcmp(line, "usemtl") == 0);
            line += 7;
            start = line;
            end = start;
            while (*end++ != '\n') ;
            strncpy(mtl_name, start, end-start-1);
            mtl_name[end-start-1] = 0;
            for (int i=0; i<materials.size; i++)
                if (strcmp(materials[i]->name, mtl_name) == 0)
                    material = materials[i];
            line = strchr(line, '\n')+1;
            
        default:
            break;
		}
	}
    
	num_vertices = (uint)temp_vertices.size;
	vertices = new Vertex[num_vertices];
	for (uint i=0; i<num_vertices; i++)
		vertices[i] = temp_vertices[i].data;

    if (temp_triangles.size)
    {
        Subset* subset = new Subset;
		subset->material = material;
        subset->num_triangles = (uint)temp_triangles.size;
        subset->triangles = new Triangle[subset->num_triangles];
        for (uint i=0; i<subset->num_triangles; i++)
            subset->triangles[i] = temp_triangles[i];
		subsets.Add(subset);
    }
    
	// Scale to 1 meter
	float min =  FLT_MAX;
	float max = -FLT_MAX;
	for (uint i=0; i<num_vertices; i++)
	{
		if (vertices[i].position[0] < min) min = vertices[i].position[0];
		if (vertices[i].position[1] < min) min = vertices[i].position[1];
		if (vertices[i].position[2] < min) min = vertices[i].position[2];
		if (vertices[i].position[0] > max) max = vertices[i].position[0];
		if (vertices[i].position[1] > max) max = vertices[i].position[1];
		if (vertices[i].position[2] > max) max = vertices[i].position[2];
	}
    
	float scale = 750.0f / fabs(max - min);
	for (uint i=0; i<num_vertices; i++)
	{
		vertices[i].position[0] *= scale;
		vertices[i].position[1] *= scale;
		vertices[i].position[2] *= scale;
	}

	dpf("SCALE = %f\n", scale);

	for (uint i=0; i<num_vertices; i++)
	{
		vertices[i].texcoord[1] = 1 - vertices[i].texcoord[1];
	}
    
    fclose(file);
}

struct Symbol
{
    const char* start;
    const char* end;
};

static bool SymbolChar(char point)
{
    if (point >= 'a' && point <= 'z') return true;
    if (point >= 'A' && point <= 'Z') return true;
    if (point >= '0' && point <= '9') return true;
    if (point == '\\') return true;
    if (point == '/') return true;
    if (point == '.') return true;
    if (point == '_') return true;
    return false;
}

static bool EndOfLineChar(char point)
{
	if (point == '#') return true;
	if (point == '\n') return true;
	if (point == '\0') return true;
	return false;
}

static const char* SkipToNewline(const char* point)
{
    while (*point && *point != '\n')
        point++;
    return point;
}

static Symbol GetNextSymbol(const char* point)
{
    Symbol s;

    s.start = point;
    while (*s.start && !SymbolChar(*s.start))
    {
        if (*s.start == '#')
            s.start = SkipToNewline(s.start);
        s.start++;
    }

    if (*s.start == '\0')
    {
        s.start = NULL;
        s.end = NULL;
        return s;
    }
    
    s.end = s.start+1;

    while (*s.end && SymbolChar(*s.end))
    {
        s.end++;
    }
    
    return s;
}

void CopySymbol(char* string, Symbol s)
{
    strncpy(string, s.start, s.end-s.start);
    string[s.end-s.start] = NULL;
}

void OBJLoader::LoadMaterialLibrary(const char* obj_filename, const char* mtl_filename)
{
    // prepend mtl_filename with the path from obj_filename
    const char* last_backslash = NULL;
    const char* point = obj_filename;
    while (*point)
    {
        if (*point == '\\')
            last_backslash = point;
        point++;
    }
    char filename_with_path[4096];
    strncpy(filename_with_path, obj_filename, last_backslash - obj_filename + 1);
	filename_with_path[last_backslash - obj_filename + 1] = NULL;
	strcat(filename_with_path, mtl_filename);

    // Read in the file into a string buffer
    FILE* file = fopen(filename_with_path, "rb");
    assert(file);

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = new char[file_size + 1];
	memset(data, 0, file_size);
    fread(data, 1, file_size, file);
	data[file_size] = NULL;

    fclose(file);

    // Create array of symbols in the file (each Symbol points back to the file's data; this does not insert nulls
    // into the string buffer)
    Array<Symbol> symbols;
    
    Material* material;
        
    Symbol s = GetNextSymbol(data);
    while (s.start != s.end)
    {
        symbols.Add(s);
        s = GetNextSymbol(s.end);
    }

    // Fill out the materials by extracting symbol information
    for (size_t i = 0; i < symbols.size; i++)
    {
        #define MATCH(X) if (strncmp(symbols[i].start, X, strlen(X)) == 0)
        #define ADVANCE(X) i += X; continue;
        
		MATCH("newmtl") {
            material = new Material;
            materials.Add(material);
            CopySymbol(material->name, symbols[i+1]);
            ADVANCE(1);
        }

        MATCH("Ns") {
            sscanf(symbols[i+1].start, "%f", &material->Ns);
            ADVANCE(1);
        }

        MATCH("Ni") {
            sscanf(symbols[i+1].start, "%f", &material->Ni);
            ADVANCE(1);
        }

        MATCH("d") {
            sscanf(symbols[i+1].start, "%f", &material->d);
            ADVANCE(1);
        }

        MATCH("Tr") {
            sscanf(symbols[i+1].start, "%f", &material->Tr);
            ADVANCE(1);
        }

        MATCH("Tf") {
            sscanf(symbols[i+1].start, "%f %f", &material->Tf[0], &material->Tf[1], &material->Tf[2]);
            ADVANCE(1);
        }

        MATCH("illum") {
            sscanf(symbols[i+1].start, "%f", &material->illum);
            ADVANCE(1);
        }

        MATCH("Ka") {
            sscanf(symbols[i+1].start, "%f %f %f", &material->Ka[0], &material->Ka[1], &material->Ka[2]);
            ADVANCE(3);
        }

        MATCH("Kd") {
            sscanf(symbols[i+1].start, "%f %f %f", &material->Kd[0], &material->Kd[1], &material->Kd[2]);
            ADVANCE(3);
        }

        MATCH("Ks") {
            sscanf(symbols[i+1].start, "%f %f %f", &material->Ks[0], &material->Ks[1], &material->Ks[2]);
            ADVANCE(3);
        }

        MATCH("Ke") {
            sscanf(symbols[i+1].start, "%f %f %f", &material->Ke[0], &material->Ke[1], &material->Ke[2]);
            ADVANCE(3);
        }

		MATCH("map_Ka") {
            CopySymbol(material->map_Ka, symbols[i+1]);
            ADVANCE(1);
        }

		MATCH("map_Kd") {
            CopySymbol(material->map_Kd, symbols[i+1]);
            ADVANCE(1);
        }

		MATCH("map_d") {
            CopySymbol(material->map_d, symbols[i+1]);
            ADVANCE(1);
        }

		MATCH("map_bump") {
            CopySymbol(material->bump, symbols[i+1]);
            ADVANCE(1);
        }

		MATCH("bump") {
            CopySymbol(material->bump, symbols[i+1]);
            ADVANCE(1);
        }
    }

    delete data;
}
