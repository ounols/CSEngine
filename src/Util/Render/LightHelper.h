#pragma once
#include "../Vector.h"

struct BaseLight {

	vec4 ambientColor = vec4{ 0, 0, 0, 0 };
	vec4 diffuseColor = vec4{ 1, 1, 1, 1 };
	vec4 specularColor = vec4{ 1, 1, 1, 1 };


};

struct SLight : BaseLight {

	//Direction
	vec4 direction;

	//Point Light
	vec3* position;
	float radius = 1.0f;

	vec3 att = vec3{ 1.f, 0.f, 0.f };	//Kc, Kl, Kq

	//Spot Light
	float spot;

};
//struct DirectionalLight : BaseLight {
//
//	vec3 direction;
//	float pad;
//
//};
//
//struct PointLight : BaseLight {
//	
//
//	vec3* position;
//	float range;
//
//	vec4 att;
//	float pad;
//
//};
//
//
//struct SpotLight : PointLight {
//	
//	vec4 direction;
//	float spot;
//
//};