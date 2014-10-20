#include "stdafx.h"
#include "Elin.h"
#include "Renderer.h"
#include "LightManager.h"
#include "Timer.h"
#include "Logger.h"


Elin::Elin()
{
}


Elin::~Elin()
{
}

BOOL Elin::Init()
{
	mFbxManager = FbxManager::Create();
	if (!mFbxManager)
	{
		return false;
	}

	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(mFbxManager, IOSROOT);
	mFbxManager->SetIOSettings(fbxIOSettings);

	mFbxScene = FbxScene::Create(mFbxManager, "myScene");

	if (!LoadFBX())
	{
		return false;
	}

	//
	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();
	D3DXMatrixIdentity(&mWorld);
	mPolygonCount = 0;


	if (!CompileShader())
		return FALSE;

	if (!CreateBuffer())
		return FALSE;


	LightManager::GetInstance()->CreateDirectionalLight(MAX_LIGHT);
	//

	return TRUE;

}

BOOL Elin::LoadFBX()
{
	FbxImporter* fbxImporter = FbxImporter::Create(mFbxManager, "myImporter");

	if (!fbxImporter)
	{
		return false;
	}

	const char* filePath = "C:\\ElinModel\\Popori_F_H00_dance.FBX";
	//const char* filePath = "C:\\kim\\MI.FBX";

	if (!fbxImporter->Initialize(filePath, -1, mFbxManager->GetIOSettings()))
	{
		return false;
	}

	if (!fbxImporter->Import(mFbxScene))
	{
		return false;
	}
	fbxImporter->Destroy();

	//std::string onlyFileName = GetFileName(filePath);

	ProcessGeometry(mFbxScene->GetRootNode());

	return TRUE;
}



void Elin::ProcessGeometry(FbxNode* inNode)
{
	FbxMesh *pNewMesh = nullptr;

	if (inNode)
	{
		int ChildCount = inNode->GetChildCount();

		for (int i = 0; i < ChildCount; i++)
		{
			FbxNode* childNode = inNode->GetChild(i);

			if (childNode->GetNodeAttribute() == NULL)
				continue;

			if (childNode->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = (FbxMesh*)childNode->GetNodeAttribute();

			FbxLayerElementArrayTemplate<FbxVector4>* normal = 0;
			pMesh->GetNormals(&normal);
			FbxLayerElementArrayTemplate<FbxVector2>* uv = 0;
			pMesh->GetTextureUV(&uv, FbxLayerElement::eTextureDiffuse);

			FbxVector4* pVertices = pMesh->GetControlPoints();
			const int lVertexCount = pMesh->GetControlPointsCount();

			//fbx매쉬 생성
			if (pMesh == NULL)
			{
				Log("Mesh is null\n");
			}
			else
			{
				// ========= Get the Vertices ==============================
				int numVerts = pMesh->GetControlPointsCount();
				Vertex tempVerts;

				for (int j = 0; j < numVerts; j++)
				{
					D3DXVECTOR3 currPosition;
					currPosition.x = static_cast<float>(pMesh->GetControlPointAt(i).mData[0]);
					currPosition.y = static_cast<float>(pMesh->GetControlPointAt(i).mData[1]);
					currPosition.z = static_cast<float>(pMesh->GetControlPointAt(i).mData[2]);

					tempVerts.mPos = currPosition;

					float tU;
					float tV;

					tU = (float)(*uv)[j].mData[0];
					tV = (float)(*uv)[j].mData[1];
					tempVerts.mTU = tU;
					tempVerts.mTV = tV;

					mVertices.push_back(tempVerts);
				}

				if (numVerts == 0)
					return;

				for (int j = 0; j < pMesh->GetPolygonCount(); j++)
				{
					int iNumVertices = pMesh->GetPolygonSize(j);

					if (iNumVertices != 3)
						return;

					Indices tempIndex;

					for (int k = 0; k < iNumVertices; k++)
					{
						int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

						FbxVector4 normal;
						pMesh->GetPolygonVertexNormal(j, k, normal);

						// ========= Get the Normals ==============================
						D3DXVECTOR3 currNormal;
						//이게 좀 많이 헷갈림
						// 					tempVerts[iControlPointIndex].mNormal.x = (float)normal.mData[0];
						// 					tempVerts[iControlPointIndex].mNormal.y = (float)normal.mData[1];
						// 					tempVerts[iControlPointIndex].mNormal.z = (float)normal.mData[2];
						mVertices[iControlPointIndex].mNormal.x = (float)normal.mData[0];
						mVertices[iControlPointIndex].mNormal.y = (float)normal.mData[1];
						mVertices[iControlPointIndex].mNormal.z = (float)normal.mData[2];

						// ========= Get the Indices ==============================
						switch (k)
						{
						case 0:
							tempIndex.i0 = iControlPointIndex;
							break;
						case 1:
							tempIndex.i1 = iControlPointIndex;
							break;
						case 2:
							tempIndex.i2 = iControlPointIndex;
							break;
						default:
							break;
						}
					}
					//mIndex.push_back(tempIndex);
					mIndices.push_back(tempIndex.i0);
					mIndices.push_back(tempIndex.i1);
					mIndices.push_back(tempIndex.i2);

				}
			} // else
			ProcessGeometry(childNode);
		}
	}
}

std::string Elin::GetFileName(const char* filePath)
{
	//get pure file name
	std::string fileName = filePath;
	std::string seperator("\\");
	unsigned int pos = fileName.find_last_of(seperator);
	if (pos != std::string::npos)
	{
		fileName = fileName.substr(pos + 1);
	}
	seperator = ".";
	pos = fileName.find_last_of(seperator);
	if (pos != std::string::npos)
	{
		return fileName.substr(0, pos);
	}
	else
	{
		return fileName;
	}
}

void Elin::CleanUp()
{
	mFbxScene->Destroy();
	mFbxManager->Destroy();

	mIndices.clear();

	mVertices.clear();

}

BOOL Elin::CompileShader()
{
	if (!CompileVertexShader())
		return FALSE;

	if (!CompilePixelShader())
		return FALSE;

	return TRUE;
}

BOOL Elin::CreateBuffer()
{
	if (!CreateVertexBuff())
		return FALSE;
	if (!CreateIndexBuff())
		return FALSE;
	if (!CreateConstBuff())
		return FALSE;
	return TRUE;
}

void Elin::Render()
{
	//rotate
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY(&matRotate, Timer::GetInstance()->GetDeltaTime());
	mWorld *= matRotate;

	//update constbuff
	D3DXMATRIX matWorld;
	D3DXMATRIX matView = Camera::GetInstance()->GetMatView();
	D3DXMATRIX matProj = Camera::GetInstance()->GetMatProj();
	EVSConstantBuffer vcb;

	//열 우선배치
	D3DXMatrixTranspose(&matWorld, &mWorld);
	D3DXMatrixTranspose(&matView, &matView);
	D3DXMatrixTranspose(&matProj, &matProj);
	vcb.mWorld = matWorld;
	vcb.mView = matView;
	vcb.mProjection = matProj;
	mD3DDeviceContext->UpdateSubresource(mVSConstBuffer, 0, NULL, &vcb, 0, 0);

	EPSConstantBuffer pcb;
	DLightPointer light1 = LightManager::GetInstance()->mDLightList[0];
	DLightPointer light2 = LightManager::GetInstance()->mDLightList[1];
	pcb.vLightDir[0] = light1->GetDirection();
	pcb.vLightDir[1] = light2->GetDirection();
	pcb.vLightColor[0] = light1->GetColor();
	pcb.vLightColor[1] = light2->GetColor();
	mD3DDeviceContext->UpdateSubresource(mPSConstBuffer, 0, NULL, &pcb, 0, 0);

	//draw
	mD3DDeviceContext->VSSetShader(mVertexShader, NULL, 0);
	mD3DDeviceContext->VSSetConstantBuffers(0, 1, &mVSConstBuffer);
	mD3DDeviceContext->PSSetShader(mPixelShader, NULL, 0);
	mD3DDeviceContext->PSSetConstantBuffers(0, 1, &mPSConstBuffer);
	mD3DDeviceContext->DrawIndexed(mIndices.size(), 0, 0);

}

HRESULT Elin::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}

