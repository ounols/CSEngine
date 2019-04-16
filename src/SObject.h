#pragma once

class SObject {
public:

	SObject();
	virtual ~SObject();

	virtual void Exterminate() = 0;

    virtual void SetUndestroyable(bool enable);

    virtual void Destroy();

    virtual void __FORCE_DESTROY__();

private:
	bool isUndestroyable = false;
public:
	friend class MemoryMgr;
};
