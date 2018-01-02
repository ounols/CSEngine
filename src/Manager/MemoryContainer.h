#pragma once
#include "../SObject.h"
#include "Base/SContainer.h"

class SObject;
class MemoryContainer : public SContainer<SObject*> {

protected:
	MemoryContainer();
	~MemoryContainer();

};

