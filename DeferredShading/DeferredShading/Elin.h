#pragma once
#include <fbxsdk.h>

struct Position
{
	float pos[3];
};



class Elin
{
public:
	Elin();
	~Elin();

	BOOL Initialize();
	BOOL LoadFBX();


private:
	FbxManager* mFbxManager = nullptr;
	FbxImporter* mImporter = nullptr;
	FbxScene* mFbxScene = nullptr;
	FbxNode* mFbxRootNode = nullptr;

	void ProcessControlPoints();
	void ReadNoraml();
	void Cleanup();

};

