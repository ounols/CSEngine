#pragma once
#include "SComponent.h"
#include "SqPlus.h"

class CustomComponent : public SComponent {
public:
	CustomComponent();
	~CustomComponent();


	void Exterminate() override;
	void Init() override;
	void Tick(float elapsedTime) override;

	void SetClassName(std::string name);


	bool GetIsEnable() const override;
	void SetIsEnable(bool is_enable) override;


	SGameObject* GetGameObject() const override {
		return gameObject;
	}

private:
	void RegisterScript() const;

private:
	mutable SquirrelObject m_specialization;
	mutable SquirrelObject m_classInstance;
	SqPlus::SquirrelFunction<void> m_funcTick;
	SqPlus::SquirrelFunction<void> m_funcExterminate;

	std::string m_className = "";
};
