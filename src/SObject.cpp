#include "SObject.h"
#include "Manager/MemoryMgr.h"
#include "Manager/EngineCore.h"
#include "Util/MoreString.h"
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
        m_hash = GetRandomHash(16);
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
