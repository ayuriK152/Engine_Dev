#include "pch.h"
#include "SceneManager.h"

SceneManager::~SceneManager()
{
	cout << "Released - SceneManager\n";
}

void SceneManager::Init()
{
	_isInitialized = true;

	if (_queuedScenePath != "") {
		LoadScene(_queuedScenePath);
		_queuedScenePath = "";
	}
}

void SceneManager::LoadScene()
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	ComPtr<IFileOpenDialog> pFileOpen;
	ThrowIfFailed(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen)));

	COMDLG_FILTERSPEC fileTypes[] = {
		{ L"Scene Files (*.xml)", L"*.xml"},
		{ L"All Files (*.*)", L"*.*" }
	};
	pFileOpen->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);
	HRESULT hr = pFileOpen->Show(GRAPHIC->GetMainWnd());

	if (SUCCEEDED(hr)) {
		ComPtr<IShellItem> pItem;
		ThrowIfFailed(pFileOpen->GetResult(&pItem));

		PWSTR pszFilePath;
		ThrowIfFailed(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath));

		wstring filePath = pszFilePath;
		CoTaskMemFree(pszFilePath);

		string relPath = filesystem::relative(filePath, filesystem::path(RESOURCE_PATH_SCENE)).string();
		if (relPath.substr(0, 2) == "..") {
			DEBUG->ErrorLog("Failed make relative path of target scene! Target scene should located in Resources/Scenes folder!");
		}
		else {
			LoadScene(relPath);
		}
	}

	CoUninitialize();
}

void SceneManager::LoadScene(string sceneName, bool isFullPath)
{
	if (!_isInitialized) {
		_queuedScenePath = sceneName;
		return;
	}

	string path = isFullPath ? sceneName : RESOURCE_PATH_SCENE + sceneName;

	if (!filesystem::exists(path))
		return;

	if (_currentScenePath != "") {
		InitializeScene();
	}

	_currentSceneName = Utils::GetFileName(sceneName);
	_currentScenePath = path;

	tinyxml2::XMLDocument doc;
	XMLError e = doc.LoadFile(_currentScenePath.c_str());
	if (e != XML_SUCCESS)
		return;

	XMLNode* node = doc.FirstChild();

	XMLElement* objsElem = node->FirstChildElement("GameObjects");
	
	ReadGameObjectData(objsElem, nullptr);

	// 이 부분은 추후에 에디터에서만 적용되도록 변경해야함.
	SetWindowText(GRAPHIC->GetMainWnd(), Utils::ToWString("Bulb Engine | " + _currentSceneName).c_str());
}

void SceneManager::SaveScene(bool saveAs)
{
	if (!saveAs && !_currentScenePath.empty() && filesystem::exists(_currentScenePath)) {
		filesystem::path p(_currentScenePath);
		SaveScene(_currentScenePath, p.is_relative());	// 로드된 씬들은 이론적으로 다 상대경로긴 한데 혹시 몰라서 검증부분 넣어둠
		return;
	}

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	ComPtr<IFileSaveDialog> pFileSave;
	ThrowIfFailed(CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileSave)));

	// 탐색기 열 때 보이는 기본 폴더 설정하는 부분 구현해야함.
	//pFileSave->SetFolder();
	pFileSave->SetFileName(Utils::ToWString(_currentSceneName).c_str());
	pFileSave->SetDefaultExtension(L"xml");

	COMDLG_FILTERSPEC fileTypes[] = {
		{ L"Scene Files (*.xml)", L"*.xml"},
	};
	pFileSave->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);

	DWORD dwFlags;
	pFileSave->GetOptions(&dwFlags);
	pFileSave->SetOptions(dwFlags | FOS_OVERWRITEPROMPT | FOS_PATHMUSTEXIST);

	if (SUCCEEDED(pFileSave->Show(GRAPHIC->GetMainWnd()))) {
		ComPtr<IShellItem> pItem;
		ThrowIfFailed(pFileSave->GetResult(&pItem));

		PWSTR pszFilePath;
		ThrowIfFailed(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath));

		wstring filePath = pszFilePath;
		CoTaskMemFree(pszFilePath);

		string relPath = filesystem::relative(filePath, filesystem::path(RESOURCE_PATH_SCENE)).string();
		if (relPath.substr(0, 2) == "..") {
			DEBUG->ErrorLog("Failed make relative path of target scene! Target directory should located in Resources/Scenes folder!");
		}
		else {
			_currentSceneName = Utils::GetFileName(relPath);
			SaveScene(relPath);
		}
	}

	CoUninitialize();
}

