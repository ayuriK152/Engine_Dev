#include "pch.h"
#include "ParticleManager.h"

ParticleManager::~ParticleManager()
{

}

void ParticleManager::Init()
{

}

void ParticleManager::Update()
{
	auto cmdList = GRAPHIC->GetCommandList();
	cmdList->SetPipelineState(RENDER->GetPSO(PSO_PARTICLE_UPDATE).Get());
	cmdList->SetComputeRootSignature(RENDER->GetRootSignature().Get());

	for (auto& emitter : _particleEmitters) {
		emitter.second->Update();
	}
}

void ParticleManager::Render()
{

}

void ParticleManager::AddParticleEmitter(shared_ptr<ParticleEmitter> particleEmitter)
{
	_particleEmitters[particleEmitter->GetID()] = particleEmitter;
}
