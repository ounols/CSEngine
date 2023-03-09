#pragma once

#include <algorithm>
#include <list>
#include "SIContainer.h"

namespace CSE {

    template <class T>
    class SContainerList : public SIContainer<std::list<T>, T, int> {
    public:
        SContainerList() = default;
        ~SContainerList() override = default;

        void Register(T object) override {
            m_objects.push_back(object);
            m_size++;
        }

        void Remove(T object) override {
//            auto iObj = std::find(m_objects.begin(), m_objects.end(), object);
//            if (iObj != m_objects.end()) {
                m_objects.remove(object);
//            }
        }

        T Get(int index) const override {
            if (index < 0 || index > m_size || m_size < 1) return nullptr;
            auto iter = m_objects.begin();
            std::advance(iter, index);
            return *iter;
        }

        std::list<T> GetAll() const override {
            return m_objects;
        }

        int GetID(T object) const override {
            auto it = std::find(m_objects.begin(), m_objects.end(), object);
            if (it == m_objects.end())
                return -1;
            else
                return std::distance(m_objects.begin(), it);
        }

        int GetSize() const override {
            return m_size;
        }


    protected:
        std::list<T> m_objects;
        int m_size = 0;
    };

}