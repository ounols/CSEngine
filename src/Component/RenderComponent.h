#pragma once
#include "SComponent.h"
#include "../Util/Render/RenderInterfaces.h"
#include "DrawableStaticMeshComponent.h"
#include "MaterialComponent.h"

class RenderComponent : public SComponent, public SIRender {
public:
	RenderComponent();
	~RenderComponent();

	void Exterminate() override;

	void Init() override;
	void Tick(float elapsedTime) override;

	SComponent* Clone(SGameObject* object) override;


	void SetMatrix(mat4 camera, mat4 projection) override;
	void Render(float elapsedTime) override;

	void SetShaderHandle(int id);
	void SetShaderHandle(GLProgramHandle* handle);


	void SetIsEnable(bool is_enable) override;

private:
	void SetVectorInfomation();
	void SetMaterials() const;

private:
	DrawableStaticMeshComponent* m_mesh;
	MaterialComponent* m_material;
	const vec3* m_position;
	const vec3* m_scale;
	const Quaternion* m_rotation;


};

