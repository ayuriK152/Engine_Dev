#include "pch.h"
#include "DataParseManager.h"

void DataParseManager::XMLFromMaterial(Material* material, const wstring& name)
{
	tinyxml2::XMLDocument doc;

	XMLNode* node = doc.NewElement("Material");
	doc.InsertFirstChild(node);

}
