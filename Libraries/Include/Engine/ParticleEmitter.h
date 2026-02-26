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

struct EmitterSetting {
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
	void Update(ID3D12GraphicsCommandList* cmdList);	// 일반적인 Update와 다르게 CS를 위한 명령어 리스트를 필요로 함. override X
	void Render(ID3D12GraphicsCommandList* cmdList, UINT renderState) override;

	void OnDestroy() override;

public:
	bool IsPlaying() { return _isPlaying; }

	void SetPlay(bool value) { _isPlaying = value; }

	void SetParticleTexture(string textureName);
	void SetParticleTexture(wstring textureName);
	void SetParticleTexture(shared_ptr<Texture> texture);

	void SetParticleSetting(EmitterSetting setting) { _emitterSetting = setting; }

private:
	ComPtr<ID3D12Resource> _particleBuffer;
	ComPtr<ID3D12Resource> _particleBufferUpload;

	EmitterSetting _emitterSetting;

	UINT _particleMount = 100;
	bool _isPlaying = false;
};