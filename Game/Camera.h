#pragma once

class Camera
{
public:
    Camera();

    void Update();

    void MoveTo(XMVECTOR new_eye, float new_theta = 0, float new_phi = 0);

    XMMATRIX view, proj;

protected:
    void BuildMatrices();

    XMVECTOR eye;
    float theta, phi;
};
