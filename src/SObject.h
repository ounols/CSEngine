#pragma once
//#include "Manager/MemoryMgr.h"

class SObject {
public:

	SObject();
	virtual ~SObject();

	virtual void Exterminate() = 0;

	void SetUndestroyable(bool enable);

private:
	bool isUndestroyable = false;
public:
	friend class MemoryMgr;
};
