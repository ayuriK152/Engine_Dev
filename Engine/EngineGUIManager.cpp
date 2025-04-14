#include "pch.h"
#include "EngineGUIManager.h"

#pragma region static fields

ExampleDescriptorHeapAllocator EngineGUIManager::_srvHeapDescAllocator;

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
	init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // Or your render target format.

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
}

void EngineGUIManager::FixedUpdate()
{

}

void EngineGUIManager::Update()
{

}

void EngineGUIManager::Render()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Test");
		ImGui::Text("Test Text");
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), GRAPHIC->GetCommandList().Get());
}

