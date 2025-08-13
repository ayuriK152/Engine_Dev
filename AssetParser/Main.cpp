#include "pch.h"

#define CURRENT_VERSION "v0.1.0"


int assetCount = 0;
vector<wstring> parsableAssetPaths;
vector<string> parsableAssetNames;

void DirectoryIterator(filesystem::path path, int count);

int main()
{
	cout << "Bulb Engine Asset Parser " << CURRENT_VERSION << endl << endl;
	cout << "Asset list" << endl << endl;
	filesystem::path resourcePath(RESOURCE_PATH_ASSET);
	DirectoryIterator(resourcePath, 0);
	cout << endl << "Found Parsable Assets: " << assetCount << endl << endl;

	// 여기서부터 파싱 부분
	string parsedAssetPathStr = RESOURCE_PATH_ASSET;
	parsedAssetPathStr += "Parsed\\";
	if (!filesystem::exists(parsedAssetPathStr))
		filesystem::create_directory(parsedAssetPathStr);
	for (int i = 0; i < parsableAssetPaths.size(); i++)
	{
		shared_ptr<AssetLoader> loader = make_shared<AssetLoader>();
		wstring assetPathStr = parsableAssetPaths[i];
		cout << "Parsing: " << UniversalUtils::ToString(assetPathStr.c_str()) << endl;
		loader->ImportAssetFile(assetPathStr);
		filesystem::path assetPath(assetPathStr);
		filesystem::path parsedAssetPath(parsedAssetPathStr + parsableAssetNames[i]);
		filesystem::copy(assetPath, parsedAssetPath);
		filesystem::remove(assetPath);
	}

	cout << endl << assetCount << " assets parsed." << endl;

	system("pause");
	return 0;
}

void DirectoryIterator(filesystem::path path, int count)
{
	filesystem::directory_iterator iter(path);
	for (auto& i = iter; i != filesystem::end(iter); i++)
	{
		string pathStr = UniversalUtils::ToString(i->path().c_str());
		string name = pathStr.substr(pathStr.find_last_of('\\') + 1);
		for (int j = 0; j < count; j++)
			cout << "-";
		if (i->is_directory())
		{
			if (name == "Parsed")
				continue;
			cout << name << endl;
			DirectoryIterator(i->path(), count + 1);
		}
		else
		{
			cout << name << endl;
			if (name.find(".fbx") == string::npos && name.find(".FBX") == string::npos)
				continue;
			assetCount++;
			parsableAssetPaths.push_back(i->path().c_str());
			parsableAssetNames.push_back(name);
		}
	}
}
