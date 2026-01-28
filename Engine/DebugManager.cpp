#include "pch.h"
#include "DebugManager.h"

DebugManager::~DebugManager()
{

}

void DebugManager::Init()
{
	_vertexBufferView.StrideInBytes = sizeof(VertexPC);
	_indexBufferView.Format = DXGI_FORMAT_R16_UINT;

	_bufferVertexCount = 100;
	_vertexUploadBuffer = make_unique<UploadBuffer<VertexPC>>(100, false);

	_vertexBufferView.BufferLocation = _vertexUploadBuffer->GetResource()->GetGPUVirtualAddress();
	_vertexBufferView.SizeInBytes = sizeof(VertexPC) * 100;

	_bufferIndexCount = 200;
	_indexUploadBuffer = make_unique<UploadBuffer<UINT16>>(200, false);

	_indexBufferView.BufferLocation = _indexUploadBuffer->GetResource()->GetGPUVirtualAddress();
	_indexBufferView.SizeInBytes = sizeof(UINT16) * 200;

	_debugLines.clear();
	Initialize();
}

void DebugManager::Update()
{
	_debugLines.clear();
	_vertices.clear();
	_indices.clear();

	JPH::BodyManager::DrawSettings drawSettings;
	//drawSettings.mDrawShape = true;
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
	_debugLines.push_back(DebugLine(
		inFrom.GetX(), 
		inFrom.GetY(),
		inFrom.GetZ(),
		inTo.GetX(),
		inTo.GetY(),
		inTo.GetZ(),
		inColor.r,
		inColor.g,
		inColor.b,
		inColor.a
	));
}

void DebugManager::DrawLine(Vector3 from, Vector3 to, ColorRGBA color)
{
	_debugLines.push_back(DebugLine(
		from, 
		to,
		color
	));
}

void DebugManager::DrawTriangle(RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor, ECastShadow inCastShadow /*= ECastShadow::Off*/)
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

void DebugManager::DrawGeometry(RMat44Arg inModelMatrix, const AABox& inWorldSpaceBounds, float inLODScaleSq, ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode /*= ECullMode::CullBackFace*/, ECastShadow inCastShadow /*= ECastShadow::On*/, EDrawMode inDrawMode /*= EDrawMode::Solid*/)
{
	XMMATRIX world = XMLoadFloat4x4((XMFLOAT4X4*)&inModelMatrix);

	AABox localBox = inGeometry->mBounds;
	Vector3 minP = Vector3(localBox.mMin.GetX(), localBox.mMin.GetY(), localBox.mMin.GetZ());
	Vector3 maxP = Vector3(localBox.mMax.GetX(), localBox.mMax.GetY(), localBox.mMax.GetZ());

	// 로컬 박스의 8개 점 계산
	Vector3 corners[8] = {
		Vector3(minP.x, minP.y, minP.z), Vector3(maxP.x, minP.y, minP.z),
		Vector3(maxP.x, maxP.y, minP.z), Vector3(minP.x, maxP.y, minP.z),
		Vector3(minP.x, minP.y, maxP.z), Vector3(maxP.x, minP.y, maxP.z),
		Vector3(maxP.x, maxP.y, maxP.z), Vector3(minP.x, maxP.y, maxP.z)
	};

	// 8개 점을 world 행렬로 변환하여 선 12개 긋기
	for (int i = 0; i < 24; i++)
		_indices.push_back(_boxColliderIndices[i] + _vertices.size());
	for (int i = 0; i < 8; i++)
		_vertices.push_back(VertexPC(XMVector3TransformCoord(XMLoadFloat3(&corners[i]), world), ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f)));
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

void DebugManager::AddDebugCollider(shared_ptr<Collider> collider)
{
	_drawQueue.push_back(collider);

	switch (collider->GetColliderType())
	{
		case ColliderType::Box:
		{
			BoundingOrientedBox box = static_pointer_cast<BoxCollider>(collider)->GetBoundingBox();

			// 정점 배열에 요소가 추가되기 전 크기를 받아와야되기 때문에 인덱스 먼저 추가
			for (int i = 0; i < 24; i++)
				_indices.push_back(_boxColliderIndices[i] + _vertices.size());

			Vector3 corners[8];
			box.GetCorners(corners);

			VertexPC v[8];
			for (int i = 0; i < 8; i++)
			{
				v[i].Position = corners[i];
				v[i].Color = _colliderDebugColor;
				_vertices.push_back(v[i]);
			}

			break;
		}
	}
}

void DebugManager::DeleteDebugCollider(shared_ptr<Collider> collider)
{
	bool isExists = false;
	for (int i = 0; i < _drawQueue.size(); i++) {
		if (_drawQueue[i] == collider) {
			_drawQueue.erase(_drawQueue.begin() + i);
			isExists = true;
			break;
		}
	}

	if (isExists) {
		switch (collider->GetColliderType()) {
		case ColliderType::Box: {
			_vertices.erase(_vertices.end() - 8, _vertices.end());
			_indices.erase(_indices.end() - 24, _indices.end());
		}
		}
	}
}
