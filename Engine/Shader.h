#pragma once

enum class ShaderType
{
	VS,		// Vertex Shader
	PS,		// Pixel Shader
	CS,		// Compute Shader
	GS		// Geometry Shader
};

class Shader : public Resource
{
	using Super = Resource;
public:
	Shader(wstring fileName, D3D_SHADER_MACRO* macros, ShaderType shaderType);
	~Shader();

	ComPtr<ID3DBlob> GetBlob();

	virtual void Load(const wstring& path) override;

private:
	D3D_SHADER_MACRO* _macros;
	ShaderType _shaderType;
	ComPtr<ID3DBlob> _blob;
};

