#pragma once

#define TOGGLEVALUE_GUI_DEMOWINDOW		"GUI_DemoWindow"
#define TOGGLEVALUE_GUI_CONSOLE			"GUI_Console"
#define TOGGLEVALUE_GUI_ENGINESTATUS	"GUI_EngineStatus"
#define TOGGLEVALUE_GUI_ENGINESETTINGS	"GUI_EngineSettings"
#define TOGGLEVALUE_GUI_HIERARCHY		"GUI_Hierarchy"
#define TOGGLEVALUE_GUI_INSPECTOR		"GUI_Inspector"
#define TOGGLEVALUE_GUI_RESOURCEDIR		"GUI_ResourceDirectory"

#define WIDTH_GUI_HIERARCHY			250
#define WIDTH_GUI_INSPECTOR			300

#define HEIGHT_GUI_RESOURCEDIR		300

class Transform;
class MeshRenderer;
class SkinnedMeshRenderer;
class Camera;
class Light;
class Animator;

struct DescriptorHeapAllocator
{
	ID3D12DescriptorHeap* Heap = nullptr;
	D3D12_DESCRIPTOR_HEAP_TYPE  HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
	D3D12_CPU_DESCRIPTOR_HANDLE HeapStartCpu;
	D3D12_GPU_DESCRIPTOR_HANDLE HeapStartGpu;
	UINT                        HeapHandleIncrement;
	ImVector<int>               FreeIndices;

	void Create(ID3D12Device* device, ID3D12DescriptorHeap* heap)
	{
		IM_ASSERT(Heap == nullptr && FreeIndices.empty());
		Heap = heap;
		D3D12_DESCRIPTOR_HEAP_DESC desc = heap->GetDesc();
		HeapType = desc.Type;
		HeapStartCpu = Heap->GetCPUDescriptorHandleForHeapStart();
		HeapStartGpu = Heap->GetGPUDescriptorHandleForHeapStart();
		HeapHandleIncrement = device->GetDescriptorHandleIncrementSize(HeapType);
		FreeIndices.reserve((int)desc.NumDescriptors);
		for (int n = desc.NumDescriptors; n > 0; n--)
			FreeIndices.push_back(n - 1);
	}
	void Destroy()
	{
		Heap = nullptr;
		FreeIndices.clear();
	}
	void Alloc(D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_desc_handle)
	{
		IM_ASSERT(FreeIndices.Size > 0);
		int idx = FreeIndices.back();
		FreeIndices.pop_back();
		out_cpu_desc_handle->ptr = HeapStartCpu.ptr + (idx * HeapHandleIncrement);
		out_gpu_desc_handle->ptr = HeapStartGpu.ptr + (idx * HeapHandleIncrement);
	}
	void Free(D3D12_CPU_DESCRIPTOR_HANDLE out_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE out_gpu_desc_handle)
	{
		int cpu_idx = (int)((out_cpu_desc_handle.ptr - HeapStartCpu.ptr) / HeapHandleIncrement);
		int gpu_idx = (int)((out_gpu_desc_handle.ptr - HeapStartGpu.ptr) / HeapHandleIncrement);
		IM_ASSERT(cpu_idx == gpu_idx);
		FreeIndices.push_back(cpu_idx);
	}
};

class EngineGUIManager
{
	DECLARE_SINGLE(EngineGUIManager)
public:
	~EngineGUIManager();

	void Init();
	void FixedUpdate();
	void Update();
	void Render();

private:
	void ToggleWindows();

	void ShowConsole();
	void ShowEngineStatus();
	void ShowEngineSettings();
	void ShowHierarchyView();
	void ShowInspectorView();
	void ShowResourceDirectory();

	void HierarchyObjectRecursion(shared_ptr<Transform> parent);

#pragma region Component
	void ShowTransform();
	void ShowMeshRenderer(shared_ptr<MeshRenderer> meshRenderer);
	void ShowSkinnedMeshRenderer(shared_ptr<SkinnedMeshRenderer> meshRenderer);
	void ShowCamera(shared_ptr<Camera> camera);
	void ShowLight(shared_ptr<Light> light);
	void ShowAnimator(shared_ptr<Animator> animator);
	void ShowScript(shared_ptr<Script> script);
	void ShowCollider(shared_ptr<Collider> collider);
	void ShowRigidbody(shared_ptr<Rigidbody> rigidbody);
#pragma endregion

public:
	bool isShowDemoWindow = true;

private:
	static DescriptorHeapAllocator _srvHeapDescAllocator;

	map<string, bool> _guiToggleValues;

	shared_ptr<GameObject> _selectedObj = nullptr;
	bool _isParentSelectMode = false;
};

