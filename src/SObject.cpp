#include "SObject.h"
#include "Manager/MemoryMgr.h"

using namespace CSE;

SObject::SObject() {
	// register this object to MemoryContainer class
	MemoryMgr::getInstance()->Register(this);
}


SObject::~SObject() {
}


void SObject::SetUndestroyable(bool enable) {
	isUndestroyable = enable;
}

void SObject::Destroy() {
	MemoryMgr::getInstance()->ReleaseObject(this);

}

void SObject::__FORCE_DESTROY__() {
	MemoryMgr::getInstance()->ReleaseObject(this, true);
}
