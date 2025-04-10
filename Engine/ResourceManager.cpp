#include "pch.h"
#include "ResourceManager.h"

void ResourceManager::CreateDefaultResources()
{
	//==========Shader==========
	D3D_SHADER_MACRO skinnedDefines[] =
	{
		"SKINNED", "1", NULL, NULL
	};

	auto stdVS = make_shared<Shader>(L"DefaultVS.hlsl", nullptr, ShaderType::VS);
	Add<Shader>(L"standardVS", stdVS);
	
	auto skinnedVS = make_shared<Shader>(L"DefaultVS.hlsl", skinnedDefines, ShaderType::VS);
	Add<Shader>(L"skinnedVS", skinnedVS);
	
	auto opaquePS = make_shared<Shader>(L"DefaultPS.hlsl", nullptr, ShaderType::PS);
	Add<Shader>(L"opaquePS", opaquePS);

	auto skyboxPS = make_shared<Shader>(L"SkyboxPS.hlsl", nullptr, ShaderType::PS);
	Add<Shader>(L"skyboxPS", skyboxPS);

	auto skyboxVS = make_shared<Shader>(L"SkyboxVS.hlsl", nullptr, ShaderType::VS);
	Add<Shader>(L"skyboxVS", skyboxVS);


	//==========Texture==========
	auto defaultTex = make_shared<Texture>(L"white1x1.dds");
	Add<Texture>(L"Tex_Default", defaultTex);

	auto skyboxTex = make_shared<Texture>(L"Skybox_Daylight.dds", TextureType::Skybox);
	Add<Texture>(L"Tex_DefaultSkybox", skyboxTex);


	//==========Material==========
	auto defaultMat = make_shared<Material>("Default", 0, -1);
	Add<Material>(L"Mat_Default", defaultMat);

	auto defaultSkyboxMat = make_shared<Material>("DefaultSkybox", L"Tex_DefaultSkybox");
	Add<Material>(L"Mat_DefaultSkybox", defaultSkyboxMat);


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
}