BOOL Elin::CompileVertexShader()
{
	//
	return TRUE;
}

BOOL Elin::CompilePixelShader()
{
	//
	return TRUE;
}

BOOL Elin::CreateVertexBuff()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	// Create vertex buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	//문제
	int numVertices = mVertices.size();
	bd.ByteWidth = sizeof(Vertex) * numVertices;

	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = &mVertices;
	hr = Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &InitData, &mVertexBuffer);
	if (FAILED(hr))
		return FALSE;

	// Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Renderer::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	return TRUE;
}

BOOL Elin::CreateIndexBuff()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	// Create index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	int numTriangle = mIndices.size();
	bd.ByteWidth = sizeof(int) * numTriangle;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = &mIndices;
	hr = Renderer::GetInstance()->GetDevice()->CreateBuffer(&bd, &InitData, &mIndexBuffer);
	if (FAILED(hr))
		return FALSE;

	// Set index buffer
	Renderer::GetInstance()->GetDeviceContext()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set primitive topology
	Renderer::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return TRUE;
}

BOOL Elin::CreateConstBuff()
{
	D3D11_BUFFER_DESC bd;
	//아래 zeromemory를 꼭해야함
	ZeroMemory(&bd, sizeof(bd));
	// Create the constant buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VSConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = mD3DDevice->CreateBuffer(&bd, NULL, &mVSConstBuffer);
	if (FAILED(hr))
		return FALSE;

	bd.ByteWidth = sizeof(PSConstantBuffer);
	hr = mD3DDevice->CreateBuffer(&bd, NULL, &mPSConstBuffer);
	if (FAILED(hr))
		return FALSE;


	return TRUE;
}

void Elin::Release()
{
	// 	Saferelease(mVSConstBuffer);
	// 	Saferelease(mPSConstBuffer);
	// 	Saferelease(mVertexBuffer);
	// 	Saferelease(mIndexBuffer);
	// 
	// 	Saferelease(mVertexLayout11);
	// 	Saferelease(mVertexShader);
	// 	Saferelease(mPixelShader);
}

