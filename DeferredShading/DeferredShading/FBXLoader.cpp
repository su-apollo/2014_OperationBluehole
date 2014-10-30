#include "stdafx.h"
#include "FBXLoader.h"
#include "Logger.h"





FBXLoader::FBXLoader()
{
}


FBXLoader::~FBXLoader()
{
	if (mScene)
	{
		mScene->Destroy();
		mScene = nullptr;
	}

	if (mSdkManager)
	{
		mSdkManager->Destroy();
		mSdkManager = nullptr;
	}
}

BOOL FBXLoader::Init()
{
	mSdkManager = FbxManager::Create();
	if (!mSdkManager)
		return FALSE;

	FbxIOSettings* ios = FbxIOSettings::Create(mSdkManager, IOSROOT);
	mSdkManager->SetIOSettings(ios);

	FbxString lPath = FbxGetApplicationDirectory();
	mSdkManager->LoadPluginsDirectory(lPath.Buffer());

	mScene = FbxScene::Create(mSdkManager, "My Scene");
	if (!mScene)
		return FALSE;

	return TRUE;
}

BOOL FBXLoader::LoadFBX(char* filename)
{
	if (!filename)
		return FALSE;

	int lFileFormat = -1;
	mImporter = FbxImporter::Create(mSdkManager, "");

	if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(filename, lFileFormat))
		lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");

	if (!mImporter || mImporter->Initialize(filename, lFileFormat) == false)
		return FALSE;

	if (!mImporter || mImporter->Import(mScene) == false)
		return FALSE;

	FbxAxisSystem OurAxisSystem = FbxAxisSystem::DirectX;

	//ÁÂÇ¥°è ¼³Á¤
	FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
	if (SceneAxisSystem != OurAxisSystem)
		FbxAxisSystem::DirectX.ConvertScene(mScene);

	//
	FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
	if (SceneSystemUnit.GetScaleFactor() != 1.0)
		FbxSystemUnit::cm.ConvertScene(mScene);
	
	TriangulateRecursive(mScene->GetRootNode());



	return TRUE;
}

void FBXLoader::TriangulateRecursive(FbxNode* pNode)
{
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute)
	{
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbsSurface ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
		{
			FbxGeometryConverter lConverter(pNode->GetFbxManager());
			lConverter.Triangulate(mScene, true);
		}
	}

	const int lChildCount = pNode->GetChildCount();
	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
		TriangulateRecursive(pNode->GetChild(lChildIndex));
}

void FBXLoader::Setup()
{
	if (mScene->GetRootNode())
		SetupNode(mScene->GetRootNode(), "null");
}

void FBXLoader::SetupNode(FbxNode* pNode, std::string parentName)
{
	if (!pNode)
		return;

	FBX_MESH_NODE meshNode;

	meshNode.name = pNode->GetName();
	meshNode.parentName = parentName;

	ZeroMemory(&meshNode.elements, sizeof(MESH_ELEMENTS));

	FbxMesh* lMesh = pNode->GetMesh();

	if (lMesh)
	{
		const int lVertexCount = lMesh->GetControlPointsCount();

		if (lVertexCount > 0)
			CopyVertexData(lMesh, &meshNode);
	}

	// todo : ¿¤¸°¸ðµ¨ È®ÀÎ
	const int lMaterialCount = pNode->GetMaterialCount();
	for (int i = 0; i < lMaterialCount; i++)
	{
		FbxSurfaceMaterial* mat = pNode->GetMaterial(i);
		if (!mat)
			continue;

		FBX_MATERIAL_NODE destMat;
		CopyMatrialData(mat, &destMat);

		meshNode.materialArray.push_back(destMat);
	}

	ComputeNodeMatrix(pNode, &meshNode);

	mMeshNodeArray.push_back(meshNode);

	const int lCount = pNode->GetChildCount();
	for (int i = 0; i < lCount; i++)
		SetupNode(pNode->GetChild(i), meshNode.name);
}

void FBXLoader::CopyVertexData(FbxMesh* pMesh, FBX_MESH_NODE* meshNode)
{
	if (!pMesh)
		return;

	int lPolygonCount = pMesh->GetPolygonCount();

	FbxVector4 pos, nor;

	meshNode->elements.numPosition = 1;
	meshNode->elements.numNormal = 1;

	unsigned int indx = 0;

	for (int i = 0; i < lPolygonCount; i++)
	{
		int lPolygonsize = pMesh->GetPolygonSize(i);

		for (int pol = 0; pol < lPolygonsize; pol++)
		{
			int index = pMesh->GetPolygonVertex(i, pol);
			meshNode->indexArray.push_back(indx);

			pos = pMesh->GetControlPointAt(index);
			pMesh->GetPolygonVertexNormal(i, pol, nor);

			meshNode->positionArray.push_back(pos);
			meshNode->normalArray.push_back(nor);

			++indx;
		}
	}

	FbxStringList	uvsetName;
	pMesh->GetUVSetNames(uvsetName);
	// todo : ¿¤¸°¸ðµ¨ °¹¼ö È®ÀÎ
	int numUVSet = uvsetName.GetCount();
	meshNode->elements.numUVSet = numUVSet;

	bool unmapped = false;

	for (int uv = 0; uv < numUVSet; uv++)
	{
		meshNode->uvsetID[uvsetName.GetStringAt(uv)] = uv;
		for (int i = 0; i < lPolygonCount; i++)
		{
			int lPolygonsize = pMesh->GetPolygonSize(i);

			for (int pol = 0; pol < lPolygonsize; pol++)
			{
				FbxString name = uvsetName.GetStringAt(uv);
				FbxVector2 texCoord;
				pMesh->GetPolygonVertexUV(i, pol, name, texCoord, unmapped);
				meshNode->texcoordArray.push_back(texCoord);
			}
		}
	}
}

