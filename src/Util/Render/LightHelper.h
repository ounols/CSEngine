#pragma once
#include "../Vector.h"

namespace CSE {

    struct BaseLight {

        vec3 color = vec3{ 2, 2, 2 };


    };

    struct SLight : BaseLight {

        //Direction
        vec4 direction;

        //Point Light
        vec3* position;
        float radius = 1.0f;

        vec3 att = vec3{ 1.f, 0.f, 0.f };    //Kc, Kl, Kq

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
}