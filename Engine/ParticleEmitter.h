#pragma once

struct Particle {
	XMFLOAT3 Position;
	XMFLOAT3 Velocity;
	float Age;
	float Lifetime;
	UINT TextureIndex;
	XMFLOAT2 Size;
	XMFLOAT4 Color;
};

struct EmitterInfo {
	XMFLOAT3 EmitterPos;
	float SpawnRate;
	XMFLOAT3 ParticleInitialVelocity;
	float ParticleLifeTime = 1;
	XMFLOAT2 ParticleSize = { 1.0f, 1.0f };
	float DeltaTime = 0;
	float Time = 0;
	UINT TextureIdx = 0;
};

class ParticleEmitter : public Component
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();

	void Init() override;
	void Update() override;

public:
	bool IsPlaying() { return _isPlaying; }
	void SetPlay(bool value) { _isPlaying = value; }

private:
	ComPtr<ID3D12Resource> _particleBuffer;
	ComPtr<ID3D12Resource> _particleBufferUpload;

	EmitterInfo _info;

	UINT _maxParticleCount = 1000;
	bool _isPlaying = false;
};