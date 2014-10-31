#pragma once
#include <fbxsdk.h>
#include "Singleton.h"

struct Vertex
{
	D3DXVECTOR3 mPos;
	D3DXVECTOR3 mNormal;
	D3DXVECTOR3 mTangent;
	D3DXVECTOR2 mUV;
};

struct Mesh
{
	std::vector<Vertex>			mVertex;
	std::vector<unsigned int>	mIndices;
	int				mNumPolygon = 0;
	int				mNumVertex = 0;
	int				mNumIndex = 0;

	const WCHAR*	mTexutreDiff = NULL;
	const WCHAR*	mTexutreNorm = NULL;
	const WCHAR*	mTexutreSpec = NULL;

	D3DXMATRIX		mWorld;
};

struct Indices
{
	int i0, i1, i2;
};

typedef std::shared_ptr<Mesh> MeshPointer;

class ModelManager : public Singleton<ModelManager>
{
public:
	ModelManager();
	~ModelManager();

	BOOL			Init();
	FbxScene*		MakeFbxSceneFromFile(const char* filePath);
	MeshPointer		ProcessMesh(FbxNode* inNode);
	void			CleanUp();


private:
	FbxManager* mFbxManager = nullptr;
	FbxImporter* mImporter = nullptr;
	FbxScene* mFbxScene = nullptr;
};