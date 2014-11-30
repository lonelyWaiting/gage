#pragma once

/*
 * Loads font data from a font texture and a font data file. The data is output
 * from the free program at http://www.angelcode.com/PRODUCTS/BMFONT/ 
 */

class Font
{
public:
    Font(ID3D11Device* device, const char* texture_filename, const char* font_filename);
    ~Font();

    ID3D11ShaderResourceView* texture;

    int texture_width, texture_height;
    int line_height;

    struct Glyph 
    {
        float u0;
        float v0;
        float u1;
        float v1;
        float xoffset;
        float yoffset;
        float width;
        float height;
        float xadvance;
    };
    Glyph glyphs[256];
};
