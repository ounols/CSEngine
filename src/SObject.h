#pragma once

class SObject {
public:

	SObject();
	virtual ~SObject();

	virtual void Exterminate() = 0;

	void SetUndestroyable(bool enable);
	void Destroy();
	void __FORCE_DESTROY__();

private:
	bool isUndestroyable = false;
public:
	friend class MemoryMgr;
};
