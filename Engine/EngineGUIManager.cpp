#include "pch.h"
#include "EngineGUIManager.h"

#pragma region static fields

DescriptorHeapAllocator EngineGUIManager::_srvHeapDescAllocator;

#pragma endregion

EngineGUIManager::~EngineGUIManager()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void EngineGUIManager::Init()
{
	_srvHeapDescAllocator.Create(GRAPHIC->GetDevice().Get(), RENDER->GetShaderResourceViewHeap().Get());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui_ImplWin32_Init(GRAPHIC->GetMainWnd());

	ImGui_ImplDX12_InitInfo init_info = {};
	init_info.Device = GRAPHIC->GetDevice().Get();
	init_info.CommandQueue = GRAPHIC->GetCommandQueue().Get();
	init_info.NumFramesInFlight = GRAPHIC->GetNumFrameResources();
	init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	init_info.SrvDescriptorHeap = RENDER->GetShaderResourceViewHeap().Get();
	init_info.SrvDescriptorAllocFn = [](
		ImGui_ImplDX12_InitInfo*,
		D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle,
		D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
			return _srvHeapDescAllocator.Alloc(out_cpu_handle, out_gpu_handle);
		};
	init_info.SrvDescriptorFreeFn = [](
		ImGui_ImplDX12_InitInfo*,
		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle,
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
			return _srvHeapDescAllocator.Free(cpu_handle, gpu_handle);
		};

	ImGui_ImplDX12_Init(&init_info);

	_guiToggleValues[TOGGLEVALUE_GUI_DEMOWINDOW] = false;
	_guiToggleValues[TOGGLEVALUE_GUI_ENGINESTATUS] = true;
	_guiToggleValues[TOGGLEVALUE_GUI_HIERARCHY] = true;
	_guiToggleValues[TOGGLEVALUE_GUI_INSPECTOR] = true;
}

void EngineGUIManager::FixedUpdate()
{

}

void EngineGUIManager::Update()
{
	ToggleWindows();
}

void EngineGUIManager::Render()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		if (_guiToggleValues[TOGGLEVALUE_GUI_DEMOWINDOW])
			ImGui::ShowDemoWindow();
		if (_guiToggleValues[TOGGLEVALUE_GUI_ENGINESTATUS])
			ShowEngineStatus();

		ShowHierarchyView();
		ShowInspectorView();
	}

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GRAPHIC->GetCommandList().Get());
}

