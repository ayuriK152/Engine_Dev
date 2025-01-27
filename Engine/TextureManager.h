#pragma once

#pragma region ���漱��
struct Texture;
#pragma endregion

class TextureManager
{
	DECLARE_SINGLE(TextureManager);
public:
	void LoadTexture(string name, wstring fileDir);
	Texture* GetTexture(string name);
private:
	unordered_map<string, unique_ptr<Texture>> _textures;
};

