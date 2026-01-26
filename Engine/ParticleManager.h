#pragma once

class ParticleManager
{
	DECLARE_SINGLE(ParticleManager);
public:
	~ParticleManager();

public:
	void Init();
	void Update(ID3D12GraphicsCommandList* cmdList);
	void Render(ID3D12GraphicsCommandList* cmdList);

public:
	void AddParticleEmitter(shared_ptr<ParticleEmitter> particleEmitter);

private:
	// map을 사용하는게 최선인지 고민
	unordered_map<int, shared_ptr<ParticleEmitter>> _particleEmitters;
};

