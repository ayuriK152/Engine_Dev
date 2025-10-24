#pragma once

class ParticleManager
{
	DECLARE_SINGLE(ParticleManager);
public:
	~ParticleManager();

public:
	void Init();
	void Update();
	void Render();

public:
	void AddParticleEmitter(shared_ptr<ParticleEmitter> particleEmitter);

private:
	unordered_map<int, shared_ptr<ParticleEmitter>> _particleEmitters;
};

