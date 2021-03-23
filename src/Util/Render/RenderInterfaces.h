#pragma once
#include <vector>
#include "../../SObject.h"
#include "../../Manager/ResMgr.h"
#include "../Matrix.h"
#include "SMaterial.h"
#include "GLMeshID.h"

namespace CSE {
    class SISurface : public SResource {
    public:

        SISurface() {
            SetUndestroyable(true);
        }

        virtual ~SISurface() {
        }

        virtual int GetVertexCount() const = 0;

        virtual int GetLineIndexCount() const = 0;

        virtual int GetTriangleIndexCount() const = 0;

        virtual void GenerateVertices(std::vector<float>& vertices, unsigned char flags = 0) const = 0;

        virtual void GenerateLineIndices(std::vector<unsigned short>& indices) const = 0;

        virtual void GenerateTriangleIndices(std::vector<unsigned short>& indices) const = 0;

    public:
        mutable GLMeshID m_meshId;

    };

    class SIRender {
    public:
        SIRender() {}

        virtual ~SIRender() {}

        virtual void SetMatrix(mat4 camera, vec3 cameraPosition, mat4 projection) = 0;

        virtual void Render() const = 0;

    protected:
        short programRenderIndex = -1; //For RenderContainer
        SMaterial* material = nullptr;
        bool isRenderActive = false;

    public:
        friend class RenderContainer;
        friend class RenderMgr;
    };
}