#pragma once
#include "../SObject.h"
#include <string>

class SComponent
	: public SObject
{
public:

	SComponent()
	{
	}

	virtual ~SComponent()
	{
	}

	virtual void Tick(float elapsedTime) = 0;

	const char* getName() const {
		return m_name.c_str();
	}

protected:
	std::string m_name;
};

