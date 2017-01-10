#include "SObject.h"
#include "Manager/MemoryMgr.h"


SObject::SObject() {
	MemoryMgr::getInstance()->RegisterSObject(this);
}


SObject::~SObject() {
}