void EngineGUIManager::ShowEngineStatus()
{
	ImGuiWindowFlags windowFlags = 
		ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_AlwaysAutoResize | 
		ImGuiWindowFlags_NoSavedSettings | 
		ImGuiWindowFlags_NoFocusOnAppearing | 
		ImGuiWindowFlags_NoDecoration | 
		ImGuiWindowFlags_NoInputs;

	const int padding = 10;
	ImVec2 windowPos = ImVec2(0 + padding, 0 + padding);
	ImVec2 windowPivot = ImVec2(0.0f, 0.0f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
	ImGui::SetNextWindowBgAlpha(0.35f);

	if (ImGui::Begin("Engine Status", &_guiToggleValues[TOGGLEVALUE_GUI_ENGINESTATUS], windowFlags))
	{
		ImGui::Text("FPS: %s", ENGINESTAT->GetFrameCountString().c_str());
		ImGui::Text("AVG: %s", ENGINESTAT->GetAverageFrameCountString().c_str());
	}
	ImGui::End();
}

void EngineGUIManager::ShowHierarchyView()
{
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing;

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	ImVec2 windowPos = ImVec2(mainViewport->Size.x - WIDTH_GUI_INSPECTOR, 0);
	ImVec2 windowPivot = ImVec2(1.0f, 0.0f);
	ImVec2 windowSize = ImVec2(WIDTH_GUI_HIERARCHY, mainViewport->Size.y * 0.6f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::SetNextWindowSize(windowSize);

	ImGui::SetNextWindowCollapsed(!_guiToggleValues[TOGGLEVALUE_GUI_HIERARCHY]);

	if (ImGui::Begin("Hierarchy View", nullptr, windowFlags))
	{
		_guiToggleValues[TOGGLEVALUE_GUI_HIERARCHY] = true;
		for (auto& o : RENDER->GetObjects())
		{
			if (o->GetTransform()->GetParent() != nullptr)
				continue;
			HierarchyObjectRecursion(o->GetTransform());
		}
	}
	else if (_guiToggleValues[TOGGLEVALUE_GUI_HIERARCHY])
	{
		_guiToggleValues[TOGGLEVALUE_GUI_HIERARCHY] = false;
	}
	ImGui::End();
}

void EngineGUIManager::ShowInspectorView()
{
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing;

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	ImVec2 windowPos = ImVec2(mainViewport->Size.x, 0);
	ImVec2 windowPivot = ImVec2(1.0f, 0.0f);
	ImVec2 windowSize = ImVec2(WIDTH_GUI_INSPECTOR, mainViewport->Size.y * 0.6f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::SetNextWindowSize(windowSize);

	ImGui::SetNextWindowCollapsed(!_guiToggleValues[TOGGLEVALUE_GUI_INSPECTOR]);
	if (ImGui::Begin("Inspector View", nullptr, windowFlags))
	{
		_guiToggleValues[TOGGLEVALUE_GUI_INSPECTOR] = true;
		if (_selectedObj == nullptr)
		{
			ImGui::Text("No GameObject Selected");
		}
		else
		{
			ImGui::Text(_selectedObj->name.c_str());
			ImGui::Separator();

			// Transform
			ShowTransform();
		
			// Other Components
			for (auto& c : _selectedObj->GetComponents())
			{
				if (c.first == ComponentType::Transform)
					continue;

				auto componentType = magic_enum::enum_name(c.first);
				ImGui::SeparatorText(string(componentType).c_str());
			}
		}
	}
	else if (_guiToggleValues[TOGGLEVALUE_GUI_INSPECTOR])
	{
		_guiToggleValues[TOGGLEVALUE_GUI_INSPECTOR] = false;
	}
	ImGui::End();
}

void EngineGUIManager::ShowTransform()
{
	bool isChanged = false;
	Vector3 pos = _selectedObj->GetTransform()->GetLocalPosition();
	Vector3 rot = _selectedObj->GetTransform()->GetLocalRotation();
	Vector3 scale = _selectedObj->GetTransform()->GetLocalScale();
	ImGui::SeparatorText("Transform");

	ImGui::Text("Position");
	ImGui::Text("X");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Inspector_Position_X", &pos.x))
		isChanged = true;
	ImGui::Text("Y");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Inspector_Position_Y", &pos.y))
		isChanged = true;
	ImGui::Text("Z");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Inspector_Position_Z", &pos.z))
		isChanged = true;

	if (isChanged)
		_selectedObj->GetTransform()->SetLocalPosition(pos);

	isChanged = false;
	ImGui::Text("Rotation");
	ImGui::Text("X");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Inspector_Rotation_X", &rot.x))
		isChanged = true;
	ImGui::Text("Y");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Inspector_Rotation_Y", &rot.y))
		isChanged = true;
	ImGui::Text("Z");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Inspector_Rotation_Z", &rot.z))
		isChanged = true;

	if (isChanged)
		_selectedObj->GetTransform()->SetLocalRotation(rot);

	isChanged = false;
	ImGui::Text("Scale");
	ImGui::Text("X");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Inspector_Scale_X", &scale.x))
		isChanged = true;
	ImGui::Text("Y");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Inspector_Scale_Y", &scale.y))
		isChanged = true;
	ImGui::Text("Z");
	ImGui::SameLine();
	if (ImGui::InputFloat("##Inspector_Scale_Z", &scale.z))
		isChanged = true;

	if (isChanged)
		_selectedObj->GetTransform()->SetLocalScale(scale);
}

void EngineGUIManager::HierarchyObjectRecursion(shared_ptr<Transform> parent)
{
	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_NavLeftJumpsBackHere;
	if (parent->GetChilds().size() == 0)
		tree_flags |= ImGuiTreeNodeFlags_Leaf;
	if (_selectedObj == parent->GetGameObject())
		tree_flags |= ImGuiTreeNodeFlags_Selected;

	bool isNodeOpen = ImGui::TreeNodeEx(parent->GetGameObject()->name.c_str(), tree_flags);
	if (ImGui::IsItemClicked())
		_selectedObj = parent->GetGameObject();
	if (isNodeOpen)
	{
		for (shared_ptr<Transform> child : parent->GetChilds())
		{
			HierarchyObjectRecursion(child);
		}
		ImGui::TreePop();
	}
}

void EngineGUIManager::ToggleWindows()
{
	// FPS status view
	if (INPUTM->IsKeyDown(KeyValue::F2))
	{
		_guiToggleValues[TOGGLEVALUE_GUI_ENGINESTATUS] = !_guiToggleValues[TOGGLEVALUE_GUI_ENGINESTATUS];
	}

	if (INPUTM->IsKeyDown(KeyValue::F3))
	{
		_guiToggleValues[TOGGLEVALUE_GUI_HIERARCHY] = !_guiToggleValues[TOGGLEVALUE_GUI_HIERARCHY];
	}
	
	if (INPUTM->IsKeyDown(KeyValue::F4))
	{
		_guiToggleValues[TOGGLEVALUE_GUI_INSPECTOR] = !_guiToggleValues[TOGGLEVALUE_GUI_INSPECTOR];
	}
}

