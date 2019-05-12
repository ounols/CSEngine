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


	void SetMatrix(mat4 camera, vec3 cameraPosition, mat4 projection) override;
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

	bool m_isSkinned = false;


    void SetJointMatrix();
};

