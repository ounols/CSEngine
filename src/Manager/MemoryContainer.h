#pragma once
#include <vector>
#include "../SObject.h"

class SObject;
class MemoryContainer
{

protected:
	MemoryContainer();
	~MemoryContainer();

public:
	void RegisterSObject(SObject* object);

protected:
	std::vector<SObject*> m_objects;
};

