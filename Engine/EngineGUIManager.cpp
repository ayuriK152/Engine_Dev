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
	_guiToggleValues[TOGGLEVALUE_GUI_RESOURCEDIR] = false;
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
		ShowResourceDirectory();
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
			ImGui::InputText("ObjName", &_selectedObj->name);
			if (ImGui::Button("Save as Prefab"))
			{
				RESOURCE->SavePrefab(_selectedObj);
			}
			if (ImGui::Button("Set Parent"))
			{
				_isParentSelectMode = !_isParentSelectMode;
				cout << _isParentSelectMode << endl;
			}
			ImGui::Separator();

			// Transform
			ShowTransform();
		
			// Other Components
			for (auto& c : _selectedObj->GetComponents())
			{
				ComponentType componentType = c.first;
				if (componentType == ComponentType::Transform)
					continue;

				switch (componentType)
				{
					case ComponentType::MeshRenderer:
						ShowMeshRenderer(static_pointer_cast<MeshRenderer>(c.second));
						break;

					case ComponentType::SkinnedMeshRenderer:
						ShowSkinnedMeshRenderer(static_pointer_cast<SkinnedMeshRenderer>(c.second));
						break;

					case ComponentType::Camera:
						ShowCamera(static_pointer_cast<Camera>(c.second));
						break;

					case ComponentType::Light:
						ShowLight(static_pointer_cast<Light>(c.second));
						break;

					case ComponentType::Animator:
						ShowAnimator(static_pointer_cast<Animator>(c.second));
						break;

					case ComponentType::Script:
						ShowScript(static_pointer_cast<Script>(c.second));
						break;

					case ComponentType::Collider:
						ShowCollider(static_pointer_cast<Collider>(c.second));
						break;

					case ComponentType::Rigidbody:
						ShowRigidbody(static_pointer_cast<Rigidbody>(c.second));
						break;
				}
			}
		}
	}
	else if (_guiToggleValues[TOGGLEVALUE_GUI_INSPECTOR])
	{
		_guiToggleValues[TOGGLEVALUE_GUI_INSPECTOR] = false;
	}
	ImGui::End();
}

void EngineGUIManager::ShowResourceDirectory()
{
	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoFocusOnAppearing;

	ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	ImVec2 windowPos = ImVec2(0, mainViewport->Size.y);
	ImVec2 windowPivot = ImVec2(0.0f, 1.0f);
	ImVec2 windowSize = ImVec2(mainViewport->Size.x, HEIGHT_GUI_RESOURCEDIR);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::SetNextWindowSize(windowSize);

	ImGui::SetNextWindowCollapsed(!_guiToggleValues[TOGGLEVALUE_GUI_RESOURCEDIR]);

	if (ImGui::Begin("Resource Directory View", nullptr, windowFlags))
	{
		_guiToggleValues[TOGGLEVALUE_GUI_RESOURCEDIR] = true;

		if (ImGui::Button("Import Asset"))
		{

		}
	}
	else if (_guiToggleValues[TOGGLEVALUE_GUI_RESOURCEDIR])
	{
		_guiToggleValues[TOGGLEVALUE_GUI_RESOURCEDIR] = false;
	}
	ImGui::End();
}

void EngineGUIManager::HierarchyObjectRecursion(shared_ptr<Transform> parent)
{
	ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_NavLeftJumpsBackHere;
	if (parent->GetChilds().size() == 0)
		tree_flags |= ImGuiTreeNodeFlags_Leaf;
	if (_selectedObj == parent->GetGameObject())
		tree_flags |= ImGuiTreeNodeFlags_Selected;

	// label값 id추가해서 중복 방지하도록 조치 필요함
	bool isNodeOpen = ImGui::TreeNodeEx(parent->GetGameObject()->name.c_str(), tree_flags);
	if (ImGui::IsItemClicked())
	{
		if (_isParentSelectMode && _selectedObj != nullptr)
		{
			_selectedObj->GetTransform()->SetParent(parent);
			_isParentSelectMode = false;
			cout << _isParentSelectMode << endl;
		}
		_selectedObj = parent->GetGameObject();
	}
	if (isNodeOpen)
	{
		for (shared_ptr<Transform> child : parent->GetChilds())
		{
			HierarchyObjectRecursion(child);
		}
		ImGui::TreePop();
	}
}

void EngineGUIManager::ShowTransform()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool isChanged = false;
		Vector3 pos = _selectedObj->GetTransform()->GetLocalPosition();
		Vector3 rot = _selectedObj->GetTransform()->GetLocalRotation();
		Vector3 scale = _selectedObj->GetTransform()->GetLocalScale();

		ImGui::SeparatorText("Position");
		ImGui::Text("X");
		ImGui::SameLine();
		if (ImGui::InputFloat("##Transform_Position_X", &pos.x))
			isChanged = true;
		ImGui::Text("Y");
		ImGui::SameLine();
		if (ImGui::InputFloat("##Transform_Position_Y", &pos.y))
			isChanged = true;
		ImGui::Text("Z");
		ImGui::SameLine();
		if (ImGui::InputFloat("##Transform_Position_Z", &pos.z))
			isChanged = true;

		if (isChanged)
		{
			_selectedObj->GetTransform()->SetLocalPosition(pos);
			auto rb = _selectedObj->GetComponent<Rigidbody>();
			if (rb != nullptr)
			{
				rb->SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
			}
		}

		isChanged = false;
		ImGui::SeparatorText("Rotation");
		ImGui::Text("X");
		ImGui::SameLine();
		if (ImGui::InputFloat("##Transform_Rotation_X", &rot.x))
			isChanged = true;
		ImGui::Text("Y");
		ImGui::SameLine();
		if (ImGui::InputFloat("##Transform_Rotation_Y", &rot.y))
			isChanged = true;
		ImGui::Text("Z");
		ImGui::SameLine();
		if (ImGui::InputFloat("##Transform_Rotation_Z", &rot.z))
			isChanged = true;

		if (isChanged)
			_selectedObj->GetTransform()->SetLocalRotation(rot);

		isChanged = false;
		ImGui::SeparatorText("Scale");
		ImGui::Text("X");
		ImGui::SameLine();
		if (ImGui::InputFloat("##Transform_Scale_X", &scale.x))
			isChanged = true;
		ImGui::Text("Y");
		ImGui::SameLine();
		if (ImGui::InputFloat("##Transform_Scale_Y", &scale.y))
			isChanged = true;
		ImGui::Text("Z");
		ImGui::SameLine();
		if (ImGui::InputFloat("##Transform_Scale_Z", &scale.z))
			isChanged = true;

		if (isChanged)
			_selectedObj->GetTransform()->SetLocalScale(scale);
	}
}

