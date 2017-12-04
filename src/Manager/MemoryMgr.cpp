#include<algorithm>
#include "MemoryMgr.h"
#include <windows.h>

#ifdef _DEBUG
#include <typeinfo.h>
#endif

IMPLEMENT_SINGLETON(MemoryMgr);

MemoryMgr::MemoryMgr()
{
}


MemoryMgr::~MemoryMgr()
{
}


void MemoryMgr::ExterminateObjects(bool killAll) {

	for (auto object : m_objects) {
		if (object == nullptr)	continue;

		OutputDebugStringA("Auto Releasing Object : ");
		OutputDebugStringA(typeid(*object).name());
		OutputDebugStringA("...\n");

		//제거가 불가능한 조건을 가졌는지 확인
		if(object->isUndestroyable && !killAll) {
			OutputDebugStringA("denied.\n");
			continue;
		}

		object->Exterminate();
		SAFE_DELETE(object);
		OutputDebugStringA("deleted.\n");
	}
}


void MemoryMgr::ReleaseObject(SObject* object) {
	if (object == nullptr) return;

	//제거가 불가능한 조건을 가졌는지 확인
	if (object->isUndestroyable) {
		OutputDebugStringA("Releasing Object is denied.");
		return;
	}

	//실제로 존재하는 오브젝트인지 판별 후 SAFE_DELETE를 호출
	auto iObj = std::find(m_objects.begin(), m_objects.end(), object);

	if(iObj != m_objects.end()) {
		OutputDebugStringA("Releasing Object : ");
		OutputDebugStringA(typeid(*object).name());
		OutputDebugStringA("...\n");

		m_objects.erase(iObj);
		SAFE_DELETE(object);

		OutputDebugStringA("deleted\n");

	}
}
