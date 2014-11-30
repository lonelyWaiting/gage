class Particles
{
public:
    Particles(ID3D11Device* device, ID3D11DeviceContext* ctxt);
    virtual ~Particles();

    void Update();

    void Render(ID3D11DeviceContext* ctxt, float time, const XMMATRIX& view, const XMMATRIX& proj);

	float time_since_last_restart;
};
