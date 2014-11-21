#include "stdafx.h"
#include "ModelManager.h"
#include "Logger.h"

ModelManager::ModelManager()
{
}


ModelManager::~ModelManager()
{
}


BOOL ModelManager::Init()
{
	return TRUE;
}


FbxScene* ModelManager::MakeFbxSceneFromFile(const char* filePath)
{
	mFbxManager = FbxManager::Create();
	if (!mFbxManager)
		return false;

	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(mFbxManager, IOSROOT);
	mFbxManager->SetIOSettings(fbxIOSettings);

	mFbxScene = FbxScene::Create(mFbxManager, "myScene");

	FbxImporter* fbxImporter = FbxImporter::Create(mFbxManager, "myImporter");

	if (!fbxImporter)
		return false;

	if (!fbxImporter->Initialize(filePath, -1, mFbxManager->GetIOSettings()))
		return false;

	if (!fbxImporter->Import(mFbxScene))
		return false;

	fbxImporter->Destroy();

	FbxAxisSystem OurAxisSystem = FbxAxisSystem::OpenGL;
	//좌표계 설정
	FbxAxisSystem SceneAxisSystem = mFbxScene->GetGlobalSettings().GetAxisSystem();
	if (SceneAxisSystem != OurAxisSystem)
		FbxAxisSystem::DirectX.ConvertScene(mFbxScene);

	FbxSystemUnit SceneSystemUnit = mFbxScene->GetGlobalSettings().GetSystemUnit();
	if (SceneSystemUnit.GetScaleFactor() != 1.0)
		FbxSystemUnit::cm.ConvertScene(mFbxScene);

	return mFbxScene;
}



