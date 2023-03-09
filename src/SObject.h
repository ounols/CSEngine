#pragma once

#include <string>

namespace CSE {

    class SObject {
    public:

        SObject();
        explicit SObject(bool isRegister);

        virtual ~SObject();

        virtual void Exterminate() = 0;

        virtual void SetUndestroyable(bool enable);

        virtual void Destroy();

        virtual void __FORCE_DESTROY__();

        virtual std::string GenerateMeta();

        std::string GetHash() const {
            return m_hash;
        }

        virtual void SetHash(std::string& hash);

    private:
        void GenerateHashString();

    private:
        bool isUndestroyable = false;

    protected:
        std::string m_hash;

    public:
        friend class MemoryMgr;
    };
}