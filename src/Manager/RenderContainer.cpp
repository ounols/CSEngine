#include "RenderContainer.h"
#include "../Util/GLProgramHandle.h"


RenderContainer::RenderContainer() {}


RenderContainer::~RenderContainer() {}


void RenderContainer::Register(SIRender* object) {
	GLProgramHandle* handler = object->handler;

	m_rendersLayer[handler].push_back(object);

}


void RenderContainer::Remove(SIRender* object) {
	GLProgramHandle* handler = object->handler;

	auto iObj = std::find(m_rendersLayer[handler].begin(), m_rendersLayer[handler].end(), object);

	if (iObj != m_rendersLayer[handler].end()) {
		m_rendersLayer[handler].erase(iObj);
	}

}
