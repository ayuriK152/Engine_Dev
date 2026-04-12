#include "pch.h"
#include "ParticleEmitter.h"

// REGISTER_COMPONENT(ParticleEmitter);

ParticleEmitter::ParticleEmitter() : Component(ComponentType::ParticleEmitter)
{
	_emitterSetting = {};
}

ParticleEmitter::~ParticleEmitter()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "Released - ParticleEmitter:" << _id << "\n";
#endif
}

void ParticleEmitter::Init()
{
	int byteSize = sizeof(Particle) * MAX_PARTICLE_MOUNT;
	ThrowIfFailed(GRAPHIC->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&_particleBuffer)));

	_instantiateTime = _emitterSetting.SpawnRate;

	PARTICLE->AddParticleEmitter(static_pointer_cast<ParticleEmitter>(shared_from_this()));
}

void ParticleEmitter::Update(ID3D12GraphicsCommandList* cmdList)
{
	if (!_isPlaying) return;

	_elapsedTime += TIME->DeltaTime();

	if (_elapsedTime <= _emitterSetting.ParticleLifeTime) {
		_instantiateTime += TIME->DeltaTime();
		if (_instantiateTime >= _emitterSetting.SpawnRate) {
			_instantiateTime -= _emitterSetting.SpawnRate;
			_currentParticleMount += _mountPerTick;
		}
	}

	_emitterSetting.EmitterPos = GetTransform()->GetPosition();
	_emitterSetting.CurrentParticleMount = _currentParticleMount;

	cmdList->SetComputeRoot32BitConstants(ROOT_PARAM_EMITTER_CB, sizeof(EmitterSetting) / 4, &_emitterSetting, 0);

	cmdList->SetComputeRootUnorderedAccessView(ROOT_PARAM_PARTICLES_RW, _particleBuffer->GetGPUVirtualAddress());

	cmdList->Dispatch((_currentParticleMount + 255) / 256, 1, 1);
}

void ParticleEmitter::Render(ID3D12GraphicsCommandList* cmdList, UINT renderState)
{
	if (!_isPlaying) return;

	cmdList->SetGraphicsRootUnorderedAccessView(ROOT_PARAM_PARTICLES_RW, _particleBuffer->GetGPUVirtualAddress());
	cmdList->DrawInstanced(_currentParticleMount, 1, 0, 0);
}

void ParticleEmitter::OnDestroy()
{
#ifdef PRINT_DEBUG_CONSOLE_LOG
	cout << "OnDestroy - ParticleEmitter:" << _id << "\n";
#endif
}

void ParticleEmitter::LoadXML(Bulb::XMLElement compElem)
{
	_isPlaying = compElem.BoolAttribute("IsPlaying");
	_mountPerTick = compElem.IntAttribute("MountPerTick", 5);
	_emitterSetting.SpawnRate = compElem.FloatAttribute("SpawnRate", 1.0f);
	_emitterSetting.ParticleInitialVelocity = compElem.FloatAttribute("ParticleInitVelocity", 1.0f);
	_emitterSetting.GravityFactor = compElem.FloatAttribute("GravityFactor", 1.0f);
	_emitterSetting.ParticleLifeTime = compElem.FloatAttribute("ParticleLifeTime", 1.0f);
	_emitterSetting.ParticleSize.x = compElem.FloatAttribute("ParticleSizeX", 1.0f);
	_emitterSetting.ParticleSize.y = compElem.FloatAttribute("ParticleSizeY", 1.0f);
	_particleTexture = compElem.Attribute("ParticleTexture");
	_emitterSetting.TextureIdx = RESOURCE->Get<Texture>(Utils::ToWString(_particleTexture))->GetSRVHeapIndex();
}

void ParticleEmitter::SaveXML(Bulb::XMLElement compElem)
{
	compElem.SetAttribute("ComponentType", "ParticleEmitter");

	compElem.SetAttribute("IsPlaying", _isPlaying);
	compElem.SetAttribute("MountPerTick", (int)_mountPerTick);
	compElem.SetAttribute("SpawnRate", _emitterSetting.SpawnRate);
	compElem.SetAttribute("ParticleInitVelocity", _emitterSetting.ParticleInitialVelocity);
	compElem.SetAttribute("GravityFactor", _emitterSetting.GravityFactor);
	compElem.SetAttribute("ParticleLifeTime", _emitterSetting.ParticleLifeTime);
	compElem.SetAttribute("ParticleSizeX", _emitterSetting.ParticleSize.x);
	compElem.SetAttribute("ParticleSizeY", _emitterSetting.ParticleSize.y);
	compElem.SetAttribute("ParticleTexture", _particleTexture.c_str());
}

ComponentSnapshot ParticleEmitter::CaptureSnapshot()
{
	ComponentSnapshot snapshot;

	snapshot.id = _id;
	snapshot.componentType = "ParticleEmitter";

	return snapshot;
}

void ParticleEmitter::RestoreSnapshot(ComponentSnapshot snapshot)
{

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