void FBXLoader::CopyMatrialData(FbxSurfaceMaterial* mat, FBX_MATERIAL_NODE* destMat)
{
	if (!mat)
		return;

	if (mat->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		destMat->type = FBX_MATERIAL_NODE::MATERIAL_LAMBERT;
	}
	else if (mat->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		destMat->type = FBX_MATERIAL_NODE::MATERIAL_PHONG;
	}

	const FbxDouble3 lEmissive = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, &destMat->emmisive);
	SetFbxColor(destMat->emmisive, lEmissive);

	const FbxDouble3 lAmbient = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, &destMat->ambient);
	SetFbxColor(destMat->ambient, lAmbient);

	const FbxDouble3 lDiffuse = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, &destMat->diffuse);
	SetFbxColor(destMat->diffuse, lDiffuse);

	const FbxDouble3 lSpecular = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, &destMat->specular);
	SetFbxColor(destMat->specular, lSpecular);

	//
	FbxProperty lTransparencyFactorProperty = mat->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (lTransparencyFactorProperty.IsValid())
	{
		double lTransparencyFactor = lTransparencyFactorProperty.Get<FbxDouble>();
		destMat->TransparencyFactor = static_cast<float>(lTransparencyFactor);
	}

	// Specular Power
	FbxProperty lShininessProperty = mat->FindProperty(FbxSurfaceMaterial::sShininess);
	if (lShininessProperty.IsValid())
	{
		double lShininess = lShininessProperty.Get<FbxDouble>();
		destMat->shininess = static_cast<float>(lShininess);
	}
}

void FBXLoader::ComputeNodeMatrix(FbxNode* pNode, FBX_MESH_NODE* meshNode)
{
	if (!pNode || !meshNode)
		return;

	FbxAnimEvaluator* lEvaluator = mScene->GetAnimationEvaluator();
	FbxMatrix lGlobal;
	lGlobal.SetIdentity();

	if (pNode != mScene->GetRootNode())
	{
		lGlobal = lEvaluator->GetNodeGlobalTransform(pNode);
		FBXMatrixToFloat16(&lGlobal, meshNode->mat4x4);
	}
	else
	{
		FBXMatrixToFloat16(&lGlobal, meshNode->mat4x4);
	}
}

FbxDouble3 FBXLoader::GetMaterialProperty(const FbxSurfaceMaterial * pMaterial, const char * pPropertyName, const char * pFactorPropertyName, FBX_MATRIAL_ELEMENT* pElement)
{
	pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_NONE;

	FbxDouble3 lResult(0, 0, 0);
	const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
	const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
	if (lProperty.IsValid() && lFactorProperty.IsValid())
	{
		lResult = lProperty.Get<FbxDouble3>();
		double lFactor = lFactorProperty.Get<FbxDouble>();
		if (lFactor != 1)
		{
			lResult[0] *= lFactor;
			lResult[1] *= lFactor;
			lResult[2] *= lFactor;
		}

		pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_COLOR;
	}

	if (lProperty.IsValid())
	{
		int existTextureCount = 0;
		const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

		for (int i = 0; i < lTextureCount; i++)
		{
			FbxFileTexture* lFileTexture = lProperty.GetSrcObject<FbxFileTexture>(i);
			if (!lFileTexture)
				continue;

			FbxString uvsetName = lFileTexture->UVSet.Get();
			std::string uvSetString = uvsetName.Buffer();
			std::string filepath = lFileTexture->GetFileName();

			pElement->textureSetArray[uvSetString].push_back(filepath);
			existTextureCount++;
		}

		const int lLayeredTextureCount = lProperty.GetSrcObjectCount<FbxLayeredTexture>();

		for (int i = 0; i < lLayeredTextureCount; i++)
		{
			FbxLayeredTexture* lLayeredTexture = lProperty.GetSrcObject<FbxLayeredTexture>(i);

			const int lTextureFileCount = lLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

			for (int j = 0; j < lTextureFileCount; j++)
			{
				FbxFileTexture* lFileTexture = lLayeredTexture->GetSrcObject<FbxFileTexture>(j);
				if (!lFileTexture)
					continue;

				FbxString uvsetName = lFileTexture->UVSet.Get();
				std::string uvSetString = uvsetName.Buffer();
				std::string filepath = lFileTexture->GetFileName();

				pElement->textureSetArray[uvSetString].push_back(filepath);
				existTextureCount++;
			}
		}

		if (existTextureCount > 0)
		{
			if (pElement->type == FBX_MATRIAL_ELEMENT::ELEMENT_COLOR)
				pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_BOTH;
			else
				pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_TEXTURE;
		}
	}

	return lResult;
}

void FBXLoader::SetFbxColor(FBX_MATRIAL_ELEMENT& destColor, const FbxDouble3 srcColor)
{
	destColor.a = 1.0f;
	destColor.r = static_cast<float>(srcColor[0]);
	destColor.g = static_cast<float>(srcColor[1]);
	destColor.b = static_cast<float>(srcColor[2]);
}


