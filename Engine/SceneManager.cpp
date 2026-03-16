#include "pch.h"
#include "SceneManager.h"

SceneManager::~SceneManager()
{
	cout << "Released - SceneManager\n";
}

void SceneManager::Init()
{
	_isInitialized = true;

	if (_queuedSceneName != "") {
		LoadScene(_queuedSceneName);
		_queuedSceneName = "";
	}
}

void SceneManager::LoadScene(string sceneName)
{
	if (!_isInitialized) {
		_queuedSceneName = sceneName;
		return;
	}

	string pathString = RESOURCE_PATH_SCENE + sceneName + ".xml";
	if (!filesystem::exists(pathString))
		return;

	tinyxml2::XMLDocument doc;
	XMLError e = doc.LoadFile(pathString.c_str());
	if (e != XML_SUCCESS)
		return;

	XMLNode* node = doc.FirstChild();

	XMLElement* gameObjectsElem = node->FirstChildElement("GameObjects");
	XMLElement* objElem = gameObjectsElem->FirstChildElement("GameObject");
	
	while (objElem) {
		shared_ptr<GameObject> go = make_shared<GameObject>();

		string name = objElem->Attribute("Name");
		go->SetName(name);

		const char* psoName = objElem->Attribute("PSO");
		if (psoName != 0) go->SetPSOName(psoName);

		XMLElement* componentsElem = objElem->FirstChildElement("Components");
		XMLElement* compElem = componentsElem->FirstChildElement("Component");
		
		while (compElem) {
			string componentType = compElem->Attribute("ComponentType");
			shared_ptr<Component> component = ComponentFactory::Create(componentType);

			go->AddComponent(component);
			component->LoadXML(compElem);

			compElem = compElem->NextSiblingElement("Component");
		}

		RENDER->AddGameObject(go);

		objElem = objElem->NextSiblingElement("GameObject");
	}
}
