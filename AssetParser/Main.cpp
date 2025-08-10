#include "pch.h"
#include "Main.h"

int main()
{
	cout << "Bulb Engine Asset Parser " << CURRENT_VERSION << endl << endl;
	filesystem::path assetPath(RESOURCE_PATH_ASSET);
	filesystem::directory_iterator iter(assetPath);
	cout << "Asset list" << endl << endl;
	for (auto& i = iter; i != filesystem::end(iter); i++)
	{
		string pathStr = UniversalUtils::ToString(i->path().c_str());
		string name = pathStr.substr(pathStr.find_last_of('\\') + 1);
		if (i->is_directory() && name == "Parsed")
			continue;
		cout << name << endl;
	}
	system("pause");
	return 0;
}