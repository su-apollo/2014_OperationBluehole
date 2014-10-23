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

	mD3DDevice = Renderer::GetInstance()->GetDevice();
	mD3DDeviceContext = Renderer::GetInstance()->GetDeviceContext();
	D3DXMatrixIdentity(&mWorld);



	//fbx로부터 정보를 가져와 mModel을 채운다. 
	if (!LoadFBX())
	{
		return false;
	}


	if (!CompileShader())
		return FALSE;

	//매쉬 전체로 바꿔야함 위험함.


	for (unsigned int i = 0; i < mModel.size(); ++i)
	{
		if (!CreateMeshBuffer(mModel[i]))
			return FALSE;
	}

	LoadTexture();

	D3DXMATRIX matRotate;
	D3DXMatrixRotationY(&matRotate, static_cast<float>(180.0*(3.14 / 180.0)));
	D3DXMatrixRotationX(&matRotate, static_cast<float>(-90.0*(3.14 / 180.0)));

	mWorld *= matRotate;

	LightManager::GetInstance()->CreateDirectionalLight(MAX_LIGHT);




	return TRUE;

}

BOOL Elin::LoadFBX()
{
	FbxImporter* fbxImporter = FbxImporter::Create(mFbxManager, "myImporter");

	if (!fbxImporter)
	{
		return false;
	}

	const char* filePath = "ElinModel/Popori_F_H00_dance.FBX";

	if (!fbxImporter->Initialize(filePath, -1, mFbxManager->GetIOSettings()))
	{
		return false;
	}

	if (!fbxImporter->Import(mFbxScene))
	{
		return false;
	}
	fbxImporter->Destroy();

	ProcessGeometry(mFbxScene->GetRootNode());

	return TRUE;
}



void Elin::ProcessGeometry(FbxNode* inNode)
{
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
			EMesh pNewMesh (new Mesh);
			FbxMesh* pMesh = (FbxMesh*)childNode->GetNodeAttribute();


			FbxLayerElementArrayTemplate<FbxVector4>* normal = 0;
			pMesh->GetNormals(&normal);
			FbxLayerElementArrayTemplate<FbxVector2>* uv = 0;
			pMesh->GetTextureUV(&uv, FbxLayerElement::eTextureDiffuse);


			//fbx매쉬 생성
			if (pMesh == NULL)
			{
				Log("Mesh is null\n");
			}
			else
			{
				// ========= Get the Vertices ==============================
				// Fill all the mVertices.
				int numVerts = pMesh->GetControlPointsCount();
				pNewMesh->mNumVertex = numVerts;
				Vertex tempVerts;

				for (int j = 0; j < numVerts; j++)
				{
					D3DXVECTOR3 currPosition;
					currPosition.x = static_cast<float>(pMesh->GetControlPointAt(j).mData[0]);
					currPosition.y = static_cast<float>(pMesh->GetControlPointAt(j).mData[1]);
					currPosition.z = static_cast<float>(pMesh->GetControlPointAt(j).mData[2]);

					tempVerts.mPos = currPosition;



					pNewMesh->mVertex.push_back(tempVerts);
				}

				if (numVerts == 0)
					return;

				pNewMesh->mNumPolygon = pMesh->GetPolygonCount();


				

				//get all UV set names
				FbxStringList lUVSetNameList;
				pMesh->GetUVSetNames(lUVSetNameList);




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

						//순서 찾아볼 것.
						pNewMesh->mVertex[iControlPointIndex].mNormal.x = (float)normal.mData[0];
						pNewMesh->mVertex[iControlPointIndex].mNormal.y = (float)normal.mData[1];
						pNewMesh->mVertex[iControlPointIndex].mNormal.z = (float)normal.mData[2];


						// ========= Get the Indices ==============================

						const char* lUVSetName = lUVSetNameList.GetStringAt(0);
						FbxVector2 tex;
						bool isMapped;
						pMesh->GetPolygonVertexUV(j,k, lUVSetName, tex, isMapped);
						pNewMesh->mVertex[iControlPointIndex].mUV.x = tex[0];
						pNewMesh->mVertex[iControlPointIndex].mUV.y = -tex[1];


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
					//이거 순서 어떻게?
					pNewMesh->mIndices.push_back(tempIndex.i0);
					pNewMesh->mIndices.push_back(tempIndex.i1);
					pNewMesh->mIndices.push_back(tempIndex.i2);
				}
				pNewMesh->mNumIndex = pNewMesh->mIndices.size();
				mModel.push_back(pNewMesh);

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
}

