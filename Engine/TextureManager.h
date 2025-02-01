#pragma once

class TextureManager
{
	DECLARE_SINGLE(TextureManager);
public:
	void LoadTexture(string name, wstring fileDir);
	shared_ptr<Texture> GetTexture(string name);

private:
	unordered_map<string, shared_ptr<Texture>> _textures;
};