void SceneManager::SaveScene(string scenePath, bool isFullPath)
{
	string path = isFullPath ? scenePath : RESOURCE_PATH_SCENE + scenePath;

	tinyxml2::XMLDocument doc;

	XMLElement* sceneElem = doc.NewElement("Scene");
	sceneElem->SetAttribute("Name", _currentSceneName.c_str());
	doc.InsertFirstChild(sceneElem);

	XMLElement* objsElem = sceneElem->InsertNewChildElement("GameObjects");
	auto& gameObjects = RENDER->GetObjects();
	for (auto& go : gameObjects) {
		if (go->GetTransform()->GetDepthLevel() == 0)
			WriteGameObjectData(objsElem, go);
	}

	doc.SaveFile(path.c_str());
}

void SceneManager::InitializeScene()
{
	RENDER->InitializeOnRuntime();
	ANIMATION->InitializeOnRuntime();
}

void SceneManager::ReadGameObjectData(XMLElement* objsElem, shared_ptr<GameObject> parent)
{
	XMLElement* objElem = objsElem->FirstChildElement("GameObject");

	while (objElem) {
		shared_ptr<GameObject> go;
		const char* prefabPath = objElem->Attribute("PrefabPath");

		if (prefabPath != 0) {
			go = RESOURCE->LoadPrefabObject(prefabPath)[0];
		}
		else {
			go = GameObject::Instantiate();

			string name = objElem->Attribute("Name");
			go->SetName(name);

			const char* psoName = objElem->Attribute("PSO");
			if (psoName != 0) go->SetPSOName(psoName);
		}

		// Component
		XMLElement* componentsElem = objElem->FirstChildElement("Components");
		if (componentsElem != nullptr) {
			XMLElement* compElem = componentsElem->FirstChildElement("Component");

			while (compElem) {
				string componentType = compElem->Attribute("ComponentType");
				shared_ptr<Component> component = ComponentFactory::Create(componentType);

				go->AddComponent(component);
				component->LoadXML(compElem);

				compElem = compElem->NextSiblingElement("Component");
			}
		}

		// Child Objects
		XMLElement* childsElem = objElem->FirstChildElement("GameObjects");
		if (childsElem != nullptr) {
			ReadGameObjectData(childsElem, go);
		}

		if (parent != nullptr) {
			go->GetTransform()->SetParent(parent->GetTransform());
		}

		objElem = objElem->NextSiblingElement("GameObject");
	}
}

void SceneManager::WriteGameObjectData(XMLElement* objsElem, shared_ptr<GameObject> go)
{
	XMLElement* objElem = objsElem->InsertNewChildElement("GameObject");
	objElem->SetAttribute("Name", go->GetName().c_str());
	objElem->SetAttribute("PSO", go->GetPSOName().c_str());

	XMLElement* compsElem = objElem->InsertNewChildElement("Components");
	auto& componentsArr = go->GetAllComponents();
	for (auto& componentsVec : componentsArr) {
		for (auto& component : componentsVec) {
			XMLElement* compElem = compsElem->InsertNewChildElement("Component");
			component->SaveXML(compElem);
		}
	}

	shared_ptr<Transform> transform = go->GetTransform();
	auto& childs = transform->GetChilds();
	if (childs.size() > 0) {
		XMLElement* childsElem = objElem->InsertNewChildElement("GameObjects");
		for (int i = 0; i < childs.size(); ++i) {
			WriteGameObjectData(childsElem, childs[i]->GetGameObject());
		}
	}

	objsElem->InsertEndChild(objElem);
}
