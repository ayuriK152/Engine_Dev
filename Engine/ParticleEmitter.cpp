#include "pch.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter() : Component(ComponentType::ParticleEmitter)
{
	_emitterSetting = {};
}

ParticleEmitter::~ParticleEmitter()
{

}

void ParticleEmitter::Init()
{
	int byteSize = sizeof(Particle) * _particleMount;
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&_particleBuffer)));

	PARTICLE->AddParticleEmitter(static_pointer_cast<ParticleEmitter>(shared_from_this()));
}

void ParticleEmitter::Update(ID3D12GraphicsCommandList* cmdList)
{
	if (!_isPlaying) return;

	_emitterSetting.EmitterPos = GetTransform()->GetPosition();

	cmdList->SetComputeRoot32BitConstants(ROOT_PARAM_EMITTER_CB, sizeof(EmitterSetting) / 4, &_emitterSetting, 0);

	cmdList->SetComputeRootUnorderedAccessView(ROOT_PARAM_PARTICLES_RW, _particleBuffer->GetGPUVirtualAddress());

	cmdList->Dispatch((_particleMount + 255) / 256, 1, 1);
}

void ParticleEmitter::Render(ID3D12GraphicsCommandList* cmdList, UINT renderState)
{
	if (!_isPlaying) return;

	cmdList->SetGraphicsRootUnorderedAccessView(ROOT_PARAM_PARTICLES_RW, _particleBuffer->GetGPUVirtualAddress());
	cmdList->DrawInstanced(_particleMount, 1, 0, 0);
}

void ParticleEmitter::SetParticleTexture(string textureName)
{
	SetParticleTexture(Utils::ToWString(textureName));
}

void ParticleEmitter::SetParticleTexture(wstring textureName)
{
	shared_ptr<Texture> tex = RESOURCE->Get<Texture>(textureName);
	if (tex == nullptr) {
		DEBUG->ErrorLog(Utils::ToString(textureName) + " does not exits!");
		return;
	}

	_emitterSetting.TextureIdx = tex->GetSRVHeapIndex();
}

void ParticleEmitter::SetParticleTexture(shared_ptr<Texture> texture)
{
	SetParticleTexture(texture->GetNameW());
}
