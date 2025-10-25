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

	for (auto& emitter : _particleEmitters) {
		emitter.second->Update();
	}
}

void ParticleManager::Render()
{
	for (auto& emitter : _particleEmitters) {
		emitter.second->Render();
	}
}

void ParticleManager::AddParticleEmitter(shared_ptr<ParticleEmitter> particleEmitter)
{
	_particleEmitters[particleEmitter->GetID()] = particleEmitter;
}
