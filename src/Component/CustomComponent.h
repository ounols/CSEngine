#pragma once
#include "SComponent.h"
#include "sqext.h"

class CustomComponent : public SComponent {
public:
	CustomComponent();
	~CustomComponent();


	void Exterminate() override;
	void Init() override;
	void Tick(float elapsedTime) override;

	void SetClassName(std::string name);
	std::string SGetClassName() const;


	bool GetIsEnable() const override;
	void SetIsEnable(bool is_enable) override;

	void Log(const char* log);

	SGameObject* GetGameObject() const override;


	Sqrat::Object GetClassInstance() const {
		return m_classInstance->get();
	}

private:
	void RegisterScript();

private:
	mutable sqext::SQIClass* m_specialization = nullptr;
	mutable sqext::SQIClassInstance* m_classInstance = nullptr;
	int m_funcSetCSEngine = 0;
	int m_funcInit = 1;
	int m_funcTick = 2;
	int m_funcExterminate = 3;

	std::string m_className = "";

	bool m_isError = false;
};
