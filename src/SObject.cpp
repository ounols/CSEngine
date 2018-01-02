#include "SObject.h"
#include "Manager/MemoryMgr.h"


SObject::SObject() {
	// register this object to MemoryContainer class
	MemoryMgr::getInstance()->Register(this);
}


SObject::~SObject() {
}


void SObject::SetUndestroyable(bool enable) {
	isUndestroyable = enable;
}
