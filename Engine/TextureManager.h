#pragma once

class TextureManager
{
	DECLARE_SINGLE(TextureManager);
public:
	void LoadTexture(wstring name, wstring fileDir);
	shared_ptr<Texture> GetTexture(wstring name);

private:
	unordered_map<wstring, shared_ptr<Texture>> _textures;
};

