#pragma once
#include "SComponent.h"
#include "../Util/Render/RenderInterfaces.h"
#include "DrawableStaticMeshComponent.h"

class RenderComponent : public SComponent, public SIRender {
public:
	RenderComponent();
	~RenderComponent();

	void Exterminate() override;

	void Init() override;
	void Tick(float elapsedTime) override;


	void SetMatrix(mat4 camera) override;
	void Render(float elapsedTime) override;

	void SetShaderHandle(int id);
	void SetShaderHandle(GLProgramHandle* handle);


private:
	void SetVectorInfomation();

private:
	DrawableStaticMeshComponent* m_mesh;
	const vec3* m_position;
	const vec3* m_scale;
	const vec3* m_rotation;
};

