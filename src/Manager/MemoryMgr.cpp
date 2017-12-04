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

		//���Ű� �Ұ����� ������ �������� Ȯ��
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

	//���Ű� �Ұ����� ������ �������� Ȯ��
	if (object->isUndestroyable) {
		OutputDebugStringA("Releasing Object is denied.");
		return;
	}

	//������ �����ϴ� ������Ʈ���� �Ǻ� �� SAFE_DELETE�� ȣ��
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
