#pragma once
#include "PostProcessor.h"




class Billboard
{
public:
	Billboard();
	virtual ~Billboard();






private:

	ID3D11Buffer*           mVertexBuffer = NULL;
	ID3D11Buffer*           mIndexBuffer = NULL;




};

