#pragma once

enum class ShaderType
{
	VS,
	PS
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

