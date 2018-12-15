#include "RenderContainer.h"
#include "../Util/GLProgramHandle.h"
#include <iostream>


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

		//빈공간은 제거
		if(m_rendersLayer[handler].empty()) {
			auto iHandler = m_rendersLayer.find(handler);
			m_rendersLayer.erase(iHandler);
		}
	}


}
