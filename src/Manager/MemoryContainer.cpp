#include "MemoryContainer.h"


MemoryContainer::MemoryContainer()
{
}


MemoryContainer::~MemoryContainer()
{
}


void MemoryContainer::RegisterSObject(SObject* object) {

	m_objects.push_back(object);

}