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
	float ParticleInitialVelocity = 5;
	float ParticleLifeTime = 3;
	XMFLOAT2 ParticleSize = { 0.01f, 0.01f };
	UINT TextureIdx = 0;
};

class ParticleEmitter : public Component
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();

	void Init() override;
	void Update() override;
	void Render() override;

public:
	bool IsPlaying() { return _isPlaying; }
	void SetPlay(bool value) { _isPlaying = value; }
	void SetParticleTexture(string textureName);
	void SetParticleTexture(wstring textureName);
	void SetParticleTexture(shared_ptr<Texture> texture);

private:
	ComPtr<ID3D12Resource> _particleBuffer;
	ComPtr<ID3D12Resource> _particleBufferUpload;

	EmitterInfo _info;

	UINT _particleMount = 100;
	bool _isPlaying = false;
};