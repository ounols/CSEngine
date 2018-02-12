#include<algorithm>
#include "MemoryMgr.h"
#ifdef WIN32
#include <windows.h>
#ifdef _DEBUG
#include <typeinfo.h>
#endif
#elif __ANDROID__
#define LOGE(...) __android_log_print(ANDROID_LOG_DEBUG,"SCEngineMomory",__VA_ARGS__)
#include <android/log.h>
#endif



IMPLEMENT_SINGLETON(MemoryMgr);

MemoryMgr::MemoryMgr()
{
}


MemoryMgr::~MemoryMgr()
{
}


void MemoryMgr::ExterminateObjects(bool killAll) {

	int index = 0;

	for (auto object : m_objects) {
		if (object == nullptr) {
			index++;
			continue;
		}

#ifdef WIN32
		OutputDebugStringA("Auto Releasing Object : ");
		OutputDebugStringA(typeid(*object).name());
		OutputDebugStringA("...\n");
#elif __ANDROID__
		LOGE("Auto Releasing Object : UNKOWN...");
#endif

		//���Ű� �Ұ����� ������ �������� Ȯ��
		if(object->isUndestroyable && !killAll) {
#ifdef WIN32
			OutputDebugStringA("denied.\n");
#elif __ANDROID__
			LOGE("denied.\n");
#endif
			index++;
			continue;
		}

		object->Exterminate();
		SAFE_DELETE(object);
#ifdef WIN32
		OutputDebugStringA("deleted.\n");
#elif __ANDROID__
		LOGE("deleted.\n");
#endif

		m_objects.at(index) = nullptr;
		index++;
	}

	m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), nullptr), m_objects.end());
}


void MemoryMgr::ReleaseObject(SObject* object) {
	if (object == nullptr) return;

	//���Ű� �Ұ����� ������ �������� Ȯ��
	if (object->isUndestroyable) {
#ifdef WIN32
		OutputDebugStringA("Releasing Object is denied.");
#elif __ANDROID__
        LOGE("Releasing Object is denied.");
#endif
		return;
	}

	//������ �����ϴ� ������Ʈ���� �Ǻ� �� SAFE_DELETE�� ȣ��
	auto iObj = std::find(m_objects.begin(), m_objects.end(), object);

	if(iObj != m_objects.end()) {
#ifdef WIN32
		OutputDebugStringA("Releasing Object : ");
		OutputDebugStringA(typeid(*object).name());
		OutputDebugStringA("...\n");
#elif __ANDROID__
		LOGE("Releasing Object : UNKOWN...");
#endif

		m_objects.erase(iObj);
		object->Exterminate();
		SAFE_DELETE(object);

#ifdef WIN32
		OutputDebugStringA("deleted\n");
#elif __ANDROID__
		LOGE("deleted\n");
#endif

	}
}
