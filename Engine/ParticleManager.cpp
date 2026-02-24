#include "pch.h"
#include "ParticleManager.h"

ParticleManager::~ParticleManager()
{

}

void ParticleManager::Init()
{

}

void ParticleManager::Update(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(RENDER->GetPSO(PSO_PARTICLE_UPDATE).Get());

	for (auto& emitter : _particleEmitters) {
		emitter.second->Update(cmdList);
	}
}

void ParticleManager::Render(ID3D12GraphicsCommandList* cmdList, UINT renderState)
{
	for (auto& emitter : _particleEmitters) {
		emitter.second->Render(cmdList, renderState);
	}
}

void ParticleManager::AddParticleEmitter(shared_ptr<ParticleEmitter> particleEmitter)
{
	_particleEmitters[particleEmitter->GetID()] = particleEmitter;
}
