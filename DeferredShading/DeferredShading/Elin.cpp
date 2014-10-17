#include "stdafx.h"
#include "Elin.h"
#include "MacroSet.h"





Elin::Elin()
{
}


Elin::~Elin()
{
}

BOOL Elin::Initialize()
{
	mFbxManager = FbxManager::Create();
	if (!mFbxManager)
	{
		return false;
	}

	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(mFbxManager, IOSROOT);
	mFbxManager->SetIOSettings(fbxIOSettings);

	mFbxScene = FbxScene::Create(mFbxManager, "myScene");

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
	//get data only from mesh (FbxNodeAttribute::eMesh)
	if (inNode->GetNodeAttribute())
	{
		switch (inNode->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			//1.get control points
			ProcessControlPoints(inNode);
			//2.traverse vertices from triangles
			//ProcessMesh(inNode);

			break;
		}
	}

	for (int i = 0; i < inNode->GetChildCount(); ++i)
	{
		ProcessGeometry(inNode->GetChild(i));
	}


}


void Elin::ProcessControlPoints(FbxNode* inNode)
{

	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int ctrlPointCount = currMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		ControlPoint* currCtrlPoint = new ControlPoint();
		D3DXVECTOR3 currPosition;
		currPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
		currCtrlPoint->mPos = currPosition;
		mControlPoints[i] = currCtrlPoint;
	}

}


void Elin::ProcessMesh(FbxNode* inNode)
{
	FbxMesh* currMesh = inNode->GetMesh();

	mTriangleCount = currMesh->GetPolygonCount();
	int vertexCounter = 0;
	mTriangles.reserve(mTriangleCount);

	for (unsigned int i = 0; i < mTriangleCount; ++i)
	{
		Triangle currTriangle;
		mTriangles.push_back(currTriangle);

		for (unsigned int j = 0; j < 3; ++j)
		{
			int ctrlPointIndex = currMesh->GetPolygonVertex(i, j);
			ControlPoint* currCtrlPoint = mControlPoints[ctrlPointIndex];
			Vertex temp;
			temp.mPos = currCtrlPoint->mPos;

			mVertices.push_back(temp);
			mTriangles.back().mIndices.push_back(vertexCounter);
			++vertexCounter;
		}
	}






	//we don't need controlpoints anymore!
	for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	{
		delete itr->second;
	}
	mControlPoints.clear();
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
