#pragma once

#include "SComponent.h"
#include "../Util/Render/MeshSurface.h"

namespace CSE {

    class DrawableStaticMeshComponent : public SComponent {
    public:
        DrawableStaticMeshComponent();

        ~DrawableStaticMeshComponent() override;

        void Init() override;

        void Tick(float elapsedTime) override;

        void Exterminate() override;

        SComponent* Clone(SGameObject* object) override;

        virtual bool SetMesh(const SISurface& meshSurface);

        const GLMeshID& GetMeshID() const {
            return m_meshId;
        }

    protected:
        virtual void CreateMeshBuffers(const SISurface& surface);

    protected:
        GLMeshID m_meshId;
    };
}