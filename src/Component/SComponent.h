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



protected:
};
