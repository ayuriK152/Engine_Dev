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
		{
			_indexUploadBuffer->CopyData(i, _indices[i]);
		}
	}

	UINT vertexIndex = 0;
	for (auto& collider : _drawQueue)
	{
		switch (collider->GetColliderType())
		{
			case ColliderType::Box:
			{
				if (collider->GetGameObject()->GetFramesDirty() > 0)
				{
					BoundingOrientedBox box = static_pointer_cast<BoxCollider>(collider)->GetBoundingBox();

					Vector3 corners[8];
					box.GetCorners(corners);

					for (int i = 0; i < 8; i++)
						_vertices[i + vertexIndex].Position = corners[i];

					for (int i = 0; i < 8; i++)
						_vertexUploadBuffer->CopyData(i + vertexIndex, _vertices[i + vertexIndex]);

				}

				vertexIndex += 8;
				break;
			}
		}
	}
}

void DebugManager::Render()
{
	if (_bufferVertexCount == 0 || _bufferIndexCount == 0)
		return;

	auto cmdList = GRAPHIC->GetCommandList().Get();

	cmdList->IASetVertexBuffers(0, 1, &_vertexBufferView);
	cmdList->IASetIndexBuffer(&_indexBufferView);
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	cmdList->DrawIndexedInstanced(_indices.size(), 1, 0, 0, 0);
}

void DebugManager::Log(const string& message)
{
	_debugLogs.push_back({ TIME->TotalTime(), message });
}

void DebugManager::AddDebugRender(shared_ptr<Collider> collider)
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
