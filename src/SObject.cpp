#include "SObject.h"
#include "Manager/MemoryMgr.h"
#include "Manager/EngineCore.h"
#include <random>
#include <string>

using namespace CSE;

MemoryMgr* memoryMgr = nullptr;

SObject::SObject() {
    memoryMgr = CORE->GetCore(MemoryMgr);
    GenerateHashString();
	// register this object to MemoryContainer class
    memoryMgr->Register(this);
}

SObject::SObject(bool isRegister) {
    GenerateHashString();
    if(isRegister) memoryMgr->Register(this);
}

SObject::~SObject() = default;


void SObject::SetUndestroyable(bool enable) {
	isUndestroyable = enable;
}

void SObject::Destroy() {
    memoryMgr->ReleaseObject(this);

}

void SObject::__FORCE_DESTROY__() {
    memoryMgr->ReleaseObject(this, true);
}

std::string SObject::GenerateMeta() {
    return nullptr;
}

void SObject::GenerateHashString() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 61); // 0~9, A~Z, a~z까지 숫자 생성

    m_hash.clear();
    m_hash.reserve(16);

    do {
        for (int i = 0; i < 16; i++) {
            int num = dis(gen);
            if (num < 10) {
                // 0~9인 경우, 숫자 문자로 추가
                m_hash += std::to_string(num);
            } else if (num < 36) {
                // 10~35인 경우, A~Z인 문자로 추가
                m_hash += static_cast<char>(num + 'A' - 10);
            } else {
                // 36~61인 경우, a~z인 문자로 추가
                m_hash += static_cast<char>(num + 'a' - 36);
            }
        }
    } while (memoryMgr->HasHash(m_hash));
}

void SObject::SetHash(std::string& hash) {
    const std::string prevHash = std::string(m_hash);
    if(hash.empty()) {
        GenerateHashString();
        memoryMgr->ChangeHash(prevHash, m_hash);
        return;
    }
    m_hash = hash;
    memoryMgr->ChangeHash(prevHash, hash);
}
