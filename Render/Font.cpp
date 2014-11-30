#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "Font.h"

/*
 * This code loads the output from the free program at http://www.angelcode.com/PRODUCTS/BMFONT/ 
 * Much of the data is skipped for our purposes (we don't care about kerning, for instance).
 * After loading, you can index into the Font::glyphs array by the ASCII character you want to render
 * so that glyphs['A'] will get the data for the character A.
 */

enum BlockType
{
    Info = 1,
    Common = 2,
    Pages = 3,
    Chars = 4,
    KerningPairs = 5,
    NumBlockTypes
};

#pragma pack(1)
struct BlockHeader
{
    uint8  id;    // BlockType (it is one byte or we would use the BlockType enum directly)
    uint32 size;  // Size of the block (not including this header)
};

#pragma pack(1)
struct BlockCommon
{
 	uint16 lineHeight;
 	uint16 base;
 	uint16 scaleW;
 	uint16 scaleH;
 	uint16 pages;
 	uint8 bitField;
 	uint8 alphaChnl;
 	uint8 redChnl;
 	uint8 greenChnl;
 	uint8 blueChnl;
};

#pragma pack(1)
struct BlockChars
{
    uint32 id;
    uint16 x;
    uint16 y;
    uint16 width;
    uint16 height;
    int16  xoffset;
    int16  yoffset;
    int16  xadvance;
    uint8  page;
    uint8  chnl;
};

static void InitGlyphs(Font::Glyph* glyphs)
{
    for (int i=0; i<256; i++)
    {
        glyphs[i].u0 = 0;
        glyphs[i].v0 = 0;
        glyphs[i].u1 = 0;
        glyphs[i].v1 = 0;
        glyphs[i].xoffset = 0;
        glyphs[i].yoffset = 0;
        glyphs[i].xadvance = 0;
    }
}

static Font::Glyph ParseBlockChar(BlockChars* block, float texture_width, float texture_height)
{
    Font::Glyph glyph;
    glyph.u0 = block->x / (float)texture_width;
    glyph.v0 = block->y / (float)texture_height;
    glyph.u1 = (block->x + block->width)  / (float)texture_width;
    glyph.v1 = (block->y + block->height) / (float)texture_height;
    glyph.xoffset = block->xoffset;
    glyph.yoffset = block->yoffset;
    glyph.width = block->width;
    glyph.height = block->height;
    glyph.xadvance = block->xadvance;
    return glyph;                            
}

Font::Font(ID3D11Device* device, const char* texture_filename, const char* font_filename)
{
    InitGlyphs(glyphs);

    HRESULT hr;
    hr = D3DX11CreateShaderResourceViewFromFile(device, texture_filename, NULL, NULL, &texture, NULL);
	assert(SUCCEEDED(hr));

    FILE* file = fopen(font_filename, "rb");
    fseek(file, 0, SEEK_END);
    int fontdata_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8* font_data = (uint8*) new uint8[fontdata_size];
    fread(font_data, fontdata_size, 1, file);
    assert(font_data[0] == 'B');
    assert(font_data[1] == 'M');
    assert(font_data[2] == 'F');
    assert(font_data[3] == 3);

    uint8* current = font_data + 4;
    while (current != font_data + fontdata_size)
    {
        BlockHeader* header = (BlockHeader*) current;
        current += sizeof(BlockHeader);

        switch(header->id)
        {
        case Info:
            break;

        case Common:
            {
                BlockCommon* block = (BlockCommon*)current;
                line_height = block->lineHeight;
                texture_width = block->scaleW;
                texture_height = block->scaleH;
            }
            break;
            
        case Pages:
            break;

        case Chars:
            {
                int num_chars = header->size / sizeof(BlockChars);
                for (int i=0; i<num_chars; i++)
                {
                    BlockChars* block = (BlockChars*)current + i;
                    if (block->id >= 0 && block->id < 256)
                    {
                        glyphs[block->id] = ParseBlockChar(block, (float)texture_width, (float)texture_height);
                    }
                }
            }
            break; 
            
        case KerningPairs:
            break;
        }

        current += header->size;
    }

    delete font_data;
    fclose(file);
}

Font::~Font()
{
    SAFE_RELEASE(texture);
}
