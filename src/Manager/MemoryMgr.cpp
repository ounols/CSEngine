#include<algorithm>
#include "MemoryMgr.h"

IMPLEMENT_SINGLETON(MemoryMgr);

MemoryMgr::MemoryMgr()
{
}


MemoryMgr::~MemoryMgr()
{
}


void MemoryMgr::ExterminateObjects() {

	for (auto object : m_objects) {
		if (object == nullptr)	continue;

		object->Exterminate();
		SAFE_DELETE(object);
	}
}


void MemoryMgr::ReleaseObject(SObject* object) {
	if (object == nullptr) return;

	//������ �����ϴ� ������Ʈ���� �Ǻ� �� SAFE_DELETE�� ȣ��
	auto iObj = std::find(m_objects.begin(), m_objects.end(), object);

	if(iObj != m_objects.end()) {
		m_objects.erase(iObj);
		SAFE_DELETE(object);
	}
}