BOOL Elin::CompileShader()
{
	if (!CompileVertexShader())
		return FALSE;

	if (!CompilePixelShader())
		return FALSE;

	return TRUE;
}


void Elin::Render()
{

	// rotate
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY(&matRotate, Timer::GetInstance()->GetDeltaTime());
	mWorld *= matRotate;

	// update constbuff
	D3DXMATRIX matWorld;
	D3DXMATRIX matView = Camera::GetInstance()->GetMatView();
	D3DXMATRIX matProj = Camera::GetInstance()->GetMatProj();
	VSConstantBuffer vcb;

	// 열 우선배치
	D3DXMatrixTranspose(&matWorld, &mWorld);
	D3DXMatrixTranspose(&matView, &matView);
	D3DXMatrixTranspose(&matProj, &matProj);
	vcb.mWorld = matWorld;
	vcb.mView = matView;
	vcb.mProjection = matProj;
	mD3DDeviceContext->UpdateSubresource(mVSConstBuffer, 0, NULL, &vcb, 0, 0);

	PSConstantBuffer pcb;
	DLightPointer light1 = LightManager::GetInstance()->mDLightList[0];
	DLightPointer light2 = LightManager::GetInstance()->mDLightList[1];
	pcb.vLightDir[0] = light1->GetDirection();
	pcb.vLightDir[1] = light2->GetDirection();
	pcb.vLightColor[0] = light1->GetColor();
	pcb.vLightColor[1] = light2->GetColor();
	mD3DDeviceContext->UpdateSubresource(mPSConstBuffer, 0, NULL, &pcb, 0, 0);

	// draw
	mD3DDeviceContext->VSSetShader(mVertexShader, NULL, 0);
	mD3DDeviceContext->VSSetConstantBuffers(0, 1, &mVSConstBuffer);
	mD3DDeviceContext->PSSetShader(mPixelShader, NULL, 0);
	mD3DDeviceContext->PSSetConstantBuffers(0, 1, &mPSConstBuffer);



	for (unsigned int i = 0; i < mMeshData.size(); ++i)
	{
		RenderMesh(mMeshData[i]);
	}
}

void Elin::RenderMesh(EMeshData meshData)
{
	mD3DDeviceContext->IASetInputLayout(mVertexLayout11);

	// Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mD3DDeviceContext->IASetVertexBuffers(0, 1, &meshData->mVertexBuffer, &stride, &offset);

	// Set index buffer
	mD3DDeviceContext->IASetIndexBuffer(meshData->mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set primitive topology
	mD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mD3DDeviceContext->PSSetShaderResources(0, 1, &meshData->mTextureRVDiff);
	mD3DDeviceContext->PSSetShaderResources(1, 1, &meshData->mTextureRVNorm);
	mD3DDeviceContext->PSSetShaderResources(2, 1, &meshData->mTextureRVSpec);

	mD3DDeviceContext->PSSetSamplers(0, 1, &meshData->mSamplerLinear);

	mD3DDeviceContext->DrawIndexed(meshData->mNumIndex, 0, 0);
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
	ID3DBlob* pVSBlob = NULL;
	hr = CompileShaderFromFile(L"VertexShader1.hlsl", "main", VS_MODEL, &pVSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), NULL, &mVertexShader);

	if (FAILED(hr))
	{
		SafeRelease(pVSBlob);
		return FALSE;
	}

	// input layout
	// 이거 offset 계산 신경써야함
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = mD3DDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &mVertexLayout11);

	SafeRelease(pVSBlob);

	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

BOOL Elin::CompilePixelShader()
{
	ID3DBlob* pPSBlob = NULL;
	//hr = CompileShaderFromFile(const_cast<WCHAR*>(PS_PATH), PS_MAIN, PS_MODEL, &pPSBlob);
	hr = CompileShaderFromFile(L"PixelShader1.hlsl", "main", PS_MODEL, &pPSBlob);
	if (FAILED(hr))
		return FALSE;

	hr = mD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(), NULL, &mPixelShader);

	SafeRelease(pPSBlob);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}



