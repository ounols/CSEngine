#pragma once
#include "../SObject.h"

#include <vector>
#include "Base/SContainer.h"


class GLProgramHandle;

class ShaderProgramContainer : public SObject, public SContainer<GLProgramHandle*> {

public:
	ShaderProgramContainer();
	~ShaderProgramContainer();

	void Exterminate() override;


};
