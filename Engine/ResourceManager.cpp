#include "pch.h"
#include "ResourceManager.h"

void ResourceManager::CreateDefaultResources()
{
	//==========Shader==========
	D3D_SHADER_MACRO skinnedDefines[] =
	{
		"SKINNED", "1", NULL, NULL
	};

	auto stdVS = make_shared<Shader>(L"Default.hlsl", nullptr, ShaderType::VS);
	Add<Shader>(L"standardVS", stdVS);
	auto skinnedVS = make_shared<Shader>(L"Default.hlsl", skinnedDefines, ShaderType::VS);
	Add<Shader>(L"skinnedVS", skinnedVS);
	auto opaquePS = make_shared<Shader>(L"Default.hlsl", nullptr, ShaderType::PS);
	Add<Shader>(L"opaquePS", opaquePS);


	//==========Texture==========
	auto tex = make_shared<Texture>(L"white1x1.dds");
	Add<Texture>(L"Tex_Default", tex);


	//==========Material==========
	auto defaultMat = make_shared<Material>("Default", 0, 0, -1);
	Add<Material>(L"Mat_Default", defaultMat);


	//==========Mesh==========
	shared_ptr<Mesh> boxMesh = make_shared<Mesh>();
	Add<Mesh>(L"Mesh_BasicBox", boxMesh);

	shared_ptr<Mesh> sphereMesh = make_shared<Mesh>();
	Add<Mesh>(L"Mesh_BasicSphere", sphereMesh);

	shared_ptr<Mesh> quadMesh = make_shared<Mesh>();
	Add<Mesh>(L"Mesh_BasicQuad", quadMesh);
}
