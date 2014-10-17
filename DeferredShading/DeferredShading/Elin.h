#pragma once
#include <fbxsdk.h>
#include <xnamath.h>
#include <unordered_map>

struct Vertex
{
	D3DXVECTOR3 mPos;
	//D3DXVECTOR3 mNormal;
	//uv
};

struct ControlPoint
{
	D3DXVECTOR3 mPos;
};

struct Triangle
{
	std::vector<unsigned int> mIndices;
};




class Elin
{
public:
	Elin();
	~Elin();

	BOOL Initialize();
	BOOL LoadFBX();
	void CleanUp();


private:
	FbxManager* mFbxManager = nullptr;
	FbxImporter* mImporter = nullptr;
	FbxScene* mFbxScene = nullptr;

	void ProcessGeometry(FbxNode* inNode);
	void ProcessControlPoints(FbxNode* inNode);
	void ProcessMesh(FbxNode* inNode);
	





	std::string GetFileName(const char* fileName);

	std::unordered_map<unsigned int, ControlPoint*> mControlPoints;
	unsigned int mTriangleCount;
	std::vector<Triangle> mTriangles;
	std::vector<Vertex> mVertices;

};

