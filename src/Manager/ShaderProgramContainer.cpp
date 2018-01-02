#include "ShaderProgramContainer.h"
#include "../Macrodef.h"
#include "../Util/GLProgramHandle.h"


ShaderProgramContainer::ShaderProgramContainer() {
	SetUndestroyable(true);
}


ShaderProgramContainer::~ShaderProgramContainer() {
	ShaderProgramContainer::Exterminate();
}



void ShaderProgramContainer::Exterminate() {

}
