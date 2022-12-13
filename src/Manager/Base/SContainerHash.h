#pragma once

#include <unordered_map>
#include <string>
#include <algorithm>
#include "SIContainer.h"

namespace CSE {

    template <class T>
    class SContainerHash : public SIContainer<std::unordered_map<std::string, T>, T, std::string> {
    public:
        SContainerHash() = default;

        ~SContainerHash() override = default;

        void Register(T object) override {
            std::string hash = object->GetHash();
            if (m_objects.count(hash) > 0) {
                throw -1;
                return;
            }
            m_objects.insert(std::pair<std::string, T>(hash, object));
            ++m_size;
        }

        void Remove(T object) override {
            std::string hash = object->GetHash();
            if (m_objects.count(hash) <= 0) return;
            for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
                if (it->second == object) {
                    m_objects.erase(it);
                    --m_size;
                    break;
                }
            }
        }

        bool HasHash(const std::string& hash) const {
            return m_objects.count(hash) > 0;
        }

        T Get(std::string index) const override {
            return m_objects.at(index);
        }

        std::unordered_map<std::string, T> GetAll() const override {
            return m_objects;
        }

        std::string GetID(T object) const override {
            return object->GetHash();
        }

        int GetSize() const override {
            return m_size;
        }

        void ChangeHash(const std::string& srcHash, const std::string& dstHash) {
            T object = m_objects.at(srcHash);
            if(object == nullptr) return;
            m_objects.erase(srcHash);
            const auto& prevSize = m_objects.size();
            m_objects.insert(std::pair<std::string, T>(dstHash, object));
            //if(prevSize - m_objects.size() == 0) throw -1;
        }

    protected:
        std::unordered_map<std::string, T> m_objects;
        int m_size = 0;
    };

}
