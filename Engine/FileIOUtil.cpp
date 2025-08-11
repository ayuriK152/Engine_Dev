#include "pch.h"
#include "FileIOUtil.h"

void FileIOUtil::Init()
{
	LoadTextures();
	LoadMaterials();
}

void FileIOUtil::XMLFromMaterial(shared_ptr<Material> material, const wstring& name)
{
	if (!filesystem::exists(RESOURCE_PATH_MATERIALW + name))
		filesystem::create_directory(RESOURCE_PATH_MATERIALW + name);
	if (filesystem::exists(RESOURCE_PATH_MATERIALW + name + L"\\" + material->GetNameW() + L".xml"))
		return;

	char* nameChar = UniversalUtils::ToChar(material->GetNameW());
	tinyxml2::XMLDocument doc;

	XMLNode* node = doc.NewElement("Material");
	doc.InsertFirstChild(node);

	XMLElement* element = doc.NewElement("Name");
	element->SetText(nameChar);
	node->InsertEndChild(element);

	element = doc.NewElement("Ambient");
	element->SetAttribute("r", material->ambient.x);
	element->SetAttribute("g", material->ambient.y);
	element->SetAttribute("b", material->ambient.z);
	element->SetAttribute("a", material->ambient.w);
	node->InsertEndChild(element);

	element = doc.NewElement("Diffuse");
	element->SetAttribute("r", material->diffuse.x);
	element->SetAttribute("g", material->diffuse.y);
	element->SetAttribute("b", material->diffuse.z);
	element->SetAttribute("a", material->diffuse.w);
	node->InsertEndChild(element);

	element = doc.NewElement("Specular");
	element->SetAttribute("r", material->specular.x);
	element->SetAttribute("g", material->specular.y);
	element->SetAttribute("b", material->specular.z);
	element->SetAttribute("a", material->specular.w);
	node->InsertEndChild(element);

	element = doc.NewElement("Emissive");
	element->SetAttribute("r", material->emissive.x);
	element->SetAttribute("g", material->emissive.y);
	element->SetAttribute("b", material->emissive.z);
	element->SetAttribute("a", material->emissive.w);
	node->InsertEndChild(element);

	element = doc.NewElement("TextureName");
	element->SetText(material->textureName.c_str());
	node->InsertEndChild(element);

	char fullpath[100] = RESOURCE_PATH_MATERIAL;
	strcat(fullpath, UniversalUtils::ToChar(name));
	strcat(fullpath, "\\");
	strcat(fullpath, nameChar);
	strcat(fullpath, ".xml");
	doc.SaveFile(fullpath);
}

void FileIOUtil::LoadTextures()
{
	filesystem::path p = RESOURCE_PATH_TEXTURE;
	filesystem::directory_iterator iter(p);

	for (auto& i = iter; i != filesystem::end(iter); i++)
	{
		istringstream ss(UniversalUtils::ToString(i->path().c_str()));
		string fileName;
		while (getline(ss, fileName, '\\'));
		ss = istringstream(fileName);
		string format;
		while (getline(ss, format, '.'));
		if (format != "dds" && format != "png")
			continue;

		auto defaultTex = make_shared<Texture>(UniversalUtils::ToWString(fileName));
		RESOURCE->Add<Texture>(UniversalUtils::ToWString(fileName.substr(0, fileName.size() - (format.size() + 1))), defaultTex);
	}
}

void FileIOUtil::LoadMaterials()
{
	tinyxml2::XMLDocument doc;
	filesystem::path p = RESOURCE_PATH_MATERIAL;
	filesystem::recursive_directory_iterator iter(p);

	for (auto& i = iter; i != filesystem::end(iter); i++)
	{
		XMLError e = doc.LoadFile(UniversalUtils::ToChar(i->path().c_str()));
		if (e != XML_SUCCESS)
			continue;

		XMLNode* node = doc.FirstChild();

		XMLElement* element = node->FirstChildElement("Name");
		string name = "";
		if (element->GetText())
			name = string(element->GetText());

		element = node->FirstChildElement("Ambient");
		Color ambient;
		ambient.x = element->FloatAttribute("r");
		ambient.y = element->FloatAttribute("g");
		ambient.z = element->FloatAttribute("b");
		ambient.w = element->FloatAttribute("a");

		element = node->FirstChildElement("Diffuse");
		Color diffuse;
		diffuse.x = element->FloatAttribute("r");
		diffuse.y = element->FloatAttribute("g");
		diffuse.z = element->FloatAttribute("b");
		diffuse.w = element->FloatAttribute("a");

		element = node->FirstChildElement("Specular");
		Color specular;
		specular.x = element->FloatAttribute("r");
		specular.y = element->FloatAttribute("g");
		specular.z = element->FloatAttribute("b");
		specular.w = element->FloatAttribute("a");

		element = node->FirstChildElement("Emissive");
		Color emissive;
		emissive.x = element->FloatAttribute("r");
		emissive.y = element->FloatAttribute("g");
		emissive.z = element->FloatAttribute("b");
		emissive.w = element->FloatAttribute("a");

		shared_ptr<Material> mat = make_shared<Material>(name);
		mat->ambient = ambient;
		mat->diffuse = diffuse;
		mat->specular = specular;
		mat->emissive = emissive;

		element = node->FirstChildElement("TextureName");
		string textureName = "";
		if (element->GetText()) {
			textureName = string(element->GetText());
			mat->SetTexture(RESOURCE->Get<Texture>(UniversalUtils::ToWString(textureName.substr(0, textureName.length() - 4))));
		}

		RESOURCE->Add<Material>(UniversalUtils::ToWString(name), mat);
	}
}

HANDLE FileIOUtil::CreateFileHandle(string fileName, string ext)
{
	string fullPath;

	if (PathFileExistsA(fileName.c_str()))
		fullPath = fileName;
	else
	{
		if (ext == "")
			fullPath = RESOURCE_PATH + fileName + BULB_EXT_UNKNOWN;
		else
			fullPath = RESOURCE_PATH + fileName + "." + ext;

		string subPath = fullPath.substr(0, fullPath.find_last_of('\\') + 1);
		if (!filesystem::exists(subPath))
			filesystem::create_directories(subPath);
	}

	if (PathFileExistsA(fullPath.c_str())) {
		return CreateFileA(
			fullPath.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
	}
	else {
		return CreateFileA(
			fullPath.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
	}
}

void FileIOUtil::WriteToFile(HANDLE fileHandle, void* data, UINT32 size)
{
	WriteFile(
		fileHandle,
		&data,
		size,
		NULL,
		NULL
	);
}

void FileIOUtil::WriteToFile(HANDLE fileHandle, const string& data)
{
	UINT32 size = (UINT32)data.size();
	WriteToFile(fileHandle, size);

	if (size == 0)
		return;

	WriteFile(
		fileHandle,
		(data.data()),
		size,
		NULL,
		NULL
	);
}

void FileIOUtil::ReadFileData(HANDLE fileHandle, string& out)
{
	UINT32 size;
	ReadFile(
		fileHandle,
		&size,
		sizeof(UINT32),
		NULL,
		NULL
	);

	char* data = new char[size + 1];
	data[size] = 0;
	ReadFile(
		fileHandle,
		data,
		size,
		NULL,
		NULL
	);

	out = data;
	delete[] data;
}

void FileIOUtil::ReadFileData(HANDLE fileHandle, void* out, UINT32 dataSize)
{
	ReadFile(
		fileHandle,
		out,
		dataSize,
		NULL,
		NULL
	);
}
