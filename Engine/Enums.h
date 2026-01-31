#pragma once

#define COUNT_COMPONENTTYPE	10
enum class ComponentType
{
	Undefined			 = -1,
	Transform			 = 0,
	MeshRenderer		 = 1 << 0,
	SkinnedMeshRenderer	 = 1 << 1,
	Camera				 = 1 << 2,
	Rigidbody			 = 1 << 3,
	Light				 = 1 << 4,
	Animator			 = 1 << 5,
	Script				 = 1 << 6,
	ParticleEmitter		 = 1 << 7,
	CharacterController	 = 1 << 8
};