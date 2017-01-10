#include "MemoryMgr.h"


MemoryMgr::MemoryMgr()
{
}


MemoryMgr::~MemoryMgr()
{
}


void MemoryMgr::ExterminateObject() {
	for (const auto& object : m_objects) {
		if (object == nullptr)	continue;

		object->Exterminate();
		delete object;
	}
}