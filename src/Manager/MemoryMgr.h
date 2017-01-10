#pragma once
#include "MemoryContainer.h"
#include "../Macrodef.h"

class MemoryMgr :
	public MemoryContainer
{
protected:
	MemoryMgr();
	~MemoryMgr();

public:
	SINGLETONE(MemoryMgr);

	void ExterminateObject();
};

