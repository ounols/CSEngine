#pragma once
#include "SComponent.h"
#include "../Util/Render/LightHelper.h"

class LightComponent : public SComponent {

public:
	enum LIGHT{ NONE, DIRECTIONAL, POINT, SPOT };
public:
	LightComponent();
	~LightComponent();


	void Exterminate() override;
	void Init() override;
	void Tick(float elapsedTime) override;

	void SetLightType(LIGHT type);
	void SetDirection(vec4 direction);


	SLight* GetLight() const {
		//switch (m_type) {
		//case DIRECTIONAL: {
		//	return (m_light);
		//}

		//case POINT:
		//	return (m_light);

		//case SPOT:
		//	return (m_light);
		//	
		//default:
		//	return m_light;
		//}

		return m_light;
	}


private:
	void SetLightPosition();

public:
	LIGHT m_type = DIRECTIONAL;

private:
	SLight* m_light = nullptr;

};

