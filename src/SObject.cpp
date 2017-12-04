#include "SObject.h"
#include "Manager/MemoryMgr.h"


SObject::SObject() {
	// register this object to MemoryContainer class
	MemoryMgr::getInstance()->RegisterSObject(this);
}


SObject::~SObject() {
}


void SObject::SetUndestroyable(bool enable) {
	isUndestroyable = enable;
}
