#include "stdafx.h"
#include "Elin.h"





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
	return TRUE;
}
