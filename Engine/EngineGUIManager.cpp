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
		if (_guiToggleValues[TOGGLEVALUE_GUI_HIERARCHY])
			ShowHierarchyView();
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

	const float padding = 10.0f;
	ImVec2 windowPos = ImVec2(0.0f + padding, 0.0f + padding);
	ImVec2 windowPivot = ImVec2(0.0f, 0.0f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
	ImGui::SetNextWindowBgAlpha(0.35f);

	if (ImGui::Begin("Engine Status", &_guiToggleValues[TOGGLEVALUE_GUI_ENGINESTATUS], windowFlags))
	{
		ImGui::Text("FPS: %s", ENGINESTAT->GetFrameCountString().c_str());
	}
	ImGui::End();
}

void EngineGUIManager::ShowHierarchyView()
{
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing;

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	
	const float padding = 10.0f;
	ImVec2 windowPos = ImVec2(mainViewport->Size.x - padding, 0.0f + padding);
	ImVec2 windowPivot = ImVec2(1.0f, 0.0f);
	ImVec2 windowSize = ImVec2(400.0f, mainViewport->Size.y * 0.6f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::SetNextWindowSize(windowSize);

	if (ImGui::Begin("Hierarchy View", nullptr, windowFlags))
	{
		for (auto& o : RENDER->GetObjects())
		{
			if (o->GetTransform()->GetParent() != nullptr)
				continue;
			HierarchyObjectRecursion(o->GetTransform());
		}
	}
	ImGui::End();
}

void EngineGUIManager::HierarchyObjectRecursion(shared_ptr<Transform> parent)
{
	//ImGui::Selectable(parent->GetGameObject()->name.c_str());
	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (parent->GetChilds().size() == 0)
		tree_flags |= ImGuiTreeNodeFlags_Leaf;
	if (ImGui::TreeNodeEx(parent->GetGameObject()->name.c_str(), tree_flags))
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
}

