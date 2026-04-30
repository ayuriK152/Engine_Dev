#include "pch.h"
#include "ParticleManager.h"

ParticleManager* ParticleManager::s_instance = nullptr;

ParticleManager::~ParticleManager()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - ParticleManager\n";
#endif

	for (auto pe : _particleEmitters)
		pe.second.reset();
}

ParticleManager* ParticleManager::GetInstance()
{
	if (s_instance == nullptr)
		s_instance = new ParticleManager();
	return s_instance;
}

Bulb::ProcessResult ParticleManager::Delete()
{
	if (s_instance != nullptr) {
		delete s_instance;
		s_instance = nullptr;
		return Bulb::ProcessResult::SUCCESS;
	}
	return Bulb::ProcessResult::FAILED_INSTANCE_NOT_FOUND;
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

void ParticleManager::DeleteParticleEmitter(shared_ptr<ParticleEmitter> particleEmitter)
{
	_particleEmitters.erase(particleEmitter->GetID());
}
