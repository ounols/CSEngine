#pragma once

namespace CSE {
    template <class S, class T, class M>
    class SIContainer {
    public:
        SIContainer() = default;
        virtual ~SIContainer() = default;

        virtual void Register(T object) = 0;

        virtual void Remove(T object) = 0;

        virtual T Get(M index) const = 0;

        virtual S GetAll() const = 0;

        virtual M GetID(T object) const = 0;

        virtual int GetSize() const = 0;
    };
}