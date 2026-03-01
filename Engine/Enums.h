#pragma once

#define COUNT_COMPONENTTYPE	10
enum class ComponentType
{
	Undefined			 = 0,
	Transform			 = 1 << 0,
	MeshRenderer		 = 1 << 1,
	SkinnedMeshRenderer	 = 1 << 2,
	Camera				 = 1 << 3,
	Rigidbody			 = 1 << 4,
	Light				 = 1 << 5,
	Animator			 = 1 << 6,
	Script				 = 1 << 7,
	ParticleEmitter		 = 1 << 8,
	CharacterController	 = 1 << 9
};

enum class UIType
{
	Frame = 1 << 0,
	Panel = 1 << 1,
	Button = 1 << 2,
	Slider = 1 << 3,
	Text = 1 << 4
};