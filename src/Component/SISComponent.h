#pragma once

namespace CSE {
    class SISComponent {
    public:
        SISComponent() = default;

        SISComponent(const SISComponent& src) = default;

        virtual ~SISComponent() = default;

        virtual void Start() = 0;

        virtual void Init() = 0;

        virtual void Tick(float elapsedTime) = 0;
    };
}