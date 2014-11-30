#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <cassert>
#include "stdio.h"
#include "Core.h"
#include "Font.h"
#include "Shader.h"
#include "Text.h"

static Font* font;

static ID3D11Buffer*       vertex_buffer;
static ID3D11Buffer*       index_buffer;
static ID3D11InputLayout*  input_layout;
static ID3D11SamplerState* texture_sampler;

static const int MaxVertexBufferSize = 65536;
static const int MaxIndexBufferSize = 65536;

struct Vertex
{
    float x, y, u, v;
};

struct TextBuffer
{
    TextBuffer() { Reset(); }
    void Reset() { current = stringbuffer; num_entries = 0; }


    static const int MaxStringBufferSize = 65536;
    char  stringbuffer[MaxStringBufferSize];
    char* current;

    struct Entry
    {
        float x;
        float y;
        int   length;
        char* string;
    };
    static const int MaxEntries = 128;
    Entry entries[MaxEntries];
    int num_entries;
};

static TextBuffer text_buffer;

void Text::Startup(ID3D11Device* pDevice)
{
    font = new Font(pDevice, "Assets\\font_0.dds", "Assets\\font.fnt");

    // Create and initialize vertex buffer
    HRESULT hr;
    D3D11_BUFFER_DESC buf_desc;
    ZeroMemory(&buf_desc, sizeof(buf_desc));
	buf_desc.ByteWidth = MaxVertexBufferSize;
    buf_desc.Usage = D3D11_USAGE_DYNAMIC;
    buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    hr = pDevice->CreateBuffer(&buf_desc, NULL, &vertex_buffer);
    assert(SUCCEEDED(hr));

    // Create and initialize index buffer
    ZeroMemory(&buf_desc, sizeof(buf_desc));
	buf_desc.ByteWidth = MaxIndexBufferSize;
    buf_desc.Usage = D3D11_USAGE_DYNAMIC;
    buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    buf_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    hr = pDevice->CreateBuffer(&buf_desc, NULL, &index_buffer);
    assert(SUCCEEDED(hr));

    // Create shaders
    D3D11_INPUT_ELEMENT_DESC input_desc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
	hr = pDevice->CreateInputLayout(input_desc, _countof(input_desc), Shader::GetVertexShaderBlob(ShaderText), Shader::GetVertexShaderBlobSize(ShaderText), &input_layout);
    assert(SUCCEEDED(hr));
    
    // Create the sampler state
    D3D11_SAMPLER_DESC sampler_desc;
    ZeroMemory( &sampler_desc, sizeof(sampler_desc) );
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = 0;
    hr = pDevice->CreateSamplerState(&sampler_desc, &texture_sampler);
	assert(SUCCEEDED(hr));
}
 
void Text::Shutdown()
{
    delete font;
    SAFE_RELEASE(vertex_buffer);
    SAFE_RELEASE(index_buffer);
    SAFE_RELEASE(input_layout);
    SAFE_RELEASE(texture_sampler);
}

float ToViewportX(float x)
{
    float scale =  SIZEX / 2.0f;
    float bias  =  SIZEX / 2.0f;
    return (x - bias) / scale;
}

float ToViewportY(float y)
{
    float scale = -SIZEY / 2.0f;
    float bias  =  SIZEY / 2.0f;
    return (y - bias) / scale;
}

void Text::Print(float x, float y, const char* string, ...)
{
    assert(text_buffer.num_entries < TextBuffer::MaxEntries);

    va_list args;
    va_start(args, string);
    static char buffer[4096];
    vsprintf_s(buffer, 4096, string, args);
    va_end(args);

    assert(text_buffer.current + strlen(buffer) < text_buffer.stringbuffer + TextBuffer::MaxStringBufferSize);

    TextBuffer::Entry* pEntry = &text_buffer.entries[text_buffer.num_entries++];
    pEntry->x = x;
    pEntry->y = y;
    pEntry->length = (int) strlen(buffer);
    pEntry->string = text_buffer.current;
    memcpy(text_buffer.current, buffer, pEntry->length);
    text_buffer.current += pEntry->length;
}

void Text::Draw(ID3D11DeviceContext* pCtxt)
{
    if (text_buffer.num_entries == 0)
        return;

    D3D11_MAPPED_SUBRESOURCE vb, ib;
    pCtxt->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vb);
    pCtxt->Map(index_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ib);

    Vertex* dst_vertex = (Vertex*) vb.pData;
    uint32* dst_index = (uint32*) ib.pData;

    uint vertexCount = 0;
    for (int i=0; i<text_buffer.num_entries; i++)
    {
        if ((vertexCount + text_buffer.entries[i].length) * sizeof(Vertex) > MaxVertexBufferSize)
            break;

        float x = text_buffer.entries[i].x;
        float y = text_buffer.entries[i].y;
        const char* string = text_buffer.entries[i].string;

        for (int j=0; j<text_buffer.entries[i].length; j++)
        {
            const Font::Glyph* glyph = &font->glyphs[string[j]];
            
            Vertex ul, ur, ll, lr;
            
            ul.x = ToViewportX(x + glyph->xoffset);
            ul.y = ToViewportY(y + glyph->yoffset);
            ur.x = ToViewportX(x + glyph->xoffset + glyph->width);
            ur.y = ul.y;
            ll.x = ul.x;
            ll.y = ToViewportY(y + glyph->yoffset + glyph->height);
            lr.x = ur.x;
            lr.y = ll.y;

            ul.u = glyph->u0;
            ul.v = glyph->v0;
            ur.u = glyph->u1;
            ur.v = glyph->v0;
            ll.u = glyph->u0;
            ll.v = glyph->v1;
            lr.u = glyph->u1;
            lr.v = glyph->v1;

            *dst_vertex++ = ul;
            *dst_vertex++ = ur;
            *dst_vertex++ = ll;
            *dst_vertex++ = lr;

            *dst_index++ = vertexCount + 0;
            *dst_index++ = vertexCount + 1;
            *dst_index++ = vertexCount + 2;
            *dst_index++ = vertexCount + 2;
            *dst_index++ = vertexCount + 1;
            *dst_index++ = vertexCount + 3;
            vertexCount += 4;

            x += glyph->xadvance;
        }
    }

    int numIndices = (int) (dst_index - (uint32*)ib.pData);

    pCtxt->Unmap(vertex_buffer, 0);
    pCtxt->Unmap(index_buffer, 0);

	Shader::Set(font_sampler, font->texture);
	Shader::Commit(pCtxt, ShaderText);
	
    //pCtxt->PSSetShaderResources(0, 1, &font->pTexture);

    // Draw the instances
    UINT vbStride = sizeof(Vertex), vbOffset = 0;
    pCtxt->IASetVertexBuffers(0, 1, &vertex_buffer, &vbStride, &vbOffset);
    pCtxt->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
    pCtxt->IASetInputLayout(input_layout);
    pCtxt->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pCtxt->DrawIndexed(numIndices, 0, 0);

    text_buffer.Reset();
}
