#include "pch.h"
#include "DataParseManager.h"

void DataParseManager::XMLFromMaterial(shared_ptr<Material> material, const wstring& name)
{
	if (filesystem::exists(L"..\\Resources\\Materials\\" + name + L".xml"))
		return;

	char* nameChar = UniversalUtils::ToChar(name);
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

	char fullpath[100] = "../Resources/Materials/";
	strcat(fullpath, nameChar);
	strcat(fullpath, ".xml");
	doc.SaveFile(fullpath);
}
