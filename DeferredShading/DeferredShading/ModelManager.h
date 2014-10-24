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
	std::vector<Vertex> mVertex;
	std::vector<unsigned int> mIndices;
	int mNumPolygon;
	int mNumVertex;
	int mNumIndex;

	const WCHAR* mTexutreDiff;
	const WCHAR* mTexutreNorm;
	const WCHAR* mTexutreSpec;
};

struct Indices
{
	int i0, i1, i2;
};

typedef std::shared_ptr<Mesh> EMesh;


class ModelManager : public Singleton<ModelManager>
{
public:
	ModelManager();
	~ModelManager();

	BOOL			Init();
	FbxScene*		MakeFbxSceneFromFile(const char* filePath);
	EMesh			ProcessMesh(FbxNode* inNode);
	void			CleanUp();


private:
	FbxManager* mFbxManager = nullptr;
	FbxImporter* mImporter = nullptr;
	FbxScene* mFbxScene = nullptr;
};