void EngineGUIManager::ShowMeshRenderer(shared_ptr<MeshRenderer> meshRenderer)
{
	if (ImGui::CollapsingHeader("MeshRenderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("Mesh");
		ImGui::Text(UniversalUtils::ToChar(meshRenderer->GetMesh()->GetNameW()));

		ImGui::SeparatorText("Material");
		ImGui::Text(UniversalUtils::ToChar(meshRenderer->GetMaterial()->GetNameW()));
	}
}

void EngineGUIManager::ShowSkinnedMeshRenderer(shared_ptr<SkinnedMeshRenderer> meshRenderer)
{
	if (ImGui::CollapsingHeader("SkinnedMeshRenderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("Mesh");
		ImGui::Text(UniversalUtils::ToChar(meshRenderer->GetMesh()->GetNameW()));

		ImGui::SeparatorText("Material");
		ImGui::Text(UniversalUtils::ToChar(meshRenderer->GetMaterial()->GetNameW()));

		ImGui::SeparatorText("RootBone");
		ImGui::Text(meshRenderer->GetRootBone()->GetGameObject()->name.c_str());
	}
}

void EngineGUIManager::ShowCamera(shared_ptr<Camera> camera)
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("Main Camera");
		ImGui::Text(camera->IsMainCamera() ? "True" : "False");
	}
}

void EngineGUIManager::ShowLight(shared_ptr<Light> light)
{
	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("Light Type");
		if (dynamic_pointer_cast<DirectionalLight>(light) != nullptr)
			ImGui::Text("Directional Light");
	}
}

void EngineGUIManager::ShowAnimator(shared_ptr<Animator> animator)
{
	if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("Current Animation");
		ImGui::Text(animator->GetCurrentAnimation() != nullptr ? animator->GetCurrentAnimation()->GetName().c_str() : "None");
		ImGui::SeparatorText("Animation Tick");
		if (animator->GetCurrentAnimation() != nullptr)
			ImGui::Text("%.1f / %.1f", animator->GetCurrentTick(), animator->GetCurrentAnimation()->GetDuration());
		else
			ImGui::Text("-- / --");
		ImGui::SeparatorText("Animation List");

		static string addAnimPath;
		ImGui::InputText("##AddAnimationName", &addAnimPath);
		if (ImGui::Button("Add Animation"))
		{
			animator->AddAnimation(RESOURCE->LoadAnimation(addAnimPath));
			addAnimPath = "";
		}

		vector<string> removeQueue;
		for (auto& a : animator->GetAnimations())
		{
			ImGui::Text(a.first.c_str());
			ImGui::SameLine();
			string buttonLabel = "-##" + a.second->GetPath();
			if (ImGui::Button(buttonLabel.c_str()))
			{
				removeQueue.push_back(a.first);
			}
		}
		if (removeQueue.size() > 0)
		{
			for (const string& name : removeQueue)
				animator->RemoveAnimation(name);
		}
	}
}

void EngineGUIManager::ShowScript(shared_ptr<Script> script)
{
	if (ImGui::CollapsingHeader("Script", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}

void EngineGUIManager::ShowCollider(shared_ptr<Collider> collider)
{
	if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SeparatorText("Collider Type");
		switch (collider->GetColliderType())
		{
			case ColliderType::Box:
				ImGui::Text("Box Collider");
				break;
			case ColliderType::Sphere:
				ImGui::Text("Sphere Collider");
				break;
			default:
				ImGui::Text("Unknown Collider Type");
				break;
		}

		ImGui::SeparatorText("Is On Collide");
		if (collider->IsOnColliding())
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "On Collide");
		else
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not On Collide");
	}
}

void EngineGUIManager::ShowRigidbody(shared_ptr<Rigidbody> rigidbody)
{
	if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Gravity", &rigidbody->isGravity);
		ImGui::InputFloat("Elastic Modulus", &rigidbody->elasticModulus, 0.0f, 0.0f, "%.2f");
		ImGui::InputFloat("Friction", &rigidbody->friction, 0.0f, 0.0f, "%.2f");
		ImGui::InputFloat("Mass", &rigidbody->mass, 0.0f, 0.0f, "%.2f");
		ImGui::InputFloat("Drag", &rigidbody->drag, 0.0f, 0.0f, "%.2f");
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

	if (INPUTM->IsKeyDown(KeyValue::F5))
	{
		_guiToggleValues[TOGGLEVALUE_GUI_RESOURCEDIR] = !_guiToggleValues[TOGGLEVALUE_GUI_RESOURCEDIR];
	}
}

