#include "pch.h"
#include "Shader.h"

Shader::Shader(wstring fileName, ShaderType shaderType) : Super(ResourceType::Shader),
	_shaderType(shaderType)
{
	Load(L"Shaders\\" + fileName);
}

Shader::~Shader()
{

}

ComPtr<ID3DBlob> Shader::GetBlob()
{
	return _blob;
}

void Shader::Load(const wstring& path)
{
	if (_shaderType == ShaderType::VS)
		_blob = DXUtil::CompileShader(path, nullptr, "VS", "vs_5_1");
	if (_shaderType == ShaderType::PS)
		_blob = DXUtil::CompileShader(path, nullptr, "PS", "ps_5_1");
}
