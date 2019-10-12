#pragma once

#include <algorithm>
#include <vector>

namespace CSE {

    class SIContainer {
    public:
        SIContainer() {}

        virtual ~SIContainer() {}
    };

    template <class T>
    class SContainer : public SIContainer {
    public:
        typedef SContainer<T> BaseSContainer;
    public:
        SContainer() {
        }


        virtual ~SContainer() {
        }


        virtual void Register(T object) {
            m_objects.push_back(object);
            m_size++;
        }

        virtual void Remove(T object) {
            auto iObj = std::find(m_objects.begin(), m_objects.end(), object);

            if (iObj != m_objects.end()) {
                m_objects.erase(iObj);
            }
        }

        T Get(int index) const {
            if (index < 0 || index > m_size || m_size < 1) return nullptr;
            return m_objects[index];
        }

        int GetID(T object) const {
            auto it = std::find(m_objects.begin(), m_objects.end(), object);
            if (it == m_objects.end())
                return -1;
            else
                return std::distance(m_objects.begin(), it);
        }

        int getSize() const {
            return m_size;
        }


    protected:
        std::vector<T> m_objects;
        int m_size = 0;
    };

}