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
		shared_ptr<GameObject> go = GameObject::Instantiate();

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

		objElem = objElem->NextSiblingElement("GameObject");
	}

	_currentSceneName = sceneName;

	// 이 부분은 추후에 에디터에서만 적용되도록 변경해야함.
	SetWindowText(GRAPHIC->GetMainWnd(), Utils::ToWString("Bulb Engine | " + _currentSceneName).c_str());
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
	ThrowIfFailed(pFileOpen->Show(GRAPHIC->GetMainWnd()));

	ComPtr<IShellItem> pItem;
	ThrowIfFailed(pFileOpen->GetResult(&pItem));

	PWSTR pszFilePath;
	ThrowIfFailed(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath));

	wstring filePath = pszFilePath;
	CoTaskMemFree(pszFilePath);

	CoUninitialize();

	DEBUG->Log(Utils::ToString(filePath));
}

void SceneManager::SaveScene()
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	ComPtr<IFileSaveDialog> pFileSave;
	ThrowIfFailed(CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileSave)));

	COMDLG_FILTERSPEC fileTypes[] = {
		{ L"Scene Files (*.xml)", L"*.xml"},
	};
	pFileSave->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);
	pFileSave->SetDefaultExtension(L"xml");

	DWORD dwFlags;
	pFileSave->GetOptions(&dwFlags);
	pFileSave->SetOptions(dwFlags | FOS_OVERWRITEPROMPT | FOS_PATHMUSTEXIST);

	ThrowIfFailed(pFileSave->Show(GRAPHIC->GetMainWnd()));

	ComPtr<IShellItem> pItem;
	ThrowIfFailed(pFileSave->GetResult(&pItem));

	PWSTR pszFilePath;
	ThrowIfFailed(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath));

	wstring filePath = pszFilePath;
	CoTaskMemFree(pszFilePath);

	CoUninitialize();

	DEBUG->Log(Utils::ToString(filePath));
}
