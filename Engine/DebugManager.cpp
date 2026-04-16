#include "pch.h"
#include "DebugManager.h"

DebugManager* DebugManager::s_instance = nullptr;

DebugManager::~DebugManager()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - DebugManager\n";
#endif
}

DebugManager* DebugManager::GetInstance()
{
	if (s_instance == nullptr)
		s_instance = new DebugManager();
	return s_instance;
}

Bulb::ProcessResult DebugManager::Delete()
{
	if (s_instance != nullptr) {
		delete s_instance;
		s_instance = nullptr;
		return Bulb::ProcessResult::SUCCESS;
	}
	return Bulb::ProcessResult::FAILED_INSTANCE_NOT_FOUND;
}

void DebugManager::Init()
{
	_vertexBufferView.StrideInBytes = sizeof(VertexPC);
	_indexBufferView.Format = DXGI_FORMAT_R16_UINT;

	_bufferVertexCount = DEFAULT_VERTEX_BUFFER_SIZE;
	_vertexUploadBuffer = make_unique<UploadBuffer<VertexPC>>(DEFAULT_VERTEX_BUFFER_SIZE, false);

	_vertexBufferView.BufferLocation = _vertexUploadBuffer->GetResource()->GetGPUVirtualAddress();
	_vertexBufferView.SizeInBytes = sizeof(VertexPC) * DEFAULT_VERTEX_BUFFER_SIZE;

	_bufferIndexCount = DEFAULT_INDEX_BUFFER_SIZE;
	_indexUploadBuffer = make_unique<UploadBuffer<UINT16>>(DEFAULT_INDEX_BUFFER_SIZE, false);

	_indexBufferView.BufferLocation = _indexUploadBuffer->GetResource()->GetGPUVirtualAddress();
	_indexBufferView.SizeInBytes = sizeof(UINT16) * DEFAULT_INDEX_BUFFER_SIZE;

	Initialize();
}

void DebugManager::PreUpdate()
{
	_vertices.clear();
	_indices.clear();
}

void DebugManager::Update()
{
	JPH::BodyManager::DrawSettings drawSettings;
	drawSettings.mDrawShape = true;
	drawSettings.mDrawShapeWireframe = true;
	PHYSICS->GetPhysicsSystem()->DrawBodies(drawSettings, this);

	_vertexUploadBuffer->CopyData(_vertices.data(), _vertices.size());
	_indexUploadBuffer->CopyData(_indices.data(), _indices.size());
}

// Jolt 추가 이후 로직 수정이 필요함
void DebugManager::Render(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->IASetVertexBuffers(0, 1, &_vertexBufferView);
	cmdList->IASetIndexBuffer(&_indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	cmdList->DrawIndexedInstanced(_indices.size(), 1, 0, 0, 0);
}

void DebugManager::DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor)
{
	_indices.push_back(_vertices.size());
	_indices.push_back(_vertices.size() + 1);

	_vertices.push_back(VertexPC(inFrom.GetX(), inFrom.GetY(), inFrom.GetZ(), inColor.r, inColor.g, inColor.b, inColor.a));
	_vertices.push_back(VertexPC(inTo.GetX(), inTo.GetY(), inTo.GetZ(), inColor.r, inColor.g, inColor.b, inColor.a));
}

void DebugManager::DrawLine(Bulb::Vector3 from, Bulb::Vector3 to, Bulb::Color color)
{
	_indices.push_back(_vertices.size());
	_indices.push_back(_vertices.size() + 1);

	_vertices.push_back(VertexPC(from, color));
	_vertices.push_back(VertexPC(to, color));
}

void DebugManager::DrawTriangle(RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor, ECastShadow inCastShadow)
{
	DrawLine(inV1, inV2, inColor);
	DrawLine(inV2, inV3, inColor);
	DrawLine(inV3, inV1, inColor);
}

JPH::DebugRenderer::Batch DebugManager::CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount)
{
	return Batch();
}

JPH::DebugRenderer::Batch DebugManager::CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const uint32* inIndices, int inIndexCount)
{
	return Batch();
}

// 박스만 렌더링 할 수 있도록 되있음. 개선 가능하면 개선하는 편이 좋을 듯.
void DebugManager::DrawGeometry(RMat44Arg inModelMatrix, const AABox& inWorldSpaceBounds, float inLODScaleSq, ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode /*= ECullMode::CullBackFace*/, ECastShadow inCastShadow /*= ECastShadow::On*/, EDrawMode inDrawMode /*= EDrawMode::Solid*/)
{
	if (!_isPhysicsDebugRenderEnabled) return;

	XMMATRIX world = XMLoadFloat4x4((XMFLOAT4X4*)&inModelMatrix);

	AABox localBox = inGeometry->mBounds;
	Bulb::Vector3 minP = Bulb::Vector3(localBox.mMin.GetX(), localBox.mMin.GetY(), localBox.mMin.GetZ());
	Bulb::Vector3 maxP = Bulb::Vector3(localBox.mMax.GetX(), localBox.mMax.GetY(), localBox.mMax.GetZ());

	// 로컬 박스의 8개 점 계산
	Bulb::Vector3 corners[8] = {
		Bulb::Vector3(minP.x, minP.y, minP.z), Bulb::Vector3(maxP.x, minP.y, minP.z),
		Bulb::Vector3(maxP.x, maxP.y, minP.z), Bulb::Vector3(minP.x, maxP.y, minP.z),
		Bulb::Vector3(minP.x, minP.y, maxP.z), Bulb::Vector3(maxP.x, minP.y, maxP.z),
		Bulb::Vector3(maxP.x, maxP.y, maxP.z), Bulb::Vector3(minP.x, maxP.y, maxP.z)
	};

	// 8개 점을 world 행렬로 변환하여 선 12개 긋기
	for (int i = 0; i < 24; i++)
		_indices.push_back(_boxColliderIndices[i] + _vertices.size());
	for (int i = 0; i < 8; i++)
		_vertices.push_back(VertexPC(XMVector3TransformCoord(XMLoadFloat3(&corners[i]), world), Bulb::Color(0.0f, 1.0f, 0.0f, 1.0f)));
}

void DebugManager::DrawText3D(RVec3Arg inPosition, const string_view& inString, ColorArg inColor /*= Color::sWhite*/, float inHeight /*= 0.5f*/)
{

}

void DebugManager::Log(const string& message)
{
	_debugLogs.push_back({ TIME->TotalTime(), LOG_INFO, message });
}

void DebugManager::WarnLog(const string& message)
{
	_debugLogs.push_back({ TIME->TotalTime(), LOG_WARN, message });
}

void DebugManager::ErrorLog(const string& message)
{
	_debugLogs.push_back({ TIME->TotalTime(), LOG_ERROR, message });
}

void DebugManager::ExportDebugLogs()
{
	tinyxml2::XMLDocument doc;
	XMLElement* mainElem = doc.NewElement("DebugLogs");
	doc.InsertFirstChild(mainElem);

	for (DebugLog& log : _debugLogs) {
		XMLElement* logElem = mainElem->InsertNewChildElement("Log");
		logElem->SetText(log.message.c_str());
	}

	doc.SaveFile("DebugLog.xml");
}
