#include "SObject.h"
#include "Manager/MemoryMgr.h"
#include "Manager/EngineCore.h"

using namespace CSE;

MemoryMgr* memoryMgr = nullptr;

SObject::SObject() {
    memoryMgr = CORE->GetCore(MemoryMgr);
	// register this object to MemoryContainer class
    memoryMgr->Register(this);
}

SObject::SObject(bool isRegister) {
    if(isRegister) memoryMgr->Register(this);
}

SObject::~SObject() {
}


void SObject::SetUndestroyable(bool enable) {
	isUndestroyable = enable;
}

void SObject::Destroy() {
    memoryMgr->ReleaseObject(this);

}

void SObject::__FORCE_DESTROY__() {
    memoryMgr->ReleaseObject(this, true);
}
