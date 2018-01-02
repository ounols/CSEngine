#include "RenderContainer.h"
#include "../Util/GLProgramHandle.h"


RenderContainer::RenderContainer() {}


RenderContainer::~RenderContainer() {}


void RenderContainer::Register(SIRender* object) {
	GLProgramHandle* handler = object->handler;

	m_rendersLayer[handler].push_back(object);

}