MeshPointer ModelManager::ProcessMesh(FbxNode* inNode)
{
	MeshPointer pNewMesh(new Mesh);
	FbxMesh* pMesh = (FbxMesh*)inNode->GetNodeAttribute();

	//fbx매쉬 생성
	if (pMesh == NULL)
	{
		Log("Mesh is null!\n");
	}
	else
	{
		// ========= Get the Vertices ==============================
		// 모든 control point를 돌면서 버텍스를 생성 / position함께 저장
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
			Log("0 vertex");

		pNewMesh->mNumPolygon = pMesh->GetPolygonCount();

		// UV Set Name을 모두 얻어온다.
		FbxStringList lUVSetNameList;
		pMesh->GetUVSetNames(lUVSetNameList);

		int numUVSet = lUVSetNameList.GetCount();

		// Polygon을 돌면서 노말과 인덱스 저장
		for (int j = 0; j < pMesh->GetPolygonCount(); j++)
		{
			int iNumVertices = pMesh->GetPolygonSize(j);

			if (iNumVertices != 3)
				Log("num Vertices wrong");

			Indices tempIndex;

			for (int k = 0; k < iNumVertices; k++)
			{
				int controlPointIndex = pMesh->GetPolygonVertex(j, k);

				FbxVector4 normal;
				pMesh->GetPolygonVertexNormal(j, k, normal);

				// ========= Get the Normals ==============================
				D3DXVECTOR3 currNormal;

				pNewMesh->mVertex[controlPointIndex].mNormal.x = (float)normal.mData[0];
				pNewMesh->mVertex[controlPointIndex].mNormal.y = (float)normal.mData[1];
				pNewMesh->mVertex[controlPointIndex].mNormal.z = (float)normal.mData[2];

				// ========= Get the Indices ==============================
				const char* lUVSetName = lUVSetNameList.GetStringAt(0);
				FbxVector2 tex;
				bool isMapped;

				pMesh->GetPolygonVertexUV(j, k, lUVSetName, tex, isMapped);
				pNewMesh->mVertex[controlPointIndex].mUV.x = static_cast<float>(tex[0]);
				pNewMesh->mVertex[controlPointIndex].mUV.y = static_cast<float>(-tex[1]);

				switch (k)
				{
				case 0:
					tempIndex.i0 = controlPointIndex;
					break;
				case 1:
					tempIndex.i1 = controlPointIndex;
					break;
				case 2:
					tempIndex.i2 = controlPointIndex;
					break;
				default:
					break;
				}
			}
			pNewMesh->mIndices.push_back(tempIndex.i0);
			pNewMesh->mIndices.push_back(tempIndex.i1);
			pNewMesh->mIndices.push_back(tempIndex.i2);
		}

		// ========= Get the Tangents =============================
		// 삼각형을 돌면서 각 꼭지점의 tangent를 계산
		int polycount = pMesh->GetPolygonCount();
		int numv = pNewMesh->mVertex.size();
		int numi = pNewMesh->mIndices.size();

		for (int polygonNum = 0; polygonNum < pMesh->GetPolygonCount(); polygonNum++)
		{
			D3DXVECTOR3 tangent;
			int ControlPointIndexStart0 = pMesh->GetPolygonVertex(polygonNum, 0);
			int ControlPointIndexStart1 = pMesh->GetPolygonVertex(polygonNum, 1);
			int ControlPointIndexStart2 = pMesh->GetPolygonVertex(polygonNum, 2);

			D3DXVECTOR3 deltaPos1 = pNewMesh->mVertex[ControlPointIndexStart1].mPos - pNewMesh->mVertex[ControlPointIndexStart0].mPos;
			D3DXVECTOR3 deltaPos2 = pNewMesh->mVertex[ControlPointIndexStart2].mPos - pNewMesh->mVertex[ControlPointIndexStart1].mPos;
			D3DXVECTOR2 deltaUV1 = pNewMesh->mVertex[ControlPointIndexStart1].mUV - pNewMesh->mVertex[ControlPointIndexStart0].mUV;
			D3DXVECTOR2 deltaUV2 = pNewMesh->mVertex[ControlPointIndexStart2].mUV - pNewMesh->mVertex[ControlPointIndexStart1].mUV;;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;

			pNewMesh->mVertex[ControlPointIndexStart0].mTangent = tangent;
			pNewMesh->mVertex[ControlPointIndexStart1].mTangent = tangent;
			pNewMesh->mVertex[ControlPointIndexStart2].mTangent = tangent;
		}
		pNewMesh->mNumIndex = pNewMesh->mIndices.size();
	}

	FbxAnimEvaluator* lEvaluator = mFbxScene->GetAnimationEvaluator();
	FbxMatrix lGlobal;
	lGlobal.SetIdentity();

	//lEvaluator->GetNodeLocalTransform();
	
	if (inNode != mFbxScene->GetRootNode())
	{
		lGlobal = lEvaluator->GetNodeGlobalTransform(inNode);
		pNewMesh->mWorld = D3DXMATRIX(	lGlobal.Get(0, 0), lGlobal.Get(0, 1), lGlobal.Get(0, 2), lGlobal.Get(0, 3),
										lGlobal.Get(1, 0), lGlobal.Get(1, 1), lGlobal.Get(1, 2), lGlobal.Get(1, 3),
										lGlobal.Get(2, 0), lGlobal.Get(2, 1), lGlobal.Get(2, 2), lGlobal.Get(2, 3),
										lGlobal.Get(3, 0), lGlobal.Get(3, 1), lGlobal.Get(3, 2), lGlobal.Get(3, 3));
	}
	else
	{
		pNewMesh->mWorld = D3DXMATRIX(	lGlobal.Get(0, 0), lGlobal.Get(0, 1), lGlobal.Get(0, 2), lGlobal.Get(0, 3),
										lGlobal.Get(1, 0), lGlobal.Get(1, 1), lGlobal.Get(1, 2), lGlobal.Get(1, 3),
										lGlobal.Get(2, 0), lGlobal.Get(2, 1), lGlobal.Get(2, 2), lGlobal.Get(2, 3),
										lGlobal.Get(3, 0), lGlobal.Get(3, 1), lGlobal.Get(3, 2), lGlobal.Get(3, 3));
	}
	
	return pNewMesh;
}

void ModelManager::CleanUp()
{
	mFbxScene->Destroy();
	mFbxManager->Destroy();
}
