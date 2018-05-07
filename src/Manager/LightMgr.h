#pragma once
#include "Base/SContainer.h"
#include "../Component/LightComponent.h"
#include "../Util/GLProgramHandle.h"

class LightMgr : public SContainer<LightComponent*> {
public:
	DECLARE_SINGLETONE(LightMgr);
	~LightMgr();

	void AttachLightToShader(const GLProgramHandle* handle) const;

private:
	void AttachDirectionalLight(const GLProgramHandle* handle, const SLight* light) const;
	void AttachPositionalLight(const GLProgramHandle* handle, const SLight* light) const;


	static void SetLightMode(const GLProgramHandle* handle, const LightComponent* light);

};

