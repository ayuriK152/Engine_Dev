#include "pch.h"
#include "DebugManager.h"

DebugManager::~DebugManager()
{

}

void DebugManager::Init()
{
	_vertexBufferView.StrideInBytes = sizeof(VertexPC);
	_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
}

void DebugManager::Update()
{
	if (_vertices.size() != _bufferVertexCount)
	{
		_bufferVertexCount = _vertices.size();
		_vertexUploadBuffer = make_unique<UploadBuffer<VertexPC>>(_bufferVertexCount, false);

		_vertexBufferView.BufferLocation = _vertexUploadBuffer->GetResource()->GetGPUVirtualAddress();
		_vertexBufferView.SizeInBytes = sizeof(VertexPC) * _bufferVertexCount;

		for (int i = 0; i < _bufferVertexCount; i++)
			_vertexUploadBuffer->CopyData(i, _vertices[i]);
	}

	if (_indices.size() != _bufferIndexCount)
	{
		_bufferIndexCount = _indices.size();
		_indexUploadBuffer = make_unique<UploadBuffer<UINT16>>(_bufferIndexCount, false);

		_indexBufferView.BufferLocation = _indexUploadBuffer->GetResource()->GetGPUVirtualAddress();
		_indexBufferView.SizeInBytes = sizeof(UINT16) * _bufferIndexCount;

		for (int i = 0; i < _bufferIndexCount; i++)
			_indexUploadBuffer->CopyData(i, _indices[i]);
	}

	UINT vertexIndex = 0;
	UINT indexIndex = 0;	// 이거 이름 왜 이따구지
	for (int i = 0; i < _drawQueue.size(); i++)
	{
		switch (_drawQueue[i]->GetColliderType())
		{
			case ColliderType::Box:
			{
				// 게임 오브젝트 런타임 중 삭제된 경우
				if (_drawQueue[i]->GetGameObject() == nullptr) {
					DeleteDebugCollider(_drawQueue[i]);
					i--;
					break;
				}

				if (_drawQueue[i]->GetGameObject()->GetFramesDirty() > 0)
				{
					BoundingOrientedBox box = static_pointer_cast<BoxCollider>(_drawQueue[i])->GetBoundingBox();

					Vector3 corners[8];
					box.GetCorners(corners);

					for (int i = 0; i < 8; i++)
						_vertices[i + vertexIndex].Position = corners[i];
					for (int i = 0; i < 8; i++)
						_vertexUploadBuffer->CopyData(i + vertexIndex, _vertices[i + vertexIndex]);

					for (int i = 0; i < 8; i++)
						_indices[i + indexIndex] = _boxColliderIndices[i] + indexIndex;
					for (int i = 0; i < _bufferIndexCount; i++)
						_indexUploadBuffer->CopyData(i, _indices[i]);

				}

				vertexIndex += 8;
				indexIndex += 24;
				break;
			}
		}
	}
}

// Jolt 추가 이후 로직 수정이 필요함
void DebugManager::Render()
{
	//if (_bufferVertexCount == 0 || _bufferIndexCount == 0)
	//	return;

	//auto cmdList = GRAPHIC->GetCommandList();

	//cmdList->IASetVertexBuffers(0, 1, &_vertexBufferView);
	//cmdList->IASetIndexBuffer(&_indexBufferView);
	//cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	//cmdList->DrawIndexedInstanced(_indices.size(), 1, 0, 0, 0);
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