void Elin::Release()
{
	CleanUp();
	SafeRelease(mVSConstBuffer);
	SafeRelease(mPSConstBuffer);
	SafeRelease(mVertexLayout11);
	SafeRelease(mVertexShader);
	SafeRelease(mPixelShader);


}


BOOL Elin::CreateModelBuffer()
{
	//
	return TRUE;
}


BOOL Elin::CreateMeshBuffer(EMesh mesh)
{
	EMeshData pMeshData(new MeshData);
	mMeshData.push_back(pMeshData);
	if (!CreateMeshVB(mesh, mMeshData.back()))
		return FALSE;
	if (!CreateMeshIB(mesh, mMeshData.back()))
		return FALSE;
	if (!CreateMeshCB(mesh, mMeshData.back()))
		return FALSE;

	return TRUE;
}

BOOL Elin::CreateMeshVB(EMesh mesh, EMeshData meshData)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	// Create vertex buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vertex)* mesh->mNumVertex;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = &(mesh->mVertex.front());
	hr = mD3DDevice->CreateBuffer(&bd, &InitData, &meshData->mVertexBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}


BOOL Elin::CreateMeshIB(EMesh mesh, EMeshData meshData)
{
	meshData->mNumIndex = mesh->mNumIndex;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	// Create index buffer
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int)* mesh->mNumIndex;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = &(mesh->mIndices.front());
	hr = mD3DDevice->CreateBuffer(&bd, &InitData, &meshData->mIndexBuffer);
	if (FAILED(hr))
		return FALSE;

	return TRUE;

}


BOOL Elin::CreateMeshCB(EMesh mesh, EMeshData meshData)
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

BOOL Elin::LoadTexture()
{
	mModel[0]->mTexutreDiff = ELIN_TEXTURE_HAND_DIFF;
	mModel[0]->mTexutreNorm = ELIN_TEXTURE_HAND_NORM;
	mModel[0]->mTexutreSpec = ELIN_TEXTURE_HAND_SPEC;

	mModel[1]->mTexutreDiff = ELIN_TEXTURE_LEG_DIFF;
	mModel[1]->mTexutreNorm = ELIN_TEXTURE_LEG_NORM;
	mModel[1]->mTexutreSpec = ELIN_TEXTURE_LEG_SPEC;

	mModel[2]->mTexutreDiff = ELIN_TEXTURE_BODY_DIFF;
	mModel[2]->mTexutreNorm = ELIN_TEXTURE_BODY_NORM;
	mModel[2]->mTexutreSpec = ELIN_TEXTURE_BODY_SPEC;

	mModel[3]->mTexutreDiff = ELIN_TEXTURE_HAIR_DIFF;
	mModel[3]->mTexutreNorm = ELIN_TEXTURE_HAIR_NORM;
	mModel[3]->mTexutreSpec = ELIN_TEXTURE_HAIR_SPEC;

	mModel[4]->mTexutreDiff = ELIN_TEXTURE_FACE_DIFF;
	mModel[4]->mTexutreNorm = ELIN_TEXTURE_FACE_NORM;
	mModel[4]->mTexutreSpec = ELIN_TEXTURE_FACE_SPEC;

	for (unsigned int i = 0; i < mModel.size(); ++i)
	{
		if (!LoadMeshTexture(mModel[i],mMeshData[i]))
			return FALSE;
	}

	return TRUE;
}

BOOL Elin::LoadMeshTexture(EMesh mesh, EMeshData meshData)
{
	// Load the Texture
	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, mesh->mTexutreDiff, NULL, NULL, &meshData->mTextureRVDiff, NULL);
	if (FAILED(hr))
		return FALSE;

	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, mesh->mTexutreNorm, NULL, NULL, &meshData->mTextureRVNorm, NULL);
	if (FAILED(hr))
		return FALSE;

	hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, mesh->mTexutreSpec, NULL, NULL, &meshData->mTextureRVSpec, NULL);
	if (FAILED(hr))
		return FALSE;

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	// 선형 필터
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = mD3DDevice->CreateSamplerState(&sampDesc, &meshData->mSamplerLinear);
	if (FAILED(hr))
		return FALSE;

	return TRUE;
}
