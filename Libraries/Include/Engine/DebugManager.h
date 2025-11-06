#pragma once

#define	MAX_VERTEX_COUNT	100
#define MAX_INDEX_COUNT		300

enum LogLevel
{
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR
};

struct DebugLog
{
	float time;
	LogLevel logLevel;
	string message;
};

class DebugManager
{
	DECLARE_SINGLE(DebugManager);
public:
	~DebugManager();

	void Init();
	void Update();
	void Render();

public:
	void Log(const string& message);
	void WarnLog(const string& message);
	void ErrorLog(const string& message);
	void ClearLogs() { _debugLogs.clear(); }
	vector<DebugLog>& GetLogs() { return _debugLogs; }
	void AddDebugCollider(shared_ptr<Collider> collider);
	void DeleteDebugCollider(shared_ptr<Collider> collider);

private:
	vector<DebugLog> _debugLogs;

	vector<shared_ptr<Collider>> _drawQueue;

	vector<VertexPC> _vertices;
	vector<UINT16> _indices;

	UINT _bufferVertexCount = 0;
	UINT _bufferIndexCount = 0;

	unique_ptr<UploadBuffer<VertexPC>> _vertexUploadBuffer;
	unique_ptr<UploadBuffer<UINT16>> _indexUploadBuffer;

	D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW _indexBufferView;

	Color _colliderDebugColor = { 0.0f, 1.0f, 0.0f, 1.0f };

	const UINT16 _boxColliderIndices[24] = {
				0, 1,
				1, 2,
				2, 3,
				3, 0,
				0, 4,
				1, 5,
				2, 6,
				3, 7,
				4, 5,
				5, 6,
				6, 7,
				7, 4
	};
};

