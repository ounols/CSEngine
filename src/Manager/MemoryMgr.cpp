#include<algorithm>
#include "MemoryMgr.h"

#ifdef WIN32
#include <windows.h>
#ifdef _DEBUG
#include <typeinfo>
#include <iostream>

#endif
#elif __ANDROID__
#define LOGE(...) __android_log_print(ANDROID_LOG_DEBUG,"SCEngineMomory",__VA_ARGS__)
#include <android/log.h>


#elif __linux__

#include <iostream>
#include <typeinfo>

#endif

using namespace CSE;

MemoryMgr::MemoryMgr() {
}


MemoryMgr::~MemoryMgr() {
}


void MemoryMgr::ExterminateObjects(bool killAll) {

    auto iter = m_objects.begin();
    while (iter != m_objects.end()) {
        auto object = *iter;
        if (object == nullptr) {
            ++iter;
            continue;
        }

#ifdef _DEBUG
        OutputDebugStringA("Auto Releasing Object : ");
        OutputDebugStringA(typeid(*object).name());
        OutputDebugStringA("...\n");
#elif __ANDROID__
        LOGE("Auto Releasing Object : UNKOWN...");
#elif __linux__
        std::cout << "Auto Releasing Object : " << typeid(*object).name() << "...\n";
#endif

        //���Ű� �Ұ����� ������ �������� Ȯ��
        if (object->isUndestroyable && !killAll) {
#ifdef WIN32
            OutputDebugStringA("denied.\n");
#elif __ANDROID__
            LOGE("denied.\n");
#endif
            ++iter;
            continue;
        }

        object->Exterminate();
        iter = m_objects.erase(iter);
        SAFE_DELETE(object);
#ifdef WIN32
        OutputDebugStringA("deleted.\n");
#elif __ANDROID__
        LOGE("deleted.\n");
#elif __linux__
        std::cout << "deleted.\n";
#endif
    }

    m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), nullptr), m_objects.end());
}


void MemoryMgr::ReleaseObject(SObject* object, bool isForce) {
    if (object == nullptr) return;

    //���Ű� �Ұ����� ������ �������� Ȯ��
    if (object->isUndestroyable && !isForce) {
#ifdef WIN32
        OutputDebugStringA("Releasing Object is denied.");
#elif __ANDROID__
        LOGE("Releasing Object is denied.");
#endif
        return;
    }

    //������ �����ϴ� ������Ʈ���� �Ǻ� �� SAFE_DELETE�� ȣ��
    auto iObj = std::find(m_objects.begin(), m_objects.end(), object);

    if (iObj != m_objects.end()) {
#ifdef _DEBUG
        OutputDebugStringA("Releasing Object : ");
        OutputDebugStringA(typeid(*object).name());
        OutputDebugStringA("...\n");
#elif __ANDROID__
        LOGE("Releasing Object : UNKOWN...");
#endif

        object->Exterminate();
        Remove(object);
        SAFE_DELETE(object);

#ifdef WIN32
        OutputDebugStringA("deleted\n");
#elif __ANDROID__
        LOGE("deleted\n");
#endif

    }
}

void MemoryMgr::Init() {

}