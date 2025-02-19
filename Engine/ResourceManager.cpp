#include "pch.h"
#include "ResourceManager.h"

ResourceManager::~ResourceManager()
{

}

void ResourceManager::CreateDefaultResources()
{
	//==========Shader==========
	auto stdVS = make_shared<Shader>(L"Default.hlsl", ShaderType::VS);
	Add<Shader>(L"standardVS", stdVS);
	auto opaquePS = make_shared<Shader>(L"Default.hlsl", ShaderType::PS);
	Add<Shader>(L"opaquePS", opaquePS);


	//==========Texture==========
	auto tex = make_shared<Texture>(L"white1x1.dds");
	Add<Texture>(L"Tex_Default", tex);


	//==========Mesh==========
	shared_ptr<Mesh> boxMesh = make_shared<Mesh>();
	boxMesh->CreateBasicCube();
	Add<Mesh>(L"Mesh_BasicBox", boxMesh);

	shared_ptr<Mesh> sphereMesh = make_shared<Mesh>();
	sphereMesh->CreateBasicSphere();
	Add<Mesh>(L"Mesh_BasicSphere", sphereMesh);

	shared_ptr<Mesh> quadMesh = make_shared<Mesh>();
	quadMesh->CreateBasicQuad();
	Add<Mesh>(L"Mesh_BasicQuad", quadMesh);


	//==========Material==========
	auto defaultMat = make_shared<Material>("Default", 0, 0, -1);
	Add<Material>(L"Mat_Default", defaultMat);
}
