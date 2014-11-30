#pragma once

struct Mesh;

class Entity
{
public:
    Entity(ID3D11Device* device, const char* model_name);
    virtual ~Entity();

    void Update();

    void Render(ID3D11DeviceContext* ctxt, float time, const XMMATRIX& view, const XMMATRIX& proj);

    Mesh* mesh;
};
