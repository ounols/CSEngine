#pragma once
#include "../../SObject.h"
#include <string>

namespace CSE {

    class Scene : public SObject {
    public:
        Scene();

        virtual ~Scene();

        virtual void Init() = 0;

        virtual void Tick(float elapsedTime) = 0;

        virtual void Destroy() override = 0;

        void Exterminate() override;

    protected:
        static void SetScene(Scene* scene);

    public:
        std::string m_name;
    };

}