#pragma once

enum class ShaderType
{
	VS,		// Vertex Shader
	PS,		// Pixel Shader
	GS		// Geometry Shader
};

class Shader : public Resource
{
	using Super = Resource;
public:
	Shader(wstring fileName, ShaderType shaderType);
	~Shader();

	ComPtr<ID3DBlob> GetBlob();

	virtual void Load(const wstring& path) override;

private:
	ShaderType _shaderType;
	ComPtr<ID3DBlob> _blob;
};

