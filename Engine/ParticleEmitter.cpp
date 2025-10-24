#include "pch.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter() : Component(ComponentType::ParticleEmitter)
{
	_info = {};
}

ParticleEmitter::~ParticleEmitter()
{

}

void ParticleEmitter::Init()
{
	int byteSize = sizeof(Particle) * _maxParticleCount;
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&_particleBuffer)));

	PARTICLE->AddParticleEmitter(static_pointer_cast<ParticleEmitter>(shared_from_this()));
}

void ParticleEmitter::Update()
{
	if (!_isPlaying) return;

	_info.EmitterPos = GetTransform()->GetPosition();
	_info.DeltaTime = TIME->DeltaTime();
	_info.Time += TIME->DeltaTime();

	auto cmdList = GRAPHIC->GetCommandList();
	cmdList->SetComputeRoot32BitConstants(ROOT_PARAM_EMITTER_CB, sizeof(EmitterInfo) / 4, &_info, 0);

	cmdList->SetComputeRootUnorderedAccessView(ROOT_PARAM_PARTICLES_RW, _particleBuffer->GetGPUVirtualAddress());

	cmdList->Dispatch((_maxParticleCount + 255) / 256, 1, 1);
}
