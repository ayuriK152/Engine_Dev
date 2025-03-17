#include "pch.h"
#include "Shader.h"

Shader::Shader(wstring fileName, D3D_SHADER_MACRO* macros, ShaderType shaderType) : Super(ResourceType::Shader),
	_macros(macros), _shaderType(shaderType)
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
		_blob = DXUtil::CompileShader(path, _macros, "VS", "vs_5_1");
	if (_shaderType == ShaderType::PS)
		_blob = DXUtil::CompileShader(path, _macros, "PS", "ps_5_1");
	if (_shaderType == ShaderType::GS)
		_blob = DXUtil::CompileShader(path, _macros, "GS", "gs_5_1");
}
