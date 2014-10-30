#pragma once
#include "Singleton.h"

typedef std::map<std::string, int> UVsetID;
typedef std::map<std::string, std::vector<std::string>> TextureSet;

struct FBX_MATRIAL_ELEMENT
{
	enum MATERIAL_ELEMENT_TYPE
	{
		ELEMENT_NONE = 0,
		ELEMENT_COLOR,
		ELEMENT_TEXTURE,
		ELEMENT_BOTH,
		ELEMENT_MAX,
	};

	MATERIAL_ELEMENT_TYPE type;
	float r, g, b, a;
	TextureSet			textureSetArray;

	FBX_MATRIAL_ELEMENT() { textureSetArray.clear(); }
	~FBX_MATRIAL_ELEMENT() { Release(); }

	void Release() {
		for (TextureSet::iterator it = textureSetArray.begin(); it != textureSetArray.end(); ++it)
			it->second.clear();

		textureSetArray.clear();
	}
};

struct FBX_MATERIAL_NODE
{
	enum eMATERIAL_TYPE
	{
		MATERIAL_LAMBERT = 0,
		MATERIAL_PHONG,
	};

	eMATERIAL_TYPE type;
	FBX_MATRIAL_ELEMENT ambient;
	FBX_MATRIAL_ELEMENT diffuse;
	FBX_MATRIAL_ELEMENT emmisive;
	FBX_MATRIAL_ELEMENT specular;

	float shininess;
	float TransparencyFactor;
};

struct MESH_ELEMENTS
{
	unsigned int	numPosition;
	unsigned int	numNormal;
	unsigned int	numUVSet;
};

struct FBX_MESH_NODE
{
	std::string		name;
	std::string		parentName;		

	MESH_ELEMENTS	elements;	
	std::vector<FBX_MATERIAL_NODE> materialArray;	
	UVsetID		uvsetID;

	std::vector<unsigned int>		indexArray;			
	std::vector<FbxVector4>			positionArray;	
	std::vector<FbxVector4>			normalArray;		
	std::vector<FbxVector2>			texcoordArray;	

	float	mat4x4[16];	

	~FBX_MESH_NODE() { Release(); }

	void Release()
	{
		uvsetID.clear();
		texcoordArray.clear();
		materialArray.clear();
		indexArray.clear();
		positionArray.clear();
		normalArray.clear();
	}
};

class FBXLoader
{
public:
	FBXLoader();
	~FBXLoader();

	BOOL Init();
	BOOL LoadFBX(char* filename);

private:

	void TriangulateRecursive(FbxNode* pNode);

	void Setup();
	void SetupNode(FbxNode* pNode, std::string parentName);

	void CopyVertexData(FbxMesh* pMesh, FBX_MESH_NODE* meshNode);
	void CopyMatrialData(FbxSurfaceMaterial* mat, FBX_MATERIAL_NODE* destMat);

	void ComputeNodeMatrix(FbxNode* pNode, FBX_MESH_NODE* meshNode);

	void SetFbxColor(FBX_MATRIAL_ELEMENT& destColor, const FbxDouble3 srcColor);
	FbxDouble3 GetMaterialProperty(
		const FbxSurfaceMaterial * pMaterial,
		const char * pPropertyName,
		const char * pFactorPropertyName,
		FBX_MATRIAL_ELEMENT*			pElement);

	static void FBXMatrixToFloat16(FbxMatrix* src, float dest[16])
	{
		unsigned int nn = 0;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				dest[nn] = static_cast<float>(src->Get(i, j));
				nn++;
			}
	}

	FbxManager*		mSdkManager;
	FbxScene*		mScene;
	FbxImporter*	mImporter;

	std::vector<FBX_MESH_NODE>		mMeshNodeArray;
};

