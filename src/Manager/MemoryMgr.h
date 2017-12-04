#pragma once
#include "MemoryContainer.h"
#include "../Macrodef.h"

class MemoryMgr : public MemoryContainer
{
protected:
	~MemoryMgr();
	DECLARE_SINGLETONE(MemoryMgr);

public:
	
	void ExterminateObjects(bool killAll = false);
	void ReleaseObject(SObject* object);
};