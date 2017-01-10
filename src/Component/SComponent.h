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

	virtual void Tick() = 0;

protected:
	std::string m_name;
};

