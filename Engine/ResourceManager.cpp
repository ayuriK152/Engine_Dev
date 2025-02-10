#include "pch.h"
#include "ResourceManager.h"

ResourceManager::~ResourceManager()
{

}

void ResourceManager::CreateDefaultResources()
{
	//==========Texture==========
	auto tex = make_shared<Texture>(L"white1x1.dds");
	RESOURCE->Add<Texture>(L"Tex_Default", tex);



	//==========Mesh==========
	shared_ptr<Mesh> boxMesh = make_shared<Mesh>();
	boxMesh->CreateBasicCube();
	Add<Mesh>(L"Mesh_BasicBox", boxMesh);
	shared_ptr<Mesh> sphereMesh = make_shared<Mesh>();
	sphereMesh->CreateBasicSphere();
	Add<Mesh>(L"Mesh_BasicSphere", sphereMesh);


	//==========Material==========
	auto defaultMat = make_shared<Material>("Default", 0, 0, -1);
	Add<Material>(L"Mat_Default", defaultMat);
}
