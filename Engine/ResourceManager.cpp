#include "pch.h"
#include "ResourceManager.h"

ResourceManager::~ResourceManager()
{

}

void ResourceManager::CreateDefaultResources()
{
	//==========Mesh==========
	shared_ptr<Mesh> boxMesh = make_shared<Mesh>();
	boxMesh->CreateBasicCube();
	Add<Mesh>(L"Mesh_BasicBox", boxMesh);
	shared_ptr<Mesh> sphereMesh = make_shared<Mesh>();
	sphereMesh->CreateBasicSphere();
	Add<Mesh>(L"Mesh_BasicSphere", sphereMesh);


	//==========Material==========
	auto defaultMat = make_shared<Material>("default", 0, 0, -1);
	Add<Material>(L"Mat_Default", defaultMat);


	//==========Texture==========
	auto tex = make_shared<Texture>(L"white1x1.dds");
	RESOURCE->Add<Texture>(L"whiteTex", tex);
}
