#pragma once




struct CubeVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 color;
};


class Cube
{
public:
	Cube();
	~Cube();

	ID3D11Buffer*           mVertexBuffer = NULL;
	ID3D11Buffer*           mIndexBuffer = NULL;



};

