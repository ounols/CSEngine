#pragma once
#include "../SObject.h"
#include <string>

class SComponent : public SObject {
public:

	SComponent() {
	}


	virtual ~SComponent() {
	}


	virtual void Tick(float elapsedTime) = 0;


	const char* GetName() const {
		return m_name.c_str();
	}

	void SetName(std::string name) {
		m_name = name;
	}


protected:
	std::string m_name;
};